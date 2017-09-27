#include "Resolver.h"
#include "RenderManager.h"

#define LC_NONE             0
#define LC_ALIVE            (1<<0)

#define LC_ORIGIN_CHANGED   (1<<8)
#define LC_ANGLES_CHANGED   (1<<9)
#define LC_SIZE_CHANGED     (1<<10)
#define LC_ANIMATION_CHANGED (1<<11)

#define LAG_COMPENSATION_TELEPORTED_DISTANCE_SQR ( 64.0f * 64.0f )
#define LAG_COMPENSATION_EPS_SQR ( 1.5f * 1.5f )
#define LAG_COMPENSATION_ERROR_EPS_SQR ( 5.0f * 5.0f )

extern int bullets;
float tickcount;
float YawDelta[64];
float reset[64];
float Delta[64];
float OldLowerBodyYaw[64];
float Resolved_angles[64];
int iSmart;
static int jitter = -1;

int Globals::Shots = 0;
bool Globals::change;
CUserCmd* Globals::UserCmd;
int Globals::TargetID;
IClientEntity* Globals::Target;




void R::Resolve()
{
	IClientEntity *pLocal = Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());

	for (int i = 0; i < Interfaces::Engine->GetMaxClients(); ++i)
	{
		IClientEntity *pEntity = Interfaces::EntList->GetClientEntity(i);

		if (!pEntity || pEntity->IsDormant() || !pEntity->IsAlive())
			continue;

		if (pEntity->GetTeamNum() == pLocal->GetTeamNum() || !pLocal->IsAlive())
			continue;

		Vector* eyeAngles = pEntity->GetEyeAnglesPointer();
		

		if (Menu::Window.RageBotTab.AccuracyResolverYaw.GetIndex() == 9)
		{
			if (pEntity != Globals::Target) continue;

			Vector AimAngle;
			CalcAngleYawOnly(pEntity->GetEyePosition(), pLocal->GetAbsOrigin2(), AimAngle);
			NormalizeVector(AimAngle);
			static float StoredYaw = 0;
			static bool bLowerBodyIsUpdated = false;
			if (pEntity->GetLowerBodyYaw() != StoredYaw) bLowerBodyIsUpdated = true;
			else bLowerBodyIsUpdated = false;
			if (bLowerBodyIsUpdated) StoredYaw = pEntity->GetLowerBodyYaw();
			if (pEntity->GetFlags() & FL_ONGROUND && pEntity->GetVelocity().Length2D() != 0) eyeAngles->y = pEntity->GetLowerBodyYaw();
			else
			{
				if (Menu::Window.RageBotTab.AccuracyResolverYaw.GetIndex() == 1)
				{
					int value = rand() % 10;
					switch (value) {
					case 0:eyeAngles->y = AimAngle.y; break;
					case 1:eyeAngles->y += 0; break;
					case 2:eyeAngles->y = AimAngle.y + 180; break;
					case 3:eyeAngles->y += 15; break;
					case 4:eyeAngles->y = AimAngle.y + 90; break;
					case 5:eyeAngles->y -= 15; break;
					case 6:eyeAngles->y = AimAngle.y + 270; break;
					case 7:eyeAngles->y += 0; break;
					case 8:eyeAngles->y = AimAngle.y + 180; break;
					case 9:eyeAngles->y -= 45; break;
					case 10:eyeAngles->y += 45; break;
						break;
					}
				}
			}
			break;
			if (Menu::Window.RageBotTab.AccuracyResolverYaw.GetIndex() == 2)
			{
				int num = Globals::Shots % 5;
				switch (num) {
				case 0:eyeAngles->y = AimAngle.y - 0; break;
				case 1:eyeAngles->y = AimAngle.y - 15; break;
				case 2:eyeAngles->y = AimAngle.y + 40; break;
				case 3:eyeAngles->y = AimAngle.y - 40; break;
				case 4:eyeAngles->y = AimAngle.y + 15; break;
				}
			}
			break;
			if (Menu::Window.RageBotTab.AccuracyResolverYaw.GetIndex() == 3)
			{
				int num = Globals::Shots % 4;
				switch (num) {
				case 0:eyeAngles->y = rand() % 360; break;
				case 1:eyeAngles->y = 0; break;
				case 2:eyeAngles->y = rand() % 180; break;
				case 3:eyeAngles->y = 180; break;// te reso zle jest?
					break;
				}
			}
			break;
			if (Menu::Window.RageBotTab.AccuracyResolverYaw.GetIndex() == 4)
			{
				for (int i = 0; i < 64; ++i)
				{
					IClientEntity* ent = Interfaces::EntList->GetClientEntity(i);

					if (!ent || !ent->IsAlive())
						continue;

					*(float*)((uintptr_t)ent + 0xAA08 + 4) += Delta[i];
					reset[i] = *(float*)((uintptr_t)ent + 0xAA08 + 4);

					float LowerBodyYaw = *(float*)((uintptr_t)ent + 0x39F8);

					if (OldLowerBodyYaw[i] != LowerBodyYaw)
					{
						YawDelta[i] = reset[i] - LowerBodyYaw;
						OldLowerBodyYaw[i] = LowerBodyYaw;
						*(float*)((uintptr_t)ent + 0xAA08 + 4) = LowerBodyYaw;
					}
					else
					{
						*(float*)((uintptr_t)ent + 0xAA08 + 4) = reset[i] - YawDelta[i];

						while (*(float*)((uintptr_t)ent + 0xAA08 + 4) < -180.0f)
							*(float*)((uintptr_t)ent + 0xAA08 + 4) += 360.0f;

						while (*(float*)((uintptr_t)ent + 0xAA08 + 4) > 180.0f)
							*(float*)((uintptr_t)ent + 0xAA08 + 4) -= 360.0f;
					}
					Resolved_angles[i] = *(float*)((uintptr_t)ent + 0xAA08 + 4);
				}
			}
			break;
			if (Menu::Window.RageBotTab.AccuracyResolverYaw.GetIndex() == 5)
			{
				IClientEntity* ent = Interfaces::EntList->GetClientEntity(i);

				for (int i = 0; i < 32; i++) {
					if (!pEntity || !pEntity->IsAlive() || !(pEntity->GetEyeAngles().y >= 85.f && pEntity->GetEyeAngles().y <= 89.f))
						continue;

					float CurYaw = pEntity->GetLowerBodyYaw();
					if (OldLowerBodyYaw[i] != CurYaw) {
						Delta[i] = pEntity->GetEyeAngles().y - CurYaw;
						OldLowerBodyYaw[i] = CurYaw;
						*(float*)((uintptr_t)ent + 0xAA08 + 4) = CurYaw;
						continue;
					}
					else {
						*(float*)((uintptr_t)ent + 0xAA08 + 4) = pEntity->GetEyeAngles().y - Delta[i];
					}

					float yClamped = pEntity->GetEyeAngles().y;
					while (yClamped < -180.0f)
						yClamped += 360.0f;

					while (yClamped > 180.0f)
						yClamped -= 360.0f;
					*(float*)((uintptr_t)ent + 0xAA08 + 4) = yClamped;
				}
				Resolved_angles[i] = *(float*)((uintptr_t)ent + 0xAA08 + 4);
			}
			break;
			if (Menu::Window.RageBotTab.AccuracyResolverYaw.GetIndex() == 6)
			{
				int value = rand() % 3 + 0;
				switch (value)
				{
				case 0:
				{
					eyeAngles->y = (rand() % 180);
					break;
				}
				case 1:
				{
					eyeAngles->y = (rand() % 360);
					break;
				}
				case 2:
				{
					eyeAngles->y = 0;
					break;
				}
				}
				break;
			}
			if(Menu::Window.RageBotTab.AccuracyResolverYaw.GetIndex() == 7)
			{
				int value = rand() % 10;
				switch (value) {
				case 0:eyeAngles->y = eyeAngles->y; break;
				case 1:eyeAngles->y += 0; break;
				case 2:eyeAngles->y = eyeAngles->y + 180; break;
				case 3:eyeAngles->y += 15; break;
				case 4:eyeAngles->y = eyeAngles->y + 90; break;
				case 5:eyeAngles->y -= 15; break;
				case 6:eyeAngles->y = eyeAngles->y + 270; break;
				case 7:eyeAngles->y += 0; break;
				case 8:eyeAngles->y = eyeAngles->y + 180; break;
				case 9:eyeAngles->y -= 45; break;
				case 10:eyeAngles->y += 45; break;
					break;
				}
				break;
			}
			if (Menu::Window.RageBotTab.AccuracyResolverPitch.GetIndex() == 1)
			{
				int shoots = Globals::Shots;
				int value = shoots % 9;
				int num = rand() % 5;
				switch (value && num) {
				case 0:eyeAngles->x = -50; break;
				case 1:eyeAngles->x = +50; rand(); break;
				case 2:eyeAngles->x = -89; break;
				case 3:eyeAngles->x = +89; rand(); break;
				case 4:eyeAngles->x = -90; break;
				case 5:eyeAngles->x = +90; rand(); break;
					break;
				}
			}
			if (Menu::Window.RageBotTab.AccuracyResolverPitch.GetIndex() == 2)
			{
				int shoots = Globals::Shots;
				int value = shoots % 9;
				int num = rand() % 5;
				switch (value && num) {
					case 0:eyeAngles->x = 89.f;
					case 1:eyeAngles->x = 89.f;
					case 2:eyeAngles->x = -89.f;
				}
			}
		}
	}
}
/*
static void pNormalizeAngle(float& flAngle)
{
	if (std::isnan(flAngle)) flAngle = 0.0f;
	if (std::isinf(flAngle)) flAngle = 0.0f;

	float flRevolutions = flAngle / 360;

	if (flAngle > 180 || flAngle < -180)
	{
		if (flRevolutions < 0)
			flRevolutions = -flRevolutions;

		flRevolutions = round(flRevolutions);

		if (flAngle < 0)
			flAngle = (flAngle + 360 * flRevolutions);
		else
			flAngle = (flAngle - 360 * flRevolutions);
	}
}

static void pNormalizeAngles(QAngle &angles)
{
	pNormalizeAngle(angles.x);
	pNormalizeAngle(angles.y);
	angles.z = 0;
}

static void pClampAngles(QAngle &angles)
{
	if (std::isnan(angles.x)) angles.x = 0.0f;
	if (std::isnan(angles.y)) angles.y = 0.0f;
	if (std::isnan(angles.z)) angles.z = 0.0f;
	if (std::isinf(angles.x)) angles.x = 0.0f;
	if (std::isinf(angles.y)) angles.y = 0.0f;
	if (std::isinf(angles.z)) angles.z = 0.0f;

	if (angles.x > 89.0f && angles.x <= 180.0f)
		angles.x = 89.0f;
	while (angles.x > 180.0f)
		angles.x -= 360.0f;
	while (angles.x < -89.0f)
		angles.x = -89.0f;
	while (angles.y > 180.0f)
		angles.y -= 360.0f;
	while (angles.y < -180.0f)
		angles.y += 360.0f;

	angles.z = 0;
}
static void pClamp(QAngle &angles)
{
	pNormalizeAngles(angles);
	pClampAngles(angles);
}

int GetLowsestDenominator(int a, int b)
{
	for (int i = 2; i <= min(a, b); i++)
		if ((a % i == 0) && (b % i == 0)) { return i; }
	return 1;
}

int GetServerSideID(int EntityIndex)
{
	player_info_t pInfo;
	Interfaces::Engine->GetPlayerInfo(EntityIndex, &pInfo);
	return pInfo.userID;
}
void R::Other(CUserCmd *pCmd, bool &bSendPacket, unsigned int vguiPanel,int viewangles, unsigned __int8 swap)
{

	static unsigned int FocusOverlayPanel = 0;
	static bool FoundPanel = false;

	IClientEntity* pLocal = hackManager.pLocal();

	if (Menu::Window.ResolverTab.AntiResolver.GetIndex() == 4)
	{
		{
		static bool antiResolverFlip = false;
		if (pCmd->viewangles.y == pLocal->GetVelocity().Length() > 0.f)
		{
			if (antiResolverFlip && pLocal->GetVelocity().Length() > 0.f)
				pCmd->viewangles.y += 90.f;
			else
				pCmd->viewangles.y -= 90.f;
			antiResolverFlip = !antiResolverFlip;
		}
	}
		if (Menu::Window.ResolverTab.AccuracyResolverr.GetState())
		{
			{
				bool result; // al@4
				unsigned __int8 byte_100AA52C;
				unsigned __int8 byte_100AFD00;
				unsigned __int8 byte_100AFD08;
				unsigned __int8 byte_100AFD01;
				unsigned __int8 dword_100AFABC;
				unsigned __int8 byte_100AFD09;
				unsigned __int8 dword_100AFAB8;
				unsigned __int8 sub_10004CA0;


				if (sub_10004CA0)
				{
					if (byte_100AA52C)
					{
						if (byte_100AFD00)
							*(float *)(viewangles + 16) = *(float *)(viewangles + 16) - 226.0;
						else
							*(float *)(viewangles + 16) = *(float *)(viewangles + 16) - 134.0;
						result = byte_100AFD00 == 0;
						byte_100AFD00 = byte_100AFD00 == 0;
					}
					else
					{
						if (byte_100AFD01)
						{
							result = viewangles;
							*(float *)(viewangles + 16) = *(float *)(viewangles + 16) - 134.0;
						}
						else
						{
							result = viewangles;
							*(float *)(viewangles + 16) = *(float *)(viewangles + 16) - 226.0;
						}
						byte_100AFD01 = byte_100AFD01 == 0;
					}
				}
				else if (++dword_100AFABC == 1)
				{
					byte_100AA52C = 0;
					if (byte_100AFD09)
					{
						result = viewangles;
						*(float *)(viewangles + 16) = *(float *)(viewangles + 16) - 134.0;
					}
					else
					{
						result = viewangles;
						*(float *)(viewangles + 16) = *(float *)(viewangles + 16) - 226.0;
					}
					byte_100AFD09 = byte_100AFD09 == 0;
				}
				else
				{
					byte_100AA52C = 1;
					if (byte_100AFD08)
						*(float *)(viewangles + 16) = *(float *)(viewangles + 16) - 226.0;
					else
						*(float *)(viewangles + 16) = *(float *)(viewangles + 16) - 134.0;
					result = byte_100AFD08 == 0;
					byte_100AFD08 = byte_100AFD08 == 0;
					dword_100AFABC = 0;
				}
			}
			if (Menu::Window.ResolverTab.AccuracyResolverr.GetState())
			{
				int result; // eax@1

				result = swap;
				if (swap)
					*(DWORD *)(viewangles + 12) = -89;
				else
					*(DWORD *)(viewangles + 12) = 89;
				return;
			}
			if (Menu::Window.ResolverTab.AccuracyResolverr.GetState())
			{
				int result; // eax@1
				unsigned __int8 byte_100AA52C;
				unsigned __int8 dword_100AFAA8;
				unsigned __int8 sub_10004CA0;

				if ((BYTE)result)
				{
					result = (unsigned __int8)byte_100AA52C;
					if (byte_100AA52C)
					{
						result = viewangles;
						*(float *)(viewangles + 16) = *(float *)(viewangles + 16) + 90.0;
					}
					else
					{
						*(float *)(viewangles + 16) = *(float *)(viewangles + 16) - 90.0;
					}
				}
				else if (++dword_100AFAA8 == 1)
				{
					byte_100AA52C = 0;
					result = viewangles;
					*(float *)(viewangles + 16) = *(float *)(viewangles + 16) - 90.0;
				}
				else
				{
					byte_100AA52C = 1;
					*(float *)(viewangles + 16) = *(float *)(viewangles + 16) + 90.0;
					dword_100AFAA8 = 0;
				}
				return;
			}
			if (Menu::Window.ResolverTab.AccuracyResolverr.GetState())
			{
				{
					int v1; // eax@1
					int result; // eax@1
					signed __int64 v3; // rtt@1
					float v4; // xmm0_4@4
					float v5; // xmm0_4@6
					signed int v6; // [sp+8h] [bp-4h]@1

					v1 = rand();
					v3 = v1;
					result = v1 / 100;
					v6 = v3 % 100;
					if (v6 < 98)
					{
						result = viewangles;
						*(float *)(viewangles + 16) = *(float *)(viewangles + 16) - 180.0;
					}
					if (v6 < 15)
					{
						v4 = *(float *)(viewangles + 16) + (float)(rand() % 141 - 70);
						result = viewangles;
						*(float *)(viewangles + 16) = v4;
					}
					if (v6 == 69)
					{
						v5 = *(float *)(viewangles + 16) + (float)(rand() % 181 - 90);
						result = viewangles;
						*(float *)(viewangles + 16) = v5;
					}
					return;
				}
			}
			if (Menu::Window.ResolverTab.AccuracyResolverr.GetState())
			{
				int result; // eax@1
				unsigned __int8 byte_100AA52C;
				unsigned __int8 dword_100AFAB8;
				unsigned __int8 sub_10004CA0;


				if ((BYTE)result)
				{
					result = (unsigned __int8)byte_100AA52C;
					if (byte_100AA52C)
					{
						result = viewangles;
						*(float *)(viewangles + 16) = *(float *)(viewangles + 16) + 0.0;
					}
					else
					{
						*(float *)(viewangles + 16) = *(float *)(viewangles + 16) + 180.0;
					}
				}
				else if (++dword_100AFAB8 == 1)
				{
					byte_100AA52C = 0;
					result = viewangles;
					*(float *)(viewangles + 16) = *(float *)(viewangles + 16) + 180.0;
				}
				else
				{
					byte_100AA52C = 1;
					*(float *)(viewangles + 16) = *(float *)(viewangles + 16) + 0.0;
					dword_100AFAB8 = 0;
				}
				return;
			}
		}
	}
}
void pNormalize(float& angle) {
	while (angle > 180.f) {
		angle -= 360.f;
	}
	while (angle < -180.f) {
		angle += 360.f;
	}
}
void FixYaw(const CRecvProxyData *pData, void *pStruct, void *pOut)
{
	IClientEntity *pLocal = Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());
	static Vector vLast[65];
	static bool bShotLastTime[65];
	static bool bJitterFix[65];

	float *flPitch = (float*)((DWORD)pOut - 4);
	float flYaw = pData->m_Value.m_Float;
	bool bHasAA;
	bool bSpinbot;
	static bool last[128];
	bool tmp = last[((IClientEntity*)(pStruct))->GetIndex()];
	float yaw = pData->m_Value.m_Float;

	if (Menu::Window.ResolverTab.AccuracyResolverr.GetState())
	{
		for (int i = 0; i < Interfaces::EntList->GetHighestEntityIndex(); ++i)
		{
			IClientEntity *pEntity = Interfaces::EntList->GetClientEntity(i);

			flYaw = pEntity->GetLowerBodyYaw(); //Sets entity's eye angles to their current LBY
		}
	}
	else if (Menu::Window.ResolverTab.AccuracyResolverr.GetState())
	{
		for (int i = 0; i < Interfaces::EntList->GetHighestEntityIndex(); ++i)
		{
			IClientEntity *pEntity = Interfaces::EntList->GetClientEntity(i);

			static float lowerDelta[64];
			static float lastYaw[64];

			float curLower = pEntity->GetLowerBodyYaw();
			float curYaw = flYaw;

			lowerDelta[i] = curYaw - curLower;
			pNormalize(lowerDelta[i]);

			if (fabs(lowerDelta[i]) > 15.f) {
				if (pEntity->GetVelocity().Length() > 0.1f) //If the entity is moving, Set EyeAngles to LBY. LBY Updates when moving.
				{
					flYaw = pEntity->GetLowerBodyYaw();
				}
				else
				{
					if (curYaw != lastYaw[i])
					{
						flYaw += lowerDelta[i];
						lastYaw[i] = curYaw;
					}
				}
			}
		}
	}
	else if (Menu::Window.ResolverTab.AccuracyResolverr.GetState())
	{
		for (int i = 0; i < Interfaces::EntList->GetHighestEntityIndex(); ++i)
		{
			IClientEntity *pEntity = Interfaces::EntList->GetClientEntity(i);

			static float OldLowerBodyYaws[64];
			static float OldYawDeltas[64];

			float CurYaw = pEntity->GetLowerBodyYaw();

			if (OldLowerBodyYaws[i] != CurYaw)
			{
				OldYawDeltas[i] = flYaw - CurYaw;
				OldLowerBodyYaws[i] = CurYaw;
				flYaw = CurYaw;
				continue;
			}
			else
			{
				flYaw = flYaw - OldYawDeltas[i];
			}
		}
	}
	else if (Menu::Window.ResolverTab.AccuracyResolverr.GetState())
	{
		static bool flip;

		for (int i = 0; i < Interfaces::EntList->GetHighestEntityIndex(); ++i)
		{
			IClientEntity *pEntity = Interfaces::EntList->GetClientEntity(i);

			if (!pEntity->GetVelocity().Length() > 0.1f)
			{
				float flCurTime = Interfaces::Globals->curtime;
				static float flTimeUpdate = 0.5f;
				static float flNextTimeUpdate = flCurTime + flTimeUpdate;

				if (flCurTime >= flNextTimeUpdate)
				{
					flip != flip;
				}

				if (flNextTimeUpdate < flCurTime || flNextTimeUpdate - flCurTime > 10.f)
					flNextTimeUpdate = flCurTime + flTimeUpdate;

				if (flip)
				{
					flYaw += 35.f;
				}
				else {
					flYaw -= 35.f;
				}
			}

			flYaw;
		}

		*(float*)(pOut) = flYaw;
	}
	else if (Menu::Window.ResolverTab.AccuracyResolverr.GetState())
	{
		// Anglefix
		bHasAA = ((*flPitch == 90.0f) || (*flPitch == 270.0f));
		bSpinbot = false;

		if (!bShotLastTime[((IClientEntity*)(pStruct))->GetIndex()]
			&& (fabsf(flYaw - vLast[((IClientEntity*)(pStruct))->GetIndex()].y) > 15.0f) && !bHasAA)
		{
			flYaw = vLast[((IClientEntity*)(pStruct))->GetIndex()].y;
			bShotLastTime[((IClientEntity*)(pStruct))->GetIndex()] = true;
		}
		else
		{
			if (bShotLastTime[((IClientEntity*)(pStruct))->GetIndex()]
				&& (fabsf(flYaw - vLast[((IClientEntity*)(pStruct))->GetIndex()].y) > 15.0f))
			{
				bShotLastTime[((IClientEntity*)(pStruct))->GetIndex()] = true;
				bSpinbot = true;
			}
			else
			{
				bShotLastTime[((IClientEntity*)(pStruct))->GetIndex()] = false;
			}
		}

		vLast[((IClientEntity*)(pStruct))->GetIndex()].y = flYaw;


		bool bTmp = bJitterFix[((IClientEntity*)(pStruct))->GetIndex()];

		bJitterFix[((IClientEntity*)(pStruct))->GetIndex()] = (flYaw >= 180.0f && flYaw <= 360.0f);

		if (bTmp && (flYaw >= 0.0f && flYaw <= 180.0f))
		{
			flYaw += 359.0f;
		}
		
	}

	*(float*)(pOut) = flYaw;
}


void FixPitch(const CRecvProxyData *pData, void *pStruct, void *pOut)
{
	float p = pData->m_Value.m_Float;

	 if (!Menu::Window.ResolverTab.AccuracyResolverr.GetState())
	if(pNormalize)
	{
		if (p > 89.0f && p < 91.0f)
			p -= 90.0f;
		else if (p > -89.0f && p < -91.0f)
			p += 90.0f;

		if (p > 179.0f && p < 181.0f)
			p -= 180.0f;

		else if (p > -179.0f && p < -181.0f)
			p += 180.0f;

	}
	*(float*)pOut = p;
}
void Resolverapp()
{
	ClientClass *pClass = Interfaces::Client->GetAllClasses();
	while (pClass)
	{
		const char *pszName = pClass->m_pRecvTable->m_pNetTableName;
		if (!strcmp(pszName, "DT_CSPlayer"))
		{
			for (int i = 0; i < pClass->m_pRecvTable->m_nProps; i++)
			{
				RecvProp *pProp = &(pClass->m_pRecvTable->m_pProps[i]);
				const char *name = pProp->m_pVarName;

				// Pitch Fix
				if (!strcmp(name, "m_angEyeAngles[0]"))
				{
					pProp->m_ProxyFn = FixPitch;
				}

				// Yaw Fix
				if (!strcmp(name, "m_angEyeAngles[1]"))
				{

					pProp->m_ProxyFn = FixYaw;
				}
			}
		}
	}
}
*/
