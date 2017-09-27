/*
ApocalypseCheats
------------------------------
Contributors:
XBoom
Ma$$A$
madlifer
control1337
CyclesPrograming
FZCat1337
UC Community <3

*/

#include "RageBot.h"
#include "RenderManager.h"
#include "Resolver.h"
#include "Autowall.h"
#include <iostream>
#include "UTIL Functions.h"

#define TICK_INTERVAL			( Interfaces::Globals->interval_per_tick )
#define TIME_TO_TICKS( dt )		( (int)( 0.5f + (float)(dt) / TICK_INTERVAL ) )


void CRageBot::Init()
{
	IsAimStepping = false;
	IsLocked = false;
	TargetID = -1;
}

void CRageBot::Draw()
{

}

void CorrectMovement(CUserCmd* cmd, float old_forwardmove, float old_sidemove, Vector old_angles)
{
	float delta_view, first_function, second_function;

	if (old_angles.y < 0.f) first_function = 360.0f + old_angles.y;
	else first_function = old_angles.y;

	if (cmd->viewangles.y < 0.0f) second_function = 360.0f + cmd->viewangles.y;
	else second_function = cmd->viewangles.y;

	if (second_function < first_function) delta_view = abs(second_function - first_function);
	else delta_view = 360.0f - abs(first_function - second_function);

	delta_view = 360.0f - delta_view;

	cmd->forwardmove = cos(DEG2RAD(delta_view)) * old_forwardmove + cos(DEG2RAD(delta_view + 90.f)) * old_sidemove;
	cmd->sidemove = sin(DEG2RAD(delta_view)) * old_forwardmove + sin(DEG2RAD(delta_view + 90.f)) * old_sidemove;
}
template<class T, class U>
T clamp(T in, U low, U high)
{
	if (in <= low)
		return low;

	if (in >= high)
		return high;

	return in;
}
float LagFix()
{
	//pAyy fakelag fix bro
	float updaterate = Interfaces::CVar->FindVar("cl_updaterate")->fValue;
	ConVar* minupdate = Interfaces::CVar->FindVar("sv_minupdaterate");
	ConVar* maxupdate = Interfaces::CVar->FindVar("sv_maxupdaterate");

	if (minupdate && maxupdate)
		updaterate = maxupdate->fValue;

	float ratio = Interfaces::CVar->FindVar("cl_interp_ratio")->fValue;

	if (ratio == 0)
		ratio = 1.0f;

	float lerp = Interfaces::CVar->FindVar("cl_interp")->fValue;
	ConVar* cmin = Interfaces::CVar->FindVar("sv_client_min_interp_ratio");
	ConVar* cmax = Interfaces::CVar->FindVar("sv_client_max_interp_ratio");

	if (cmin && cmax && cmin->fValue != 1)
		ratio = clamp(ratio, cmin->fValue, cmax->fValue);


	return max(lerp, ratio / updaterate);
}
bool IsAbleToShoot(IClientEntity* pLocal)
{
	CBaseCombatWeapon* pWeapon = (CBaseCombatWeapon*)Interfaces::EntList->GetClientEntityFromHandle(pLocal->GetActiveWeaponHandle());

	if (!pLocal)
		return false;

	if (!pWeapon)
		return false;

	float flServerTime = pLocal->GetTickBase() * Interfaces::Globals->interval_per_tick;

	return (!(pWeapon->GetNextPrimaryAttack() > flServerTime));
}

float hitchance(CBaseCombatWeapon* pWeapon, IClientEntity* pLocal)
{
	//	CBaseCombatWeapon* pWeapon = (CBaseCombatWeapon*)Interfaces::EntList->GetClientEntityFromHandle(pLocal->GetActiveWeaponHandle());
	float hitchance = 101;
	if (!pWeapon) return 0;
	if (Menu::Window.RageBotTab.AccuracyHitchance.GetValue() > 1)
	{//Inaccuracy method
		float inaccuracy = pWeapon->GetInaccuracy();
		if (inaccuracy == 0) inaccuracy = 0.0000001;
		inaccuracy = 1 / inaccuracy;
		hitchance = inaccuracy;

	}
	return hitchance;
}

// (DWORD)g_pNetVars->GetOffset("DT_BaseCombatWeapon", "m_flNextPrimaryAttack");
// You need something like this
bool CanOpenFire() // Creds to untrusted guy
{
	IClientEntity* pLocalEntity = (IClientEntity*)Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());
	if (!pLocalEntity)
		return false;

	CBaseCombatWeapon* entwep = (CBaseCombatWeapon*)Interfaces::EntList->GetClientEntityFromHandle(pLocalEntity->GetActiveWeaponHandle());

	float flServerTime = (float)pLocalEntity->GetTickBase() * Interfaces::Globals->interval_per_tick;
	float flNextPrimaryAttack = entwep->GetNextPrimaryAttack();

	std::cout << flServerTime << " " << flNextPrimaryAttack << std::endl;

	return !(flNextPrimaryAttack > flServerTime);
}

void CRageBot::Move(CUserCmd *pCmd, bool &bSendPacket)
{
	IClientEntity* pLocalEntity = (IClientEntity*)Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());
	if (!pLocalEntity)
		return;

	// Master switch
	if (!Menu::Window.RageBotTab.Active.GetState())
		return;

	// Anti Aim 
	if (Menu::Window.RageBotTab.AntiAimEnable.GetState())
	{
		static int ChokedPackets = -1;

		CBaseCombatWeapon* pWeapon = (CBaseCombatWeapon*)Interfaces::EntList->GetClientEntityFromHandle(hackManager.pLocal()->GetActiveWeaponHandle());
		if (!pWeapon)
			return;

		if (ChokedPackets < 1 && pLocalEntity->GetLifeState() == LIFE_ALIVE && pCmd->buttons & IN_ATTACK && CanOpenFire() && GameUtils::IsBallisticWeapon(pWeapon))
		{
			bSendPacket = false;
		}
		else
		{
			if (pLocalEntity->GetLifeState() == LIFE_ALIVE)
			{
				DoAntiAim(pCmd, bSendPacket);
			}
			ChokedPackets = -1;
		}
	}

	// Position Adjustment
	if (Menu::Window.RageBotTab.AccuracyPositionAdjustment.GetState())
		PositionAdjustment(pCmd);

	// Aimbot
	if (Menu::Window.RageBotTab.AimbotEnable.GetState())
		DoAimbot(pCmd, bSendPacket);

	// Recoil
	if (Menu::Window.RageBotTab.AccuracyRecoil.GetState())
		DoNoRecoil(pCmd);

	// Aimstep
	if (Menu::Window.RageBotTab.AimbotAimStep.GetState())
	{
		Vector AddAngs = pCmd->viewangles - LastAngle;
		if (AddAngs.Length2D() > 25.f)
		{
			Normalize(AddAngs, AddAngs);
			AddAngs *= 25;
			pCmd->viewangles = LastAngle + AddAngs;
			GameUtils::NormaliseViewAngle(pCmd->viewangles);
		}
	}

	LastAngle = pCmd->viewangles;
}

Vector BestPoint(Vector &final, IClientEntity *targetPlayer)
{
	IClientEntity* pLocal = hackManager.pLocal();

	trace_t tr;
	Ray_t ray;
	CTraceFilter filter;

	filter.pSkip = targetPlayer;
	ray.Init(final + Vector(0, 0, 10), final);
	Interfaces::Trace->TraceRay(ray, MASK_SHOT, &filter, &tr);

	final = tr.endpos;
	return final;
}

// Functionality
void CRageBot::DoAimbot(CUserCmd *pCmd, bool &bSendPacket) // Creds to encore1337 for getting it to work
{
	IClientEntity* pTarget = nullptr;
	IClientEntity* pLocal = hackManager.pLocal();
	Vector Start = pLocal->GetViewOffset() + pLocal->GetOrigin();
	bool FindNewTarget = true;
	//IsLocked = false;

	CSWeaponInfo* weapInfo = ((CBaseCombatWeapon*)Interfaces::EntList->GetClientEntityFromHandle(pLocal->GetActiveWeaponHandle()))->GetCSWpnData();

	// Don't aimbot with the knife..
	CBaseCombatWeapon* pWeapon = (CBaseCombatWeapon*)Interfaces::EntList->GetClientEntityFromHandle(pLocal->GetActiveWeaponHandle());

	if (GameUtils::IsRevolver(pWeapon) && Menu::Window.RageBotTab.AimbotAutoRevolver.GetState())
	{
		static int delay = 0;
		delay++;

		if (delay <= 15)
			pCmd->buttons |= IN_ATTACK;
		else
			delay = 0;
	}
	
	if (pWeapon)
	{
		if (pWeapon->GetAmmoInClip() == 0 || !GameUtils::IsBallisticWeapon(pWeapon))
		{
			//TargetID = 0;
			//pTarget = nullptr;
			//HitBox = -1;
			return;
		}
	}
	else
		return;

	// Make sure we have a good target
	if (IsLocked && TargetID >= 0 && HitBox >= 0)
	{
		pTarget = Interfaces::EntList->GetClientEntity(TargetID);
		if (pTarget  && TargetMeetsRequirements(pTarget))
		{
			HitBox = HitScan(pTarget);
			if (HitBox >= 0)
			{
				Vector ViewOffset = pLocal->GetOrigin() + pLocal->GetViewOffset();
				Vector View;
				Interfaces::Engine->GetViewAngles(View);
				float FoV = FovToPlayer(View, pTarget, HitBox, ViewOffset);
				if (FoV < Menu::Window.RageBotTab.AimbotFov.GetValue())
					FindNewTarget = false;
			}
		}
	}

	// Find a new target, apparently we need to
	if (FindNewTarget)
	{
		TargetID = 0;
		pTarget = nullptr;
		HitBox = -1;

		// Target selection type
		switch (Menu::Window.RageBotTab.TargetSelection.GetIndex())
		{
		case 0:
			TargetID = GetTargetCrosshair();
			break;
		case 1:
			TargetID = GetTargetDistance();
			break;
		case 2:
			TargetID = GetTargetHealth();
			break;
		}

		// Memes
		if (TargetID >= 0)
		{
			pTarget = Interfaces::EntList->GetClientEntity(TargetID);
		}
		else
		{
			pTarget = nullptr;
			HitBox = -1;
		}
	}

	Globals::Target = pTarget;
	Globals::TargetID = TargetID;

	// If we finally have a good target
	if (TargetID >= 0 && pTarget)
	{
		// Get the hitbox to shoot at
		HitBox = HitScan(pTarget);

		if (!CanOpenFire())
			return;

		// Key
		if (Menu::Window.RageBotTab.AimbotKeyPress.GetState())
		{
			int Key = Menu::Window.RageBotTab.AimbotKeyBind.GetKey();
			if (Key >= 0 && !GUI.GetKeyState(Key))
			{
				TargetID = -1;
				pTarget = nullptr;
				HitBox = -1;
				return;
			}
		}

		// Stop key
		int StopKey = Menu::Window.RageBotTab.AimbotStopKey.GetKey();
		if (StopKey >= 0 && GUI.GetKeyState(StopKey))
		{
			TargetID = -1;
			pTarget = nullptr;
			HitBox = -1;
			return;
		}

		float pointscale = Menu::Window.RageBotTab.TargetPointscale.GetValue() - 5.f; // Aim height
																					  //		float value = Menu::Window.RageBotTab.AccuracyHitchance.GetValue(); // Hitchance

		Vector Point;
		Vector AimPoint = GetHitboxPosition(pTarget, HitBox) + Vector(0, 0, pointscale);

		if (Menu::Window.RageBotTab.TargetMultipoint.GetState())
		{
			Point = BestPoint(AimPoint, pTarget);
		}
		else
		{
			Point = AimPoint;
		}

		// Lets open fire
		if (GameUtils::IsScopedWeapon(pWeapon) && !pWeapon->IsScoped() && Menu::Window.RageBotTab.AccuracyAutoScope.GetState()) // Autoscope
		{
			pCmd->buttons |= IN_ATTACK2;
		}
		else
		{
			if ((Menu::Window.RageBotTab.AccuracyHitchance.GetValue() * 1.5 <= hitchance(pWeapon, pLocal)) || Menu::Window.RageBotTab.AccuracyHitchance.GetValue() == 0 || *pWeapon->m_AttributeManager()->m_Item()->ItemDefinitionIndex() == 64)
			{
				if (AimAtPoint(Point, pCmd, bSendPacket, pLocal))
				{
					if (Menu::Window.RageBotTab.AimbotAutoFire.GetState() && !(pCmd->buttons & IN_ATTACK))
					{

						pCmd->buttons |= IN_ATTACK;
					}
					else
					{
						return;
					}
				}
				else if (Menu::Window.RageBotTab.AimbotAutoFire.GetState() && !(pCmd->buttons & IN_ATTACK))
				{

					pCmd->buttons |= IN_ATTACK;
				}
			}
		}

		if (IsAbleToShoot(pLocal) && pCmd->buttons & IN_ATTACK)
			Globals::Shots += 1;

		// Stop and Crouch
		if (TargetID >= 0 && pTarget)
		{
			if (Menu::Window.RageBotTab.AccuracyAutoStop.GetState())
			{
				pCmd->forwardmove = 0.f;
				pCmd->sidemove = 0.f;	
			}
			if (Menu::Window.RageBotTab.AccuracyAutoCrouch.GetState())
			{
				pCmd->buttons = IN_DUCK;
			}

		}
	}

	// Auto Pistol
	if (GameUtils::IsPistol(pWeapon) && Menu::Window.RageBotTab.AimbotAutoPistol.GetState())
	{
		if (pCmd->buttons & IN_ATTACK)
		{
			static bool WasFiring = false;
			WasFiring = !WasFiring;

			if (WasFiring)
			{
				Sleep(Menu::Window.RageBotTab.AimbotAutoPistolD.GetValue());
				pCmd->buttons &= ~IN_ATTACK;
			}
		}
	}
}

bool CRageBot::TargetMeetsRequirements(IClientEntity* pEntity)
{
	// Is a valid player
	if (pEntity && pEntity->IsDormant() == false && pEntity->IsAlive() && pEntity->GetIndex() != hackManager.pLocal()->GetIndex())
	{
		// Entity Type checks
		ClientClass *pClientClass = pEntity->GetClientClass();
		player_info_t pinfo;
		if (pClientClass->m_ClassID == (int)CSGOClassID::CCSPlayer && Interfaces::Engine->GetPlayerInfo(pEntity->GetIndex(), &pinfo))
		{
			// Team Check
			if (pEntity->GetTeamNum() != hackManager.pLocal()->GetTeamNum() || Menu::Window.RageBotTab.TargetFriendlyFire.GetState())
			{
				// Spawn Check
				if (!pEntity->HasGunGameImmunity())
				{
					return true;
				}
			}
		}
	}

	// They must have failed a requirement
	return false;
}

float CRageBot::FovToPlayer(Vector View, IClientEntity* pEntity, int aHitBox, Vector ViewOffSet)
{
	// Anything past 180 degrees is just going to wrap around
	CONST FLOAT MaxDegrees = 180.0f;

	// Get local angles
	Vector Angles = View;

	// Get local view / eye position
	Vector Origin = ViewOffSet;

	// Create and intiialize vectors for calculations below
	Vector Delta(0, 0, 0);
	//Vector Origin(0, 0, 0);
	Vector Forward(0, 0, 0);

	// Convert angles to normalized directional forward vector
	AngleVectors(Angles, &Forward);
	Vector AimPos = GetHitboxPosition(pEntity, aHitBox);
	// Get delta vector between our local eye position and passed vector
	VectorSubtract(AimPos, Origin, Delta);
	//Delta = AimPos - Origin;

	// Normalize our delta vector
	Normalize(Delta, Delta);

	// Get dot product between delta position and directional forward vectors
	FLOAT DotProduct = Forward.Dot(Delta);

	// Time to calculate the field of view
	return (acos(DotProduct) * (MaxDegrees / PI));
}

int CRageBot::GetTargetCrosshair()
{
	// Target selection
	int target = -1;
	float minFoV = Menu::Window.RageBotTab.AimbotFov.GetValue();

	IClientEntity* pLocal = hackManager.pLocal();
	Vector ViewOffset = pLocal->GetOrigin() + pLocal->GetViewOffset();
	Vector View; Interfaces::Engine->GetViewAngles(View);

	for (int i = 0; i < Interfaces::EntList->GetMaxEntities(); i++) //GetHighestEntityIndex()
	{
		IClientEntity *pEntity = Interfaces::EntList->GetClientEntity(i);
		if (TargetMeetsRequirements(pEntity))
		{
			int NewHitBox = HitScan(pEntity);
			if (NewHitBox >= 0)
			{
				float fov = FovToPlayer(View, pEntity, 0, ViewOffset);
				if (fov < minFoV)
				{
					minFoV = fov;
					target = i;
				}
			}
		}
	}

	return target;
}

int CRageBot::GetTargetDistance()
{
	// Target selection
	int target = -1;
	int minDist = 99999;

	IClientEntity* pLocal = hackManager.pLocal();
	Vector ViewOffset = pLocal->GetOrigin() + pLocal->GetViewOffset();
	Vector View; Interfaces::Engine->GetViewAngles(View);

	for (int i = 0; i < Interfaces::EntList->GetMaxEntities(); i++)
	{
		IClientEntity *pEntity = Interfaces::EntList->GetClientEntity(i);
		if (TargetMeetsRequirements(pEntity))
		{
			int NewHitBox = HitScan(pEntity);
			if (NewHitBox >= 0)
			{
				Vector Difference = pLocal->GetOrigin() - pEntity->GetOrigin();
				int Distance = Difference.Length();
				float fov = FovToPlayer(View, pEntity, 0, ViewOffset);
				if (Distance < minDist && fov < Menu::Window.RageBotTab.AimbotFov.GetValue())
				{
					minDist = Distance;
					target = i;
				}
			}
		}
	}

	return target;
}

int CRageBot::GetTargetHealth()
{
	// Target selection
	int target = -1;
	int minHealth = 101;

	IClientEntity* pLocal = hackManager.pLocal();
	Vector ViewOffset = pLocal->GetOrigin() + pLocal->GetViewOffset();
	Vector View; Interfaces::Engine->GetViewAngles(View);

	for (int i = 0; i < Interfaces::EntList->GetMaxEntities(); i++)
	{
		IClientEntity *pEntity = Interfaces::EntList->GetClientEntity(i);
		if (TargetMeetsRequirements(pEntity))
		{
			int NewHitBox = HitScan(pEntity);
			if (NewHitBox >= 0)
			{
				int Health = pEntity->GetHealth();
				float fov = FovToPlayer(View, pEntity, 0, ViewOffset);
				if (Health < minHealth && fov < Menu::Window.RageBotTab.AimbotFov.GetValue())
				{
					minHealth = Health;
					target = i;
				}
			}
		}
	}

	return target;
}

int CRageBot::HitScan(IClientEntity* pEntity)
{
	IClientEntity* pLocal = hackManager.pLocal();
	std::vector<int> HitBoxesToScan;

	// Get the hitboxes to scan
#pragma region GetHitboxesToScan
	int HitScanMode = Menu::Window.RageBotTab.TargetHitscan.GetIndex();
	int iSmart = Menu::Window.RageBotTab.AccuracySmart.GetValue();
	bool AWall = Menu::Window.RageBotTab.AccuracyAutoWall.GetState();
	bool Multipoint = Menu::Window.RageBotTab.TargetMultipoint.GetState();

	if (HitScanMode == 0)
	{
		switch (Menu::Window.RageBotTab.TargetHitbox.GetIndex()) /*Single-Hitbox*/
		{
		case 0:
			HitBoxesToScan.push_back((int)CSGOHitboxID::Head);
			break;
		case 1:
			HitBoxesToScan.push_back((int)CSGOHitboxID::Neck);
			HitBoxesToScan.push_back((int)CSGOHitboxID::NeckLower);
			break;
		case 2:
			HitBoxesToScan.push_back((int)CSGOHitboxID::UpperChest);
			HitBoxesToScan.push_back((int)CSGOHitboxID::Chest);
			break;
		case 3:
			HitBoxesToScan.push_back((int)CSGOHitboxID::Stomach);
			HitBoxesToScan.push_back((int)CSGOHitboxID::Pelvis);
			break;
		case 4:
			HitBoxesToScan.push_back((int)CSGOHitboxID::RightShin);
			HitBoxesToScan.push_back((int)CSGOHitboxID::LeftShin);
			break;
		}
	}
	else
	{
		switch (HitScanMode)
		{
		case 1:
			/*Scan-Normal*/
			HitBoxesToScan.push_back((int)CSGOHitboxID::Head);
			HitBoxesToScan.push_back((int)CSGOHitboxID::Neck);
			HitBoxesToScan.push_back((int)CSGOHitboxID::Stomach);
			HitBoxesToScan.push_back((int)CSGOHitboxID::Pelvis);
			HitBoxesToScan.push_back((int)CSGOHitboxID::UpperChest);
			HitBoxesToScan.push_back((int)CSGOHitboxID::Chest);
			HitBoxesToScan.push_back((int)CSGOHitboxID::LeftUpperArm);
			HitBoxesToScan.push_back((int)CSGOHitboxID::RightUpperArm);
			HitBoxesToScan.push_back((int)CSGOHitboxID::LeftThigh);
			HitBoxesToScan.push_back((int)CSGOHitboxID::RightThigh);
			break;
		case 2:
			/*Scan-High*/
			HitBoxesToScan.push_back((int)CSGOHitboxID::Head);
			HitBoxesToScan.push_back((int)CSGOHitboxID::Neck);
			HitBoxesToScan.push_back((int)CSGOHitboxID::Stomach);
			HitBoxesToScan.push_back((int)CSGOHitboxID::Pelvis);
			HitBoxesToScan.push_back((int)CSGOHitboxID::UpperChest);
			HitBoxesToScan.push_back((int)CSGOHitboxID::Chest);
			HitBoxesToScan.push_back((int)CSGOHitboxID::LeftUpperArm);
			HitBoxesToScan.push_back((int)CSGOHitboxID::RightUpperArm);
			HitBoxesToScan.push_back((int)CSGOHitboxID::LeftThigh);
			HitBoxesToScan.push_back((int)CSGOHitboxID::RightThigh);
			HitBoxesToScan.push_back((int)CSGOHitboxID::LeftShin);
			HitBoxesToScan.push_back((int)CSGOHitboxID::RightShin);
			HitBoxesToScan.push_back((int)CSGOHitboxID::LeftLowerArm);
			HitBoxesToScan.push_back((int)CSGOHitboxID::RightLowerArm);
		case 3:
			/*Scan-All*/
			HitBoxesToScan.push_back((int)CSGOHitboxID::Head);
			HitBoxesToScan.push_back((int)CSGOHitboxID::Neck);
			HitBoxesToScan.push_back((int)CSGOHitboxID::NeckLower);
			HitBoxesToScan.push_back((int)CSGOHitboxID::Stomach);
			HitBoxesToScan.push_back((int)CSGOHitboxID::Pelvis);
			HitBoxesToScan.push_back((int)CSGOHitboxID::UpperChest);
			HitBoxesToScan.push_back((int)CSGOHitboxID::Chest);
			HitBoxesToScan.push_back((int)CSGOHitboxID::LowerChest);
			HitBoxesToScan.push_back((int)CSGOHitboxID::LeftUpperArm);
			HitBoxesToScan.push_back((int)CSGOHitboxID::RightUpperArm);
			HitBoxesToScan.push_back((int)CSGOHitboxID::LeftThigh);
			HitBoxesToScan.push_back((int)CSGOHitboxID::RightThigh);
			HitBoxesToScan.push_back((int)CSGOHitboxID::LeftHand);
			HitBoxesToScan.push_back((int)CSGOHitboxID::RightHand);
			HitBoxesToScan.push_back((int)CSGOHitboxID::LeftFoot);
			HitBoxesToScan.push_back((int)CSGOHitboxID::RightFoot);
			HitBoxesToScan.push_back((int)CSGOHitboxID::LeftShin);
			HitBoxesToScan.push_back((int)CSGOHitboxID::RightShin);
			HitBoxesToScan.push_back((int)CSGOHitboxID::LeftLowerArm);
			HitBoxesToScan.push_back((int)CSGOHitboxID::RightLowerArm);
		case 4:
			HitBoxesToScan.push_back((int)CSGOHitboxID::Stomach);
			HitBoxesToScan.push_back((int)CSGOHitboxID::Pelvis);
			HitBoxesToScan.push_back((int)CSGOHitboxID::UpperChest);
			HitBoxesToScan.push_back((int)CSGOHitboxID::Chest);
			HitBoxesToScan.push_back((int)CSGOHitboxID::LowerChest);
			HitBoxesToScan.push_back((int)CSGOHitboxID::LeftFoot);
			HitBoxesToScan.push_back((int)CSGOHitboxID::RightFoot);
		}
	}
#pragma endregion Get the list of shit to scan

	// check hits
	// check hits
	for (auto HitBoxID : HitBoxesToScan)
	{
		if (AWall)
		{
			Vector Point = GetHitboxPosition(pEntity, HitBoxID);
			float Damage = 0.f;
			Color c = Color(255, 255, 255, 255);
			if (CanHit(&Damage, Point))
			{
				c = Color(0, 255, 0, 255);
				if (Damage >= Menu::Window.RageBotTab.AccuracyMinimumDamage.GetValue())
				{
					return HitBoxID;
				}
			}
		}
		else
		{
			if (GameUtils::IsVisible(hackManager.pLocal(), pEntity, HitBoxID))
				return HitBoxID;
		}
	}

	return -1;
}

void CRageBot::PositionAdjustment(CUserCmd* pCmd)
{
	static ConVar* cvar_cl_interp = Interfaces::CVar->FindVar("cl_interp");
	static ConVar* cvar_cl_updaterate = Interfaces::CVar->FindVar("cl_updaterate");
	static ConVar* cvar_sv_maxupdaterate = Interfaces::CVar->FindVar("sv_maxupdaterate");
	static ConVar* cvar_sv_minupdaterate = Interfaces::CVar->FindVar("sv_minupdaterate");
	static ConVar* cvar_cl_interp_ratio = Interfaces::CVar->FindVar("cl_interp_ratio");

	IClientEntity* pLocal = hackManager.pLocal();

	if (!pLocal)
		return;

	CBaseCombatWeapon* pWeapon = (CBaseCombatWeapon*)Interfaces::EntList->GetClientEntityFromHandle(hackManager.pLocal()->GetActiveWeaponHandle());
	if (!pWeapon)
		return;

	float cl_interp = cvar_cl_interp->GetFloat();
	int cl_updaterate = cvar_cl_updaterate->GetInt();
	int sv_maxupdaterate = cvar_sv_maxupdaterate->GetInt();
	int sv_minupdaterate = cvar_sv_minupdaterate->GetInt();
	int cl_interp_ratio = cvar_cl_interp_ratio->GetInt();

	if (sv_maxupdaterate <= cl_updaterate)
		cl_updaterate = sv_maxupdaterate;

	if (sv_minupdaterate > cl_updaterate)
		cl_updaterate = sv_minupdaterate;

	float new_interp = (float)cl_interp_ratio / (float)cl_updaterate;

	if (new_interp > cl_interp)
		cl_interp = new_interp;

	float flSimTime = pLocal->GetSimulationTime();
	float flOldAnimTime = pLocal->GetAnimTime();

	int iTargetTickDiff = (int)(0.5f + (flSimTime - flOldAnimTime) / Interfaces::Globals->interval_per_tick);

	int result = (int)floorf(TIME_TO_TICKS(cl_interp)) + (int)floorf(TIME_TO_TICKS(pLocal->GetSimulationTime()));

	if ((result - pCmd->tick_count) >= -50)
	{
		pCmd->tick_count = result;
	}
}

void CRageBot::DoNoRecoil(CUserCmd *pCmd)
{
	// Ghetto rcs shit, implement properly later
	IClientEntity* pLocal = hackManager.pLocal();
	if (pLocal)
	{
		Vector AimPunch = pLocal->localPlayerExclusive()->GetAimPunchAngle();
		if (AimPunch.Length2D() > 0 && AimPunch.Length2D() < 150)
		{
			pCmd->viewangles -= AimPunch * 2;
			GameUtils::NormaliseViewAngle(pCmd->viewangles);
		}
	}
}

bool CRageBot::AimAtPoint(Vector point, CUserCmd *pCmd, bool &bSendPacket, IClientEntity* pLocal)
{
	bool ReturnValue = false;
	// Get the full angles
	if (point.Length() == 0) return ReturnValue;

	Vector angles;
	Vector src = pLocal->GetOrigin() + pLocal->GetViewOffset();

	CalcAngle(src, point, angles);
	GameUtils::NormaliseViewAngle(angles);

	if (angles[0] != angles[0] || angles[1] != angles[1])
	{
		return ReturnValue;
	}


	IsLocked = true;
	//-----------------------------------------------

	// Aim Step Calcs
	Vector ViewOffset = pLocal->GetOrigin() + pLocal->GetViewOffset();
	if (!IsAimStepping)
		LastAimstepAngle = LastAngle; // Don't just use the viewangs because you need to consider aa

	float fovLeft = FovToPlayer(LastAimstepAngle, Interfaces::EntList->GetClientEntity(TargetID), 0, ViewOffset);

	if (fovLeft > 25.0f && Menu::Window.RageBotTab.AimbotAimStep.GetState())
	{
		Vector AddAngs = angles - LastAimstepAngle;
		Normalize(AddAngs, AddAngs);
		AddAngs *= 25;
		LastAimstepAngle += AddAngs;
		GameUtils::NormaliseViewAngle(LastAimstepAngle);
		angles = LastAimstepAngle;
	}
	else
	{
		ReturnValue = true;
	}

	// Silent Aim
	if (Menu::Window.RageBotTab.AimbotSilentAim.GetState() && !Menu::Window.RageBotTab.AimbotPerfectSilentAim.GetState())
	{
		pCmd->viewangles = angles;
	}

	// Normal Aim
	if (!Menu::Window.RageBotTab.AimbotSilentAim.GetState() && !Menu::Window.RageBotTab.AimbotPerfectSilentAim.GetState())
	{
		Interfaces::Engine->SetViewAngles(angles);
	}

	// pSilent Aim 
	Vector Oldview = pCmd->viewangles;

	if (Menu::Window.RageBotTab.AimbotPerfectSilentAim.GetState())
	{
		static int ChokedPackets = -1;
		ChokedPackets++;

		if (ChokedPackets < 6)
		{
			bSendPacket = false;
			pCmd->viewangles = angles;
		}
		else
		{
			bSendPacket = true;
			pCmd->viewangles = Oldview;
			ChokedPackets = -1;
			ReturnValue = false;
		}

		//pCmd->viewangles.z = 0;
	}

	return ReturnValue;
}

namespace AntiAims // CanOpenFire checks for fake anti aims?
{
	// Pitches

	void JitterPitch(CUserCmd *pCmd)
	{
		static bool up = true;
		if (up)
		{
			pCmd->viewangles.x = 45;
			up = !up;
		}
		else
		{
			pCmd->viewangles.x = 89;
			up = !up;
		}
	}
	void fakepitch1(CUserCmd *pCmd, bool &bSendPacket)
	{
		static bool fake = true;
		static int emotion = pCmd->viewangles.x = 89;
		static bool switchthisyaw = !switchthisyaw;
		int aapitch = pCmd->viewangles.x;

		if (switchthisyaw = false)
		{
			pCmd->viewangles.x = emotion;
			switchthisyaw;
		}
		else (bSendPacket && !switchthisyaw);
		{
			aapitch = 180.9f;
			!switchthisyaw;
		}

	}
	void FakePitch(CUserCmd *pCmd, bool &bSendPacket)
	{
		static int ChokedPackets = -1;
		ChokedPackets++;
		if (ChokedPackets < 1)
		{
			bSendPacket = false;
			pCmd->viewangles.x = 89;
		}
		else
		{
			bSendPacket = true;
			pCmd->viewangles.x = 51;
			ChokedPackets = -1;
		}
	}

	void StaticJitter(CUserCmd *pCmd)
	{
		static bool down = true;
		if (down)
		{
			pCmd->viewangles.x = 179.0f;
			down = !down;
		}
		else
		{
			pCmd->viewangles.x = 89.0f;
			down = !down;
		}
	}

	// Yaws



	void FakeEdge(CUserCmd *pCmd, bool &bSendPacket)
	{
		IClientEntity* pLocal = hackManager.pLocal();

		Vector vEyePos = pLocal->GetOrigin() + pLocal->GetViewOffset();

		CTraceFilter filter;
		filter.pSkip = Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());

		for (int y = 0; y < 360; y++)
		{
			Vector qTmp(10.0f, pCmd->viewangles.y, 0.0f);
			qTmp.y += y;

			if (qTmp.y > 180.0)
				qTmp.y -= 360.0;
			else if (qTmp.y < -180.0)
				qTmp.y += 360.0;

			GameUtils::NormaliseViewAngle(qTmp);

			Vector vForward;

			VectorAngles(qTmp, vForward);

			float fLength = (19.0f + (19.0f * sinf(DEG2RAD(10.0f)))) + 7.0f;
			vForward *= fLength;

			trace_t tr;

			Vector vTraceEnd = vEyePos + vForward;

			Ray_t ray;

			ray.Init(vEyePos, vTraceEnd);
			Interfaces::Trace->TraceRay(ray, MASK_PLAYERSOLID_BRUSHONLY, &filter, &tr);

			if (tr.fraction != 1.0f)
			{
				Vector angles;

				Vector vNegative = Vector(tr.plane.normal.x * -1.0f, tr.plane.normal.y * -1.0f, tr.plane.normal.z * -1.0f);

				VectorAngles(vNegative, angles);

				GameUtils::NormaliseViewAngle(angles);

				qTmp.y = angles.y;

				GameUtils::NormaliseViewAngle(qTmp);

				trace_t trLeft, trRight;

				Vector vLeft, vRight;
				VectorAngles(qTmp + Vector(0.0f, 30.0f, 0.0f), vLeft);
				VectorAngles(qTmp + Vector(0.0f, 30.0f, 0.0f), vRight);

				vLeft *= (fLength + (fLength * sinf(DEG2RAD(30.0f))));
				vRight *= (fLength + (fLength * sinf(DEG2RAD(30.0f))));

				vTraceEnd = vEyePos + vLeft;

				ray.Init(vEyePos, vTraceEnd);
				Interfaces::Trace->TraceRay(ray, MASK_PLAYERSOLID_BRUSHONLY, &filter, &trLeft);

				vTraceEnd = vEyePos + vRight;

				ray.Init(vEyePos, vTraceEnd);
				Interfaces::Trace->TraceRay(ray, MASK_PLAYERSOLID_BRUSHONLY, &filter, &trRight);

				if ((trLeft.fraction == 1.0f) && (trRight.fraction != 1.0f))
					qTmp.y -= 90.f;
				else if ((trLeft.fraction != 1.0f) && (trRight.fraction == 1.0f))
					qTmp.y += 90.f;

				if (qTmp.y > 180.0)
					qTmp.y -= 360.0;
				else if (qTmp.y < -180.0)
					qTmp.y += 360.0;

				pCmd->viewangles.y = qTmp.y;

				int offset = Menu::Window.RageBotTab.AntiAimOffset.GetValue();

				static int ChokedPackets = -1;
				ChokedPackets++;
				if (ChokedPackets < 1)
				{
					bSendPacket = false; // +=180?
				}
				else
				{
					bSendPacket = true;
					pCmd->viewangles.y -= offset;
					ChokedPackets = -1;
				}
				return;
			}
		}
		pCmd->viewangles.y += 360.0f;
	}

	void BackJitter(CUserCmd *pCmd)
	{

		switch ((int)rand % 5)
		{
		case 0:
			pCmd->viewangles.y -= 180;
		case 1:
			pCmd->viewangles.y -= 180;
		case 2:
			pCmd->viewangles.y -= 180;
		case 3:
			switch ((int)rand % 2)
			{
			case 0:
				pCmd->viewangles.y -= (int)rand % 45;
			case 1:
				pCmd->viewangles.y = (int)rand % 180;
			}
		case 4:
			pCmd->viewangles.y -= (int)rand % 30;
		}
	}
	void lbynibybraekr(CUserCmd* pCmd, bool &bSendPacket)
	{
		static float StartLbyBreaker;
		static float LastLBYUpdateTime;
		static float lbyDelta;
		static int LBYBreakerTimer1 = 1;
		float realAngle = pCmd->viewangles.y + 180.0f;;
		float fakeAngle;
		float lby;
		static int random;
		random = rand() % 2;


		// FAKE ANGLE ROFL ROFL ROFL
		if (random == 1) {
			fakeAngle = pCmd->viewangles.y - 90.0f;

		}
		else {
			fakeAngle = pCmd->viewangles.y + 90.0f;

		}


		// ROFL ROFL I KNOW
		while (realAngle < -180.0f)
			realAngle += 360.0f;

		while (realAngle > 180.0f)
			realAngle -= 360.0f;

		while (fakeAngle < -180.0f)
			fakeAngle += 360.0f;

		while (fakeAngle > 180.0f)
			fakeAngle -= 360.0f;


		// Real Angle should always be more then 35° FAKE Angle & LBY ; 45 because most bruteforce ur lby + 40°
		if (fabsf(realAngle - lby) > 45 && fabsf(realAngle - fakeAngle) > 45) {
			realAngle = realAngle;
		}

		if (fabsf(realAngle - lby) < 45 || fabsf(realAngle - fakeAngle) < 45) {
			realAngle += 45;
		}

		// IF LBY Update from Proxy then fake Angle "Peek"... like a fagg

		if (StartLbyBreaker != LBYBreakerTimer1)
		{
			bSendPacket = true;
			StartLbyBreaker = LBYBreakerTimer1;
			pCmd->viewangles.y = fakeAngle;
		}
		else
		{
			pCmd->viewangles.y = realAngle;
		}
	}
	void Backward(CUserCmd* cmd)
	{
		QAngle OriginalY = cmd->viewangles;
		cmd->viewangles.y = OriginalY.y + 180.0;
	}

	void Sideways(CUserCmd* cmd)
	{
		QAngle v37 = cmd->viewangles;
		cmd->viewangles.y = v37.y + 90.0;
	}
	void fake180elo(CUserCmd* pCmd, bool &bSendPacket)
	{
		static int flip; +!flip;
		static int flip2 = !flip2;
		{
			if (flip)
			{
				if (flip2)
				{
					bSendPacket = false;
					pCmd->viewangles.y -= 180.000000f;
				}
			}
			else
			{
				bSendPacket = true;
				if (flip2)
					pCmd->viewangles.y += 131.000000f;
				else if (!flip2)
					pCmd->viewangles.y -= 131.000000f;
			}

		}
	}
	void fake2(CUserCmd* pCmd, bool& bSendPacket)
	{
#define RandomInt(min, max) (rand() % (max - min + 1) + min)
		int flip = (int)floorf(Interfaces::Globals->curtime / 3) % 2;
		static bool lowerbody;
		lowerbody = !lowerbody;
		if (lowerbody)
		{
			bSendPacket = true;
			int rand2;
			static bool flipit;
			rand2 = RandomInt(1, 100);
			if (flipit) {
				if (rand2 < 2) {
					pCmd->viewangles.y -= hackManager.pLocal()->GetLowerBodyYaw() - 260.27 + flip * hackManager.pLocal()->GetLowerBodyYaw() - 184.86;
				}
				else
				{
					pCmd->viewangles.y += flip * hackManager.pLocal()->GetLowerBodyYaw() - 260.27;
				}
				flipit = false;
			}
			else {
				if (rand2 < 2) {
					pCmd->viewangles.y += -260.27 + flip * 184.86;
				}
				else
				{
					pCmd->viewangles.y += flip * 260.27;
				}
				flipit = true;
			}
			lowerbody = false;
		}
		else {
			bSendPacket = false;
			pCmd->viewangles.y += 180;
			lowerbody = true;
		}
	}
	void FakeSideways(CUserCmd *pCmd, bool &bSendPacket)
	{
		static int ChokedPackets = -1;
		ChokedPackets++;
		if (ChokedPackets < 1)
		{
			bSendPacket = false;
			pCmd->viewangles.y += 90;
		}
		else
		{
			bSendPacket = true;
			pCmd->viewangles.y -= 180;
			ChokedPackets = -1;
		}
	}
	void fakelowerbody2(CUserCmd *pCmd, bool &bSendPacket)
	{
		static int ChokedPackets = -1;
		ChokedPackets++;
#define RandomInt(min, max) (rand() % (max - min + 1) + min)
		if (ChokedPackets < 1)
		{
			bSendPacket = false;
			pCmd->viewangles.y -= hackManager.pLocal()->GetLowerBodyYaw() + RandomInt(30, 61);
		}
		else
		{
			bSendPacket = true;
			pCmd->viewangles.y += hackManager.pLocal()->GetLowerBodyYaw() - RandomInt(180, 360);
			ChokedPackets = -1;
		}
	}
	void Lbycustom(CUserCmd* pCmd, bool& bSendPacket)
	{

	}
	void pLBYbyDEDSEE(CUserCmd* pCmd, bool& bSendPacket)
	{
		static bool ySwitch;
		static bool jbool;
		static bool jboolt;
		ySwitch = !ySwitch;
		jbool = !jbool;
		jboolt = !jbool;
		if (ySwitch)
		{
			if (jbool)
			{
				if (jboolt)
				{
					pCmd->viewangles.y = hackManager.pLocal()->GetLowerBodyYaw() - 87.554f;
					bSendPacket = false;
				}
				else
				{
					pCmd->viewangles.y = hackManager.pLocal()->GetLowerBodyYaw() + 93.946f;
					bSendPacket = false;
				}
			}
			else
			{
				if (jboolt)
				{
					pCmd->viewangles.y = hackManager.pLocal()->GetLowerBodyYaw() - 126.446f;
					bSendPacket = false;
				}
				else
				{
					pCmd->viewangles.y = hackManager.pLocal()->GetLowerBodyYaw() + 124.874f;
					bSendPacket = false;
				}
			}
		}
		else
		{
			pCmd->viewangles.y = hackManager.pLocal()->GetLowerBodyYaw();
			bSendPacket = true;
		}
	}
	void fake5(CUserCmd* pCmd, bool& bSendPacket)
	{
		static bool ySwitch;
		static bool jbool;
		static bool jboolt;
		ySwitch = !ySwitch;
		jbool = !jbool;
		jboolt = !jbool;
		if (ySwitch)
		{
			if (bSendPacket)
			{
				if (jboolt)
				{
					pCmd->viewangles.y = hackManager.pLocal()->GetLowerBodyYaw() - 90.00f;
					bSendPacket = false;
				}
				else
				{
					pCmd->viewangles.y = hackManager.pLocal()->GetLowerBodyYaw() + 75.45f;
					bSendPacket = false;
				}
			}
			else
			{
				if (ySwitch)
				{
					pCmd->viewangles.y = hackManager.pLocal()->GetLowerBodyYaw() - 125.00f;
					bSendPacket = false;
				}
				else
				{
					pCmd->viewangles.y = hackManager.pLocal()->GetLowerBodyYaw() + 180.00f;
					bSendPacket = false;
				}
			}
		}
		else
		{
			pCmd->viewangles.y = hackManager.pLocal()->GetLowerBodyYaw();
			bSendPacket = true;
		}
	}
	void skeet(CUserCmd* pCmd, bool& bSendPacket)
	{
		static bool pJitter;
		if (pJitter <= 1)
		{
			pCmd->viewangles.x = 88.000000;
			pCmd->viewangles.y += 135.000000;
		}
		else if (pJitter > 1 && pJitter <= 3)
		{
			pCmd->viewangles.x = 88.000000;
			pCmd->viewangles.y += 22.000000;
		}
		if (pCmd->tick_count % 3)
		{
			bSendPacket = false;
			if (pJitter <= 1)
			{
				pCmd->viewangles.y += 60.000000;
				pJitter += 1;
			}
			else if (pJitter > 1 && pJitter <= 3)
			{
				pCmd->viewangles.y -= 55.000000;
				pJitter += 1;
			}
			else
			{
				pJitter = 0;
			}
		}
		else
		{
			bSendPacket = true;
		}
	}
	void Jitter(CUserCmd *pCmd)
	{
		static int jitterangle = 0;

		if (jitterangle <= 1)
		{
			pCmd->viewangles.y += 90;
		}
		else if (jitterangle > 1 && jitterangle <= 3)
		{
			pCmd->viewangles.y -= 90;
		}

		int re = rand() % 4 + 1;


		if (jitterangle <= 1)
		{
			if (re == 4)
				pCmd->viewangles.y += 180;
			jitterangle += 1;
		}
		else if (jitterangle > 1 && jitterangle <= 3)
		{
			if (re == 4)
				pCmd->viewangles.y -= 180;
			jitterangle += 1;
		}
		else
		{
			jitterangle = 0;
		}
	}

	void FakeStatic(CUserCmd *pCmd, bool &bSendPacket)
	{
		static int ChokedPackets = -1;
		ChokedPackets++;
		if (ChokedPackets < 1)
		{
			bSendPacket = false;
			static int y2 = -179;
			int spinBotSpeedFast = 360.0f / 1.618033988749895f;;

			y2 += spinBotSpeedFast;

			if (y2 >= 179)
				y2 = -179;

			pCmd->viewangles.y = y2;
		}
		else
		{
			bSendPacket = true;
			pCmd->viewangles.y -= 180;
			ChokedPackets = -1;
		}
	}

	void TJitter(CUserCmd *pCmd)
	{
		static bool Turbo = true;
		if (Turbo)
		{
			pCmd->viewangles.y -= 90;
			Turbo = !Turbo;
		}
		else
		{
			pCmd->viewangles.y += 90;
			Turbo = !Turbo;
		}
	}

	void TFake(CUserCmd *pCmd, bool &bSendPacket)
	{
		static int ChokedPackets = -1;
		ChokedPackets++;
		if (ChokedPackets < 1)
		{
			bSendPacket = false;
			pCmd->viewangles.y = -90;
		}
		else
		{
			bSendPacket = true;
			pCmd->viewangles.y = 90;
			ChokedPackets = -1;
		}
	}

	void FakeJitter(CUserCmd* pCmd, bool &bSendPacket)
	{
		static int jitterangle = 0;

		if (jitterangle <= 1)
		{
			pCmd->viewangles.y += 135;
		}
		else if (jitterangle > 1 && jitterangle <= 3)
		{
			pCmd->viewangles.y += 225;
		}

		static int iChoked = -1;
		iChoked++;
		if (iChoked < 1)
		{
			bSendPacket = false;
			if (jitterangle <= 1)
			{
				pCmd->viewangles.y += 45;
				jitterangle += 1;
			}
			else if (jitterangle > 1 && jitterangle <= 3)
			{
				pCmd->viewangles.y -= 45;
				jitterangle += 1;
			}
			else
			{
				jitterangle = 0;
			}
		}
		else
		{
			bSendPacket = true;
			iChoked = -1;
		}
	}

	void Static(CUserCmd *pCmd)
	{
		static bool aa1 = false;
		aa1 = !aa1;
		if (aa1)
		{
			static bool turbo = false;
			turbo = !turbo;
			if (turbo)
			{
				pCmd->viewangles.y -= 90;
			}
			else
			{
				pCmd->viewangles.y += 90;
			}
		}
		else
		{
			pCmd->viewangles.y -= 180;
		}
	}
	void FastSpin(CUserCmd *pCmd)
	{
		int random = 160 + rand() % 40;
		static float current_y = pCmd->viewangles.y;
		current_y += random;
		pCmd->viewangles.y = current_y;
	}
	void SlowSpin(CUserCmd *pCmd)
	{
		int random = rand() % 100;
		int random2 = rand() % 900;
		static bool dir;
		static float current_y = pCmd->viewangles.y;
		if (random == 1) dir = !dir;
		if (dir)
			current_y += 5;
		else
			current_y -= 5;
		pCmd->viewangles.y = current_y;
		if (random == random2)
			pCmd->viewangles.y += random;
	}

	void fakelowerbody(CUserCmd *pCmd, bool &bSendPacket)
	{
		static bool f_flip = true;
		f_flip = !f_flip;

		if (f_flip)
		{
			pCmd->viewangles.y -= hackManager.pLocal()->GetLowerBodyYaw() + 90.00f;
			bSendPacket = false;
		}
		else if (!f_flip)
		{
			pCmd->viewangles.y += hackManager.pLocal()->GetLowerBodyYaw() - 90.00f;
			bSendPacket = true;
		}
	}
	void switchsidewaysmodafukers(CUserCmd *pCmd, bool &bSendPacket)
	{
		static bool f_flip = true;
		f_flip = !f_flip;

		if (f_flip)
		{
			pCmd->viewangles.y -= hackManager.pLocal()->GetLowerBodyYaw() - 90.00f;
			bSendPacket = false;
		}
		else if (!f_flip)
		{
			pCmd->viewangles.y += hackManager.pLocal()->GetLowerBodyYaw() - 50.00f;
			bSendPacket = true;
		}
	}
	void aagud(CUserCmd *pCmd, bool &bSendPacket)
	{
		static bool f_flip = true;
		f_flip = !f_flip;
		static int fakeaa = pCmd->viewangles.y = -90 + bSendPacket;


		if (f_flip)
		{
			pCmd->viewangles.y += 90;
			bSendPacket = false;
		}
		else if (!f_flip)
		{
			pCmd->viewangles.y -= 50;
			bSendPacket = true;
		}
		else if (fakeaa)
		{
			pCmd->viewangles.y = fakeaa;
			bSendPacket = false;
		}
		else if (fakeaa = bSendPacket)
		{
			fakeaa;
		}
		else if (fakeaa = false)
		{
			pCmd->viewangles.z = 50.f;
		}

	}
	void jakiesaadobrebardzo(CUserCmd *pCmd, bool &bSendPacket)
	{
		static bool f_flip = true;
		f_flip = !f_flip;
		static int fakeaa = true;

		if (f_flip)
		{
			pCmd->viewangles.y -= hackManager.pLocal()->GetLowerBodyYaw() - 90;
			bSendPacket = false;
		}
		else if (!f_flip)
		{
			pCmd->viewangles.y -= hackManager.pLocal()->GetLowerBodyYaw() + 90;
			bSendPacket = true;
		}
		else if (!f_flip)
		{
			pCmd->viewangles.y = +90;
		}
		if (Menu::Window.RageBotTab.Lbytype.GetIndex() == 6)
		{
			pCmd->viewangles.y = fakeaa = pCmd->viewangles.y = -90;
			bSendPacket = false;
		}
		else if (fakeaa = true)
		{
			pCmd->viewangles.Length2D() > 0.f;
		}
	}
	void fake90p100yaw(CUserCmd *pCmd, bool &bSendPacket)
	{
		static bool f_flip = true;
		f_flip = !f_flip;

		if (f_flip)
		{
			QAngle v37 = pCmd->viewangles;
			pCmd->viewangles.y = v37.y + 90.0;
			bSendPacket = true;
		}
		else if (!f_flip)
		{
			QAngle v37 = pCmd->viewangles;
			pCmd->viewangles.y = v37.y - 90.0;
			bSendPacket = false;
		}
		else if (f_flip)
		{
			pCmd->viewangles.y -= hackManager.pLocal()->GetLowerBodyYaw() + 90;
			bSendPacket = true;
		}
		else if (!f_flip)
		{
			pCmd->viewangles.y -= hackManager.pLocal()->GetLowerBodyYaw() - 90;
			bSendPacket = false;
		}
	}
	void jakiescosabidaza(CUserCmd *pCmd, bool &bSendPacket)
	{
		static bool f_flip = true;
		f_flip = !f_flip;
			int iSpinx = 0;
			iSpinx++;

			if (iSpinx > 1) {
				iSpinx = 180;
			}
		if (f_flip)
		{
			QAngle v37 = pCmd->viewangles;
			pCmd->viewangles.y = v37.y + 69.5;
			bSendPacket = true;
		}
		else if (!f_flip)
		{
			QAngle v37 = pCmd->viewangles;
			pCmd->viewangles.y = v37.y - 90.0;
			bSendPacket = false;
		}
		else if (f_flip)
		{
			pCmd->viewangles.y -= hackManager.pLocal()->GetLowerBodyYaw() + 180;
			bSendPacket = true;
		}
		else if (!f_flip)
		{
			QAngle v90 = pCmd->viewangles;
			pCmd->viewangles.y = v90.y - 180.0;
			bSendPacket = true;
		}
		else if (!f_flip)
		{
			pCmd->viewangles.y -= hackManager.pLocal()->GetLowerBodyYaw() - 10;
			bSendPacket = false;
		}
	}
	void angelfakespin(CUserCmd *pCmd)
	{
		//Angel fake Spin
		int r2 = rand() % 1000;
		pCmd->viewangles.y = pCmd->viewangles.y + 36000180.0;
		float CalculatedCurTime_2 = (r2 * 5000.0);
		pCmd->viewangles.y = CalculatedCurTime_2 + 36000000.0;
	}
	void SideFake2(CUserCmd *pCmd, bool& bSendPacket)
	{
		bool faked123;
		if (faked123 = true)
		{
			bSendPacket = true; //fake aa = obviously some aa with some + shit xddd 
			pCmd->viewangles.y += 90 + rand() % 10; //pAntiAWaa :33 + non ut <3333333333333333333333333333333333333333333333333333333
			faked123 = false;
		}
		else if (Menu::Window.MiscTab.FakeLagEnable.GetState())
		{
			//if fakelag is on then dont care about anything else just put on backwards with some -
			pCmd->viewangles.y = 180 - rand() % 7, 5;
		}
		else
		{ //but when fakelag is off then put on real angle jitter 
			bSendPacket = false;
			bool pJitter;
			if (pJitter = true)
			{
				pCmd->viewangles.y = -70 + 60;
				pJitter = false;
			}
			else
			{
				pCmd->viewangles.y = 69 - rand() % 15;
				pJitter = true;
			}
			faked123 = true;
		}
	}

	void AimAtTarget(CUserCmd *pCmd)
	{
		IClientEntity* pLocal = hackManager.pLocal();

		CBaseCombatWeapon* pWeapon = (CBaseCombatWeapon*)Interfaces::EntList->GetClientEntityFromHandle(hackManager.pLocal()->GetActiveWeaponHandle());

		if (!pLocal || !pWeapon)
			return;

		Vector eye_position = pLocal->GetEyePosition();

		float best_dist = pWeapon->GetCSWpnData()->m_flRange;

		IClientEntity* target = Interfaces::EntList->GetClientEntity(Globals::TargetID);

		if (target == NULL)
			return;

		if (target == pLocal)
			return;

		if ((target->GetTeamNum() == pLocal->GetTeamNum()) || target->IsDormant() || !target->IsAlive() || target->GetHealth() <= 0)
			return;

		Vector target_position = target->GetEyePosition();

		float temp_dist = eye_position.DistTo(target_position);

		if (best_dist > temp_dist)
		{
			best_dist = temp_dist;
			CalcAngle(eye_position, target_position, pCmd->viewangles);
		}
	}

	void EdgeDetect(CUserCmd* pCmd, bool &bSendPacket)
	{
		//Ray_t ray;
		//trace_t tr;

		IClientEntity* pLocal = hackManager.pLocal();

		CTraceFilter traceFilter;
		traceFilter.pSkip = pLocal;

		bool bEdge = false;

		Vector angle;
		Vector eyePos = pLocal->GetOrigin() + pLocal->GetViewOffset();

		for (float i = 0; i < 360; i++)
		{
			Vector vecDummy(10.f, pCmd->viewangles.y, 0.f);
			vecDummy.y += i;

			Vector forward = vecDummy.Forward();

			//vecDummy.NormalizeInPlace();

			float flLength = ((16.f + 3.f) + ((16.f + 3.f) * sin(DEG2RAD(10.f)))) + 7.f;
			forward *= flLength;

			Ray_t ray;
			CGameTrace tr;

			ray.Init(eyePos, (eyePos + forward));
			Interfaces::Trace->EdgeTraceRay(ray, traceFilter, tr, true);

			if (tr.fraction != 1.0f)
			{
				Vector negate = tr.plane.normal;
				negate *= -1;

				Vector vecAng = negate.Angle();

				vecDummy.y = vecAng.y;

				//vecDummy.NormalizeInPlace();
				trace_t leftTrace, rightTrace;

				Vector left = (vecDummy + Vector(0, 45, 0)).Forward(); // or 45
				Vector right = (vecDummy - Vector(0, 45, 0)).Forward();

				left *= (flLength * cosf(rad(30)) * 2); //left *= (len * cosf(rad(30)) * 2);
				right *= (flLength * cosf(rad(30)) * 2); // right *= (len * cosf(rad(30)) * 2);

				ray.Init(eyePos, (eyePos + left));
				Interfaces::Trace->EdgeTraceRay(ray, traceFilter, leftTrace, true);

				ray.Init(eyePos, (eyePos + right));
				Interfaces::Trace->EdgeTraceRay(ray, traceFilter, rightTrace, true);

				if ((leftTrace.fraction == 1.f) && (rightTrace.fraction != 1.f))
				{
					vecDummy.y -= 45; // left
				}
				else if ((leftTrace.fraction != 1.f) && (rightTrace.fraction == 1.f))
				{
					vecDummy.y += 45; // right     
				}

				angle.y = vecDummy.y;
				angle.y += 360;
				bEdge = true;
			}
		}

		if (bEdge)
		{
			static bool turbo = true;
			bool aaEdge = Menu::Window.RageBotTab.AntiAimEdge.GetState();

			if (aaEdge = false)
			{
				//Nothing
			}
			else if (aaEdge = true)
			{
				pCmd->viewangles.y = angle.y;
			}

		}
	}
}

// AntiAim
void CRageBot::DoAntiAim(CUserCmd *pCmd, bool &bSendPacket) // pCmd->viewangles.y = 0xFFFFF INT_MAX or idk
{
	IClientEntity* pLocal = hackManager.pLocal();

	if ((pCmd->buttons & IN_USE) || pLocal->GetMoveType() == MOVETYPE_LADDER)
		return;

	// If the aimbot is doing something don't do anything
	if ((IsAimStepping || pCmd->buttons & IN_ATTACK) && !Menu::Window.RageBotTab.AimbotPerfectSilentAim.GetState())
		return;

	// Weapon shit
	CBaseCombatWeapon* pWeapon = (CBaseCombatWeapon*)Interfaces::EntList->GetClientEntityFromHandle(hackManager.pLocal()->GetActiveWeaponHandle());
	if (pWeapon)
	{
		CSWeaponInfo* pWeaponInfo = pWeapon->GetCSWpnData();
		// Knives or grenades
		if (!GameUtils::IsBallisticWeapon(pWeapon))
		{
			if (Menu::Window.RageBotTab.AntiAimKnife.GetState())
			{
				if (!CanOpenFire() || pCmd->buttons & IN_ATTACK2)
					return;
			}
			else
			{
				return;
			}
		}
	}

	if (Menu::Window.RageBotTab.AntiAimTarget.GetState())
	{
		AntiAims::AimAtTarget(pCmd);
	}

	// Don't do antiaim
	// if (DoExit) return;

	// Anti-Aim Pitch
	switch (Menu::Window.RageBotTab.AntiAimPitch.GetIndex()) // Magic pitch is 69.69?
	{
	case 0:
		// No Pitch AA
		break;
	case 1:
		// Down
		//AntiAims::StaticPitch(pCmd, false);
		pCmd->viewangles.x = 89.0f;
		break;
	case 2:
		// Half Down
		pCmd->viewangles.x = 51.f;
		break;
	case 3:
		// SMAC / Casual safe
		AntiAims::JitterPitch(pCmd);
		break;
	case 4:
		// Jitter
		pCmd->viewangles.x = 179.0f;
		break;
	case 5:
		// Fake Pitch
		pCmd->viewangles.x = -181.f;
		break;
	case 6:
		// Static Down
		pCmd->viewangles.x = 1800089.0f;
		break;
	case 7:
		// Static Jitter
		pCmd->viewangles.x = -1800089.0f;
		break;
	case 8:
		pCmd->viewangles.x = 0.0f;
		break;
	case 9:
	{
		static bool fake = true;
		static int zero = pCmd->viewangles.x = 27193.f;

		if (fake == true)
		{
			pCmd->viewangles.x = 0;
		}
		if (fake == false)
		{
			pCmd->viewangles.x = 218712.f;
		}
		else (pCmd->viewangles.x = zero);
		{
			fake = true;
		}
	}
	case 10:
		AntiAims::fakepitch1(pCmd, bSendPacket);
		break;
	}

	//Anti-Aim Yaw
	switch (Menu::Window.RageBotTab.AntiAimYaw.GetIndex())
	{
	case 0:
		// No Yaw AA
		break;
	case 1:
		// Fake Inverse
		AntiAims::FakeEdge(pCmd, bSendPacket);
		break;
	case 2:
		// Fake Sideways
		AntiAims::FakeSideways(pCmd, bSendPacket);
		break;
	case 3:
		// Fake Static
		AntiAims::FakeStatic(pCmd, bSendPacket);
		break;
	case 4:
		// Fake Inverse
		AntiAims::BackJitter(pCmd);
	//	AntiAims::TFake(pCmd, bSendPacket);
		break;
	case 5:
		// Fake Jitter
        AntiAims::fakelowerbody2(pCmd, bSendPacket);
		break;
	case 6:
		// T Jitter
		AntiAims::TJitter(pCmd);
		break;
	case 7:
		// T Inverse
		AntiAims::Backward(pCmd);
		break;
	case 8:
		// T Inverse
		AntiAims::fakelowerbody(pCmd, bSendPacket);
		break;
	case 9:
		AntiAims::fake180elo(pCmd, bSendPacket);
		break;
	case 10:
		AntiAims::fake2(pCmd, bSendPacket);
		break;
	case 11:
		AntiAims::pLBYbyDEDSEE(pCmd, bSendPacket);
		break;
	case 12:
		AntiAims::skeet(pCmd, bSendPacket);
		break;
	case 13:
		AntiAims::fake5(pCmd, bSendPacket);
		break;
	case 14:
		AntiAims::lbynibybraekr(pCmd, bSendPacket);
		break;
	case 15:
		AntiAims::jakiesaadobrebardzo(pCmd, bSendPacket);
		break;
	case 16:
		AntiAims::aagud(pCmd, bSendPacket);
		break;
	case 17:
		AntiAims::aagud(pCmd, bSendPacket);
		AntiAims::skeet(pCmd, bSendPacket);
		AntiAims::Sideways(pCmd);
		break;
	case 18:
		AntiAims::SlowSpin(pCmd);
		break;
	case 19:
		AntiAims::FastSpin(pCmd);
		break;
	case 20:
		AntiAims::fake90p100yaw(pCmd, bSendPacket);
		break;
	case 21:
		AntiAims::switchsidewaysmodafukers(pCmd, bSendPacket);
		break;
	case 22:
		AntiAims::jakiescosabidaza(pCmd, bSendPacket);
		break;
	case 23:
		AntiAims::angelfakespin(pCmd);
		break;
	case 24:
		AntiAims::SideFake2(pCmd, bSendPacket);
		break;
	}

	// Edge Anti Aim
	//AntiAims::EdgeDetect(pCmd, bSendPacket); this is broken it seems ill just remove it cuz what if it causes some crashes

	// Angle offset
	pCmd->viewangles.y += Menu::Window.RageBotTab.AntiAimOffset.GetValue();

	if (Menu::Window.RageBotTab.AntiResolver.GetIndex() == 0)
	{
		//off
	}

	if (Menu::Window.RageBotTab.AntiResolver.GetIndex() == 1)
	{
		static bool antiResolverFlip = false;
		if (pCmd->viewangles.y == pLocal->GetLowerBodyYaw())
		{
			if (antiResolverFlip)
				pCmd->viewangles.y += 60.f;
			else
				pCmd->viewangles.y -= 60.f;
			antiResolverFlip = !antiResolverFlip;
		}
	}
	if (Menu::Window.RageBotTab.AntiResolver.GetIndex() == 2)
	{
		static bool antiResolverFlip = false;
		if (pCmd->viewangles.y == pLocal->GetLowerBodyYaw())
		{
			if (antiResolverFlip)
				pCmd->viewangles.y += 50.f;
			else
				pCmd->viewangles.y -= 90.f;
			antiResolverFlip = !antiResolverFlip;
		}
	}
	if (Menu::Window.RageBotTab.AntiResolver.GetIndex() == 3)
	{
		static bool antiResolverFlip = false;
		if (pCmd->viewangles.y == pLocal->GetLowerBodyYaw())
		{
			if (antiResolverFlip)
				pCmd->viewangles.y += 20.f;
			else
				pCmd->viewangles.y -= 35.f;
			antiResolverFlip = !antiResolverFlip;
		if (!antiResolverFlip)
			pCmd->viewangles.y -= 45.f;
		else 
			pCmd->viewangles.y -= 55.f;
			antiResolverFlip = !antiResolverFlip;
		}
	}
	
	switch (Menu::Window.RageBotTab.Lbytype.GetIndex())
	{
		//none
	case 0:
		break;
	case 1:
		AntiAims::Backward(pCmd);
		break;
	case 2:
	{
		AntiAims::Sideways(pCmd);
		break;
	}
	case 3:
	{
		pCmd->viewangles.y = +90;
		break;
	}
	case 4:
		AntiAims::pLBYbyDEDSEE(pCmd, bSendPacket);
		break;
	case 5:
		AntiAims::TJitter(pCmd);
		break;
	case 6:
		AntiAims::fake5(pCmd, bSendPacket);
		break;
	case 7:
		AntiAims::fake180elo(pCmd, bSendPacket);
		break;
	case 8:
		AntiAims::lbynibybraekr(pCmd, bSendPacket);
		break;
	case 9:
		AntiAims::FakeJitter(pCmd, bSendPacket);
		break;
	case 10:
		AntiAims::SlowSpin(pCmd);
		break;
	case 11:
		AntiAims::FastSpin(pCmd);
	case 12:
		AntiAims::switchsidewaysmodafukers(pCmd, bSendPacket);
		break;
	case 13:
		AntiAims::angelfakespin(pCmd);
		break;
	case 14:
		AntiAims::SideFake2(pCmd, bSendPacket);
		AntiAims::switchsidewaysmodafukers(pCmd, bSendPacket);
		AntiAims::fake180elo(pCmd, bSendPacket);
		break;
	}
}

