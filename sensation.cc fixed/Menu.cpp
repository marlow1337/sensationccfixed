/*
Rest In Peace ApocalypseCheats
*/

#include "Menu.h"
#include "Controls.h"
#include "Hooks.h"
#include "Interfaces.h"
#include "CRC32.h"
#include <string>
#include "ItemDefinitions.hpp"
#include "Hooks.h"
#include "Hacks.h"
#include "Chams.h"
#include "ESP.h"
#include "Interfaces.h"
#include "RenderManager.h"
#include "MiscHacks.h"
#include "CRC32.h"
#include "Resolver.h"
#include "MiscHacks.h"

#define WINDOW_WIDTH 950
#define WINDOW_HEIGHT 685

ApocalypseWindow Menu::Window;

void Unloadbk()
{
	DoUnload = true;
}

void NApplyCallbk()
{

}
void SaveCallbk()
{
	switch (Menu::Window.MiscTab.SetNr.GetIndex())
	{
	case 0:
		GUI.SaveWindowState(&Menu::Window, "legit.xml");
		break;
	case 1:
		GUI.SaveWindowState(&Menu::Window, "semirage.xml");
		break;
	case 2:
		GUI.SaveWindowState(&Menu::Window, "rage.xml");
		break;
	case 3:
		GUI.SaveWindowState(&Menu::Window, "custom.xml");
		break;
	case 4:
		GUI.SaveWindowState(&Menu::Window, "custom1.xml");
		break;
	}

}

void LoadCallbk()
{
	switch (Menu::Window.MiscTab.SetNr.GetIndex())
	{
	case 0:
		GUI.LoadWindowState("legit.xml", &Menu::Window);
		break;
	case 1:
		GUI.LoadWindowState("semirage.xml", &Menu::Window);
		break;
	case 2:
		GUI.LoadWindowState("rage.xml", &Menu::Window);
		break;
	case 3:
		GUI.LoadWindowState("custom.xml", &Menu::Window);
		break;
	case 4:
		GUI.LoadWindowState("custom2.xml", &Menu::Window);
		break;
	}
}

void UnLoadCallbk()
{
	DoUnload = true;
}

void KnifeApplyCallbk()
{
	static ConVar* Meme = Interfaces::CVar->FindVar("cl_fullupdate");
	Meme->nFlags &= ~FCVAR_CHEAT;
	Interfaces::Engine->ClientCmd_Unrestricted("cl_fullupdate");

}

/*void SetAKCB()
{
IClientEntity *pLocal = Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());
IClientEntity* WeaponEnt = Interfaces::EntList->GetClientEntityFromHandle(pLocal->GetActiveWeaponHandle());
CBaseCombatWeapon* Weapon = (CBaseCombatWeapon*)WeaponEnt;
*Weapon->FallbackPaintKit() = 524;

}*/
void ApocalypseWindow::Setup()
{
	SetPosition(50, 50);
	SetSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	/*SetTitle("Apocalyplse | Private");*/

	RegisterTab(&RageBotTab);
	//RegisterTab(&ResolverTab);
	RegisterTab(&LegitBotTab);
	RegisterTab(&VisualsTab);
	RegisterTab(&MiscTab);
	RegisterTab(&ColorTab);
	RegisterTab(&SkinchangerTab);

	//RageBotTab.RegisterControl(&LoadButton);
	//LegitBotTab.RegisterControl(&LoadButton);
	//VisualsTab.RegisterControl(&LoadButton);
	//MiscTab.RegisterControl(&LoadButton);

	//RECT Client = GetClientArea();
	//Client.bottom -= 29;

	RageBotTab.Setup();
	//ResolverTab.Setup();
	LegitBotTab.Setup();
	VisualsTab.Setup();
	MiscTab.Setup();
	ColorTab.Setup();
	SkinchangerTab.Setup();

	//setts

	//SaveButton.SetText("Save Configuration");
	//SaveButton.SetCallback(SaveCallbk);
	//SaveButton.PlaceLabledControl("Save", this, &SaveButton);

	//LoadButton.SetText("Load Configuration");
	//LoadButton.SetCallback(LoadCallbk);
	//ButtonGroup.PlaceLabledControl("Load", this, &LoadButton);

}

void CRageBotTab::Setup()
{

	SetTitle("RAGEBOT");

	ActiveLabel.SetPosition(190, -33);
	ActiveLabel.SetText("Active");
	RegisterControl(&ActiveLabel);

	Active.SetFileId("active");
	Active.SetPosition(170, -32);
	RegisterControl(&Active);

#pragma region Aimbot

	AimbotGroup.SetPosition(176, -16);
	AimbotGroup.SetText("Aimbot");
	AimbotGroup.SetSize(376, 284);
	RegisterControl(&AimbotGroup);

	AimbotEnable.SetFileId("aim_enable");
	AimbotGroup.PlaceCheckBox("Enable", this, &AimbotEnable);

	AimbotAutoFire.SetFileId("aim_autofire");
	AimbotGroup.PlaceCheckBox("Auto Fire", this, &AimbotAutoFire);

	AimbotFov.SetFileId("aim_fov");
	AimbotFov.SetBoundaries(0.f, 180.f);
	AimbotFov.SetValue(180.f);
	AimbotGroup.PlaceOtherControl("FOV Range", this, &AimbotFov);

	AimbotSilentAim.SetFileId("aim_silent");
	AimbotGroup.PlaceCheckBox("Silent Aim", this, &AimbotSilentAim);

	AimbotPerfectSilentAim.SetFileId("aim_psilent");
	AimbotGroup.PlaceCheckBox("Perfect Silent", this, &AimbotPerfectSilentAim);

	AimbotAutoPistol.SetFileId("aim_autopistol");
	AimbotGroup.PlaceCheckBox("Auto Pistol", this, &AimbotAutoPistol);

	AimbotAimStep.SetFileId("aim_aimstep");
	AimbotGroup.PlaceCheckBox("Aim Step", this, &AimbotAimStep);

	AimbotKeyPress.SetFileId("aim_usekey");
	AimbotGroup.PlaceCheckBox("On Key Press", this, &AimbotKeyPress);

	AimbotKeyBind.SetFileId("aim_key");
	AimbotGroup.PlaceLabledControl("", this, &AimbotKeyBind);

	AimbotEnginePred.SetFileId("rage_prediction");
	AimbotGroup.PlaceCheckBox("Engine Prediction", this, &AimbotEnginePred);

	AimbotAutoRevolver.SetFileId("aim_autorevolver");
	AimbotGroup.PlaceCheckBox("Auto Revolver", this, &AimbotAutoRevolver);


#pragma endregion Aimbot Controls Get Setup in here

#pragma region Target
	TargetGroup.SetPosition(176, 278);
	TargetGroup.SetText("Target");
	TargetGroup.SetSize(376, 185);
	RegisterControl(&TargetGroup);

	TargetSelection.SetFileId("tgt_selection");
	TargetSelection.AddItem("FOV");
	TargetSelection.AddItem("Distance");
	TargetSelection.AddItem("Health");
	TargetGroup.PlaceLabledControl("Selection", this, &TargetSelection);

	TargetFriendlyFire.SetFileId("tgt_friendlyfire");
	TargetGroup.PlaceCheckBox("Shoot Trought Team", this, &TargetFriendlyFire);

	TargetHitbox.SetFileId("tgt_hitbox");
	TargetHitbox.AddItem("Head");
	TargetHitbox.AddItem("Neck");
	TargetHitbox.AddItem("Chest");
	TargetHitbox.AddItem("Stomach");
	TargetHitbox.AddItem("Shins");
	TargetGroup.PlaceLabledControl("Hitbox", this, &TargetHitbox);

	
	TargetHitscan.SetFileId("tgt_hitscan");
	TargetHitscan.AddItem("Off");
	TargetHitscan.AddItem("Low");
	TargetHitscan.AddItem("Medium");
	TargetHitscan.AddItem("Hard"); 
	TargetHitscan.AddItem("Body Only");
	TargetGroup.PlaceLabledControl("HitScan", this, &TargetHitscan);

	TargetMultipoint.SetFileId("tgt_multipoint");
	TargetGroup.PlaceCheckBox("Multipoint", this, &TargetMultipoint);

	TargetPointscale.SetFileId("tgt_pointscale");
	TargetPointscale.SetBoundaries(0.f, 10.f);
	TargetPointscale.SetValue(0.f);
	TargetGroup.PlaceLabledControl("Aim Height", this, &TargetPointscale);
#pragma endregion Targetting controls 

#pragma region Accuracy
	AccuracyGroup.SetPosition(560, 250);
	AccuracyGroup.SetText("Accuracy");
	AccuracyGroup.SetSize(360, 260); 
	RegisterControl(&AccuracyGroup);

	AccuracyRecoil.SetFileId("acc_norecoil");
	AccuracyGroup.PlaceCheckBox("NoRecoil", this, &AccuracyRecoil);

	AccuracyAutoWall.SetFileId("acc_awall");
	AccuracyGroup.PlaceCheckBox("AutoWall", this, &AccuracyAutoWall);

	AccuracyMinimumDamage.SetFileId("acc_mindmg");
	AccuracyMinimumDamage.SetBoundaries(1.f, 100.f);
	AccuracyMinimumDamage.SetValue(1.f);
	AccuracyGroup.PlaceLabledControl("AutoWall Minimum Damage", this, &AccuracyMinimumDamage);

	AccuracyAutoStop.SetFileId("acc_stop");
	AccuracyGroup.PlaceCheckBox("Auto Stop", this, &AccuracyAutoStop);

	AccuracyAutoCrouch.SetFileId("acc_crouch");
	AccuracyGroup.PlaceCheckBox("Auto Crouch", this, &AccuracyAutoCrouch);

	AccuracyAutoScope.SetFileId("acc_scope");
	AccuracyGroup.PlaceCheckBox("Auto Scope", this, &AccuracyAutoScope);

	AccuracyHitchance.SetFileId("acc_chance");
	AccuracyHitchance.SetBoundaries(0, 100);
	AccuracyHitchance.SetValue(10);
	AccuracyGroup.PlaceLabledControl("Hit Chance", this, &AccuracyHitchance);

	AccuracySmart.SetFileId("acc_smart");
	AccuracySmart.SetBoundaries(0, 20);
	AccuracySmart.SetValue(0);
	AccuracyGroup.PlaceLabledControl("Smart Aim", this, &AccuracySmart);

	

#pragma endregion  Accuracy controls get Setup in here

#pragma region AntiAim
	AntiAimGroup.SetPosition(560, -16); //560, 243
	AntiAimGroup.SetText("Anti Aim");
	AntiAimGroup.SetSize(360, 274);
	RegisterControl(&AntiAimGroup);

	AntiAimEnable.SetFileId("aa_enable");
	AntiAimGroup.PlaceCheckBox("Enable", this, &AntiAimEnable);

	AntiAimPitch.SetFileId("aa_x");
	AntiAimPitch.AddItem("None");
	AntiAimPitch.AddItem("Down");
	AntiAimPitch.AddItem("SMAC Safe");
	AntiAimPitch.AddItem("Jitter");
	AntiAimPitch.AddItem("Static");
	AntiAimPitch.AddItem("Fake Down");
	AntiAimPitch.AddItem("Zero");
	AntiAimPitch.AddItem("Fake Zero");
	AntiAimPitch.AddItem("Fake Pitch");
	AntiAimGroup.PlaceLabledControl("Pitch", this, &AntiAimPitch);

	AntiAimYaw.SetFileId("aa_y");
	AntiAimYaw.AddItem("None");
	AntiAimYaw.AddItem("Fake Edge");
	AntiAimYaw.AddItem("Fake Sideways");
	AntiAimYaw.AddItem("Fake Static");
	AntiAimYaw.AddItem("Forward");
	AntiAimYaw.AddItem("LBY Normal");
	AntiAimYaw.AddItem("T Jitter");
	AntiAimYaw.AddItem("Backwards");
	AntiAimYaw.AddItem("Fake LBY");
	AntiAimYaw.AddItem("Fake 1");
	AntiAimYaw.AddItem("Fake 2");
	AntiAimYaw.AddItem("LBY Spin");
	AntiAimYaw.AddItem("Fake 4");
	AntiAimYaw.AddItem("LBY Advanced");
	AntiAimYaw.AddItem("LBY Back");
	AntiAimYaw.AddItem("Fake 90");
	AntiAimYaw.AddItem("Fake 5");
	AntiAimYaw.AddItem("Side");
	AntiAimYaw.AddItem("Slow Spin");
	AntiAimYaw.AddItem("Fast Spin");
	AntiAimYaw.AddItem("LBY 90"); 
	AntiAimYaw.AddItem("Switch Sideways");
	AntiAimYaw.AddItem("Fake Breaker");
	AntiAimYaw.AddItem("Angel Fake Spin");
	AntiAimYaw.AddItem("Side Fake");
	AntiAimGroup.PlaceLabledControl("Yaw", this, &AntiAimYaw);

	Lbytype.SetFileId("aa_lbytype");
	Lbytype.AddItem("None");
	Lbytype.AddItem("Backwards");
	Lbytype.AddItem("Sideways 1");
	Lbytype.AddItem("Sideways 2");
	Lbytype.AddItem("Spin");
	Lbytype.AddItem("Jitter");
	Lbytype.AddItem("LowerBody Adv");
	Lbytype.AddItem("Fake 180");
	Lbytype.AddItem("LowerBody Brk");
	Lbytype.AddItem("Fake Jitter");
	Lbytype.AddItem("Slow Spin");
	Lbytype.AddItem("Fast Spin");
	Lbytype.AddItem("Switch Sideways");
	Lbytype.AddItem("Angel Fake Spin");
	Lbytype.AddItem("Side Fake");
	AntiAimGroup.PlaceLabledControl("Fake Yaw", this, &Lbytype);

	AntiResolver.SetFileId("aa_antiresolver");
	AntiResolver.AddItem("Off");
	AntiResolver.AddItem("Force");
	AntiResolver.AddItem("LBY");
	AntiResolver.AddItem("Auto Flip");
	AntiAimGroup.PlaceLabledControl("Anti Resolver", this, &AntiResolver);

	AntiAimOffset.SetFileId("aa_offset");
	AntiAimOffset.SetBoundaries(0, 360);
	AntiAimOffset.SetValue(69);
	AntiAimGroup.PlaceLabledControl("Custom Yaw", this, &AntiAimOffset);

	AntiAimKnife.SetFileId("aa_knife");
	AntiAimGroup.PlaceCheckBox("Anti Aim with Knife", this, &AntiAimKnife);

	Fakewalk.SetFileId("aa_fakewalk");
	AntiAimGroup.PlaceCheckBox("Fake Walk [Shift]", this, &Fakewalk);

	AccuracyResolverPitch.SetFileId("acc_rpitch");
	AccuracyResolverPitch.AddItem("Off");
	AccuracyResolverPitch.AddItem("Regular");
	AccuracyResolverPitch.AddItem("Advanced");
	AntiAimGroup.PlaceLabledControl("Resolver [X]", this, &AccuracyResolverPitch);

	AccuracyResolverYaw.SetFileId("acc_ryaw");
	AccuracyResolverYaw.AddItem("Off");
	AccuracyResolverYaw.AddItem("Store Angle");
	AccuracyResolverYaw.AddItem("Advanced");
	AccuracyResolverYaw.AddItem("Rand");
	AccuracyResolverYaw.AddItem("Delta");
	AccuracyResolverYaw.AddItem("Angle Clamp");
	AccuracyResolverYaw.AddItem("Full Yaw");
	AccuracyResolverYaw.AddItem("Rand + Force");
	AntiAimGroup.PlaceLabledControl("Resolver [Y]", this, &AccuracyResolverYaw);


#pragma endregion  AntiAim controls get setup in here
}
void CResolverTab::Setup()
{
	SetTitle("RESOLVER");

	ActiveLabel.SetPosition(190, -33);
	ActiveLabel.SetText("Active");
	RegisterControl(&ActiveLabel);

	Active.SetFileId("active");
	Active.SetPosition(170, -32);
	RegisterControl(&Active);

#pragma region Resolveroptions

	ResolverOptions.SetPosition(176, -16);
	ResolverOptions.SetText("Resolver Options");
	ResolverOptions.SetSize(376, 270);
	RegisterControl(&ResolverOptions);

	ResolverEnable.SetFileId("r_enable");
	ResolverOptions.PlaceCheckBox("Enable", this, &ResolverEnable);

	//AccuracyResolverr.SetFileId("r_resolver");
	//ResolverOptions.PlaceLabledControl("Resolver X/Y", this, &AccuracyResolverr);

	



#pragma endregion  Resolver shit
}
void CLegitBotTab::Setup()
{
	SetTitle("LEGITBOT");

	ActiveLabel.SetPosition(190, -33);
	ActiveLabel.SetText("Active");
	RegisterControl(&ActiveLabel);

	Active.SetFileId("active");
	Active.SetPosition(170, -32);
	RegisterControl(&Active);

#pragma region Aimbot
	AimbotGroup.SetPosition(176, -16);
	AimbotGroup.SetText("Aimbot");
	AimbotGroup.SetSize(240, 210);
	RegisterControl(&AimbotGroup);

	AimbotEnable.SetFileId("aim_enable");
	AimbotGroup.PlaceCheckBox("Enable", this, &AimbotEnable);

	AimbotAutoFire.SetFileId("aim_autofire");
	AimbotGroup.PlaceCheckBox("Auto Fire", this, &AimbotAutoFire);

	AimbotFriendlyFire.SetFileId("aim_friendfire");
	AimbotGroup.PlaceCheckBox("Friendly Fire", this, &AimbotFriendlyFire);

	AimbotKeyPress.SetFileId("aim_usekey");
	AimbotGroup.PlaceCheckBox("On Key Press", this, &AimbotKeyPress);

	AimbotKeyBind.SetFileId("aim_key");
	AimbotGroup.PlaceLabledControl("Key Bind", this, &AimbotKeyBind);

	AimbotAutoPistol.SetFileId("aim_apistol");
	AimbotGroup.PlaceCheckBox("Auto Pistol", this, &AimbotAutoPistol);

#pragma endregion Aimbot shit

#pragma region Triggerbot
	TriggerGroup.SetPosition(432, -16);
	TriggerGroup.SetText("Triggerbot");
	TriggerGroup.SetSize(240, 210);
	RegisterControl(&TriggerGroup);

	TriggerEnable.SetFileId("trig_enable");
	TriggerGroup.PlaceCheckBox("Enable", this, &TriggerEnable);

	TriggerKeyPress.SetFileId("trig_onkey");
	TriggerGroup.PlaceCheckBox("On Key Press", this, &TriggerKeyPress);

	TriggerKeyBind.SetFileId("trig_key");
	TriggerGroup.PlaceLabledControl("Key Bind", this, &TriggerKeyBind);

	TriggerDelay.SetFileId("trig_time");
	TriggerDelay.SetBoundaries(1.f, 1000.f);
	TriggerDelay.SetValue(1.f);
	TriggerGroup.PlaceLabledControl("Delay (ms)", this, &TriggerDelay);

	TriggerRecoil.SetFileId("trig_recoil");
	TriggerGroup.PlaceLabledControl("Recoil", this, &TriggerRecoil);

	TriggerFilterGroup.SetPosition(688, -16);
	TriggerFilterGroup.SetText("Triggerbot Filter");
	TriggerFilterGroup.SetSize(240, 210);
	RegisterControl(&TriggerFilterGroup);

	TriggerHead.SetFileId("trig_head");
	TriggerFilterGroup.PlaceCheckBox("Head", this, &TriggerHead);

	TriggerChest.SetFileId("trig_chest");
	TriggerFilterGroup.PlaceCheckBox("Chest", this, &TriggerChest);

	TriggerStomach.SetFileId("trig_stomach");
	TriggerFilterGroup.PlaceCheckBox("Stomach", this, &TriggerStomach);

	TriggerArms.SetFileId("trig_arms");
	TriggerFilterGroup.PlaceCheckBox("Arms", this, &TriggerArms);

	TriggerLegs.SetFileId("trig_legs");
	TriggerFilterGroup.PlaceCheckBox("Legs", this, &TriggerLegs);

	TriggerTeammates.SetFileId("trig_teammates");
	TriggerFilterGroup.PlaceCheckBox("Friendly Fire", this, &TriggerTeammates);
#pragma endregion Triggerbot stuff

#pragma region Main Weapon
	WeaponMainGroup.SetPosition(176, 213);
	WeaponMainGroup.SetText("Rifles");
	WeaponMainGroup.SetSize(240, 210);
	RegisterControl(&WeaponMainGroup);

	WeaponMainSpeed.SetFileId("main_speed");
	WeaponMainSpeed.SetBoundaries(0.1f, 2.f);
	WeaponMainSpeed.SetValue(1.0f);
	WeaponMainGroup.PlaceLabledControl("Smooth", this, &WeaponMainSpeed);

	WeaponMainFoV.SetFileId("main_fov");
	WeaponMainFoV.SetBoundaries(0.f, 30.f);
	WeaponMainFoV.SetValue(5.f);
	WeaponMainGroup.PlaceLabledControl("Fov", this, &WeaponMainFoV);

	WeaponMainRecoil.SetFileId("main_recoil");
	WeaponMainGroup.PlaceCheckBox("Recoil", this, &WeaponMainRecoil);

	WeaponMainPSilent.SetFileId("main_psilent");
	WeaponMainGroup.PlaceCheckBox("Silent", this, &WeaponMainPSilent);

	WeaponMainInacc.SetFileId("main_inacc");
	WeaponMainInacc.SetBoundaries(0.f, 15.f);
	WeaponMainGroup.PlaceLabledControl("Inaccuracy", this, &WeaponMainInacc);

	WeaponMainHitbox.SetFileId("main_hitbox");
	WeaponMainHitbox.AddItem("Head");
	WeaponMainHitbox.AddItem("Neck");
	WeaponMainHitbox.AddItem("Chest");
	WeaponMainHitbox.AddItem("Stomach");
	WeaponMainGroup.PlaceLabledControl("Hitbox", this, &WeaponMainHitbox);
#pragma endregion

#pragma region Pistols
	WeaponPistGroup.SetPosition(432, 213);
	WeaponPistGroup.SetText("Pistols");
	WeaponPistGroup.SetSize(240, 210);
	RegisterControl(&WeaponPistGroup);

	WeaponPistSpeed.SetFileId("pist_speed");
	WeaponPistSpeed.SetBoundaries(0.1f, 2.f);
	WeaponPistSpeed.SetValue(1.0f);
	WeaponPistGroup.PlaceLabledControl("Smooth", this, &WeaponPistSpeed);

	WeaponPistFoV.SetFileId("pist_fov");
	WeaponPistFoV.SetBoundaries(0.1f, 30.f);
	WeaponPistFoV.SetValue(0.78f);
	WeaponPistGroup.PlaceLabledControl("Fov", this, &WeaponPistFoV);

	WeaponPistRecoil.SetFileId("pist_recoil");
	WeaponPistGroup.PlaceCheckBox("Recoil", this, &WeaponPistRecoil);

	WeaponPistPSilent.SetFileId("pist_psilent");
	WeaponPistGroup.PlaceCheckBox("Silent", this, &WeaponPistPSilent);

	WeaponPistInacc.SetFileId("pist_inacc");
	WeaponPistInacc.SetBoundaries(0.f, 15.f);
	WeaponPistInacc.SetValue(0.f);
	WeaponPistGroup.PlaceLabledControl("Inaccuracy", this, &WeaponPistInacc);

	WeaponPistHitbox.SetFileId("pist_hitbox");
	WeaponPistHitbox.AddItem("Head");
	WeaponPistHitbox.AddItem("Neck");
	WeaponPistHitbox.AddItem("Chest");
	WeaponPistHitbox.AddItem("Stomach");
	WeaponPistGroup.PlaceLabledControl("Hitbox", this, &WeaponPistHitbox);
#pragma endregion

#pragma region Snipers
	WeaponSnipGroup.SetPosition(688, 213);
	WeaponSnipGroup.SetText("Snipers");
	WeaponSnipGroup.SetSize(240, 210);
	RegisterControl(&WeaponSnipGroup);



	WeaponSnipSpeed.SetFileId("wconf_speed_ak");
	WeaponSnipSpeed.SetBoundaries(0.1f, 2.f);
	WeaponSnipSpeed.SetValue(1.0f);
	WeaponSnipGroup.PlaceLabledControl("Smooth", this, &WeaponSnipSpeed);

	WeaponSnipFoV.SetFileId("wconf_fov_ak");
	WeaponSnipFoV.SetBoundaries(0.1f, 30.f);
	WeaponSnipFoV.SetValue(5.f);
	WeaponSnipGroup.PlaceLabledControl("Fov", this, &WeaponSnipFoV);

	WeaponSnipRecoil.SetFileId("wconf_recoil_ak");
	WeaponSnipGroup.PlaceCheckBox("Recoil", this, &WeaponSnipRecoil);

	WeaponSnipPSilent.SetFileId("wconf_psilent_ak");
	WeaponSnipGroup.PlaceCheckBox("Silent", this, &WeaponSnipPSilent);

	WeaponSnipInacc.SetFileId("wconf_inacc_ak");
	WeaponSnipInacc.SetBoundaries(0.f, 15.f);
	WeaponSnipGroup.PlaceLabledControl("Inaccuracy", this, &WeaponSnipInacc);

	WeaponSnipHitbox.SetFileId("wconf_hitbox_ak");
	WeaponSnipHitbox.AddItem("Head");
	WeaponSnipHitbox.AddItem("Neck");
	WeaponSnipHitbox.AddItem("Chest");
	WeaponSnipHitbox.AddItem("Stomach");
	WeaponSnipGroup.PlaceLabledControl("Hitbox", this, &WeaponSnipHitbox);

	WeaponSnipEnable.SetFileId("wconf_enable_ak");
	WeaponSnipGroup.PlaceCheckBox("Aimbot Unscoped", this, &WeaponSnipEnable);
#pragma endregion

#pragma More



#pragma endregion
}

void CVisualTab::Setup()
{
	SetTitle("VISUALS");

	ActiveLabel.SetPosition(190, -33);
	ActiveLabel.SetText("Active");
	RegisterControl(&ActiveLabel);

	Active.SetFileId("active");
	Active.SetPosition(170, -32);
	RegisterControl(&Active);

#pragma region Options
	OptionsGroup.SetText("Options");
	OptionsGroup.SetPosition(176, -16);
	OptionsGroup.SetSize(193, 430);
	RegisterControl(&OptionsGroup);

	OptionsBox.SetFileId("opt_box");
	OptionsBox.AddItem("None");
	OptionsBox.AddItem("Corners");
	OptionsBox.AddItem("Full");
	OptionsGroup.PlaceLabledControl("Box", this, &OptionsBox);

	OptionsBox.SetFileId("opt_boxfill");
	OptionsGroup.PlaceCheckBox("Fill", this, &OptionsBoxFill);

	OptionsBox.SetFileId("opt_boxcolor");
	OptionsGroup.PlaceCheckBox("Colorize fill", this, &OptionsBoxFillColor);

	OptionsName.SetFileId("opt_name");
	OptionsGroup.PlaceCheckBox("Name", this, &OptionsName);

	OptionsHealth.SetFileId("opt_hp");
	OptionsGroup.PlaceCheckBox("Health", this, &OptionsHealth);

	OptionsArmor.SetFileId("opt_armor");
	OptionsGroup.PlaceCheckBox("Armor", this, &OptionsArmor);

	OptionsDistance.SetFileId("opt_distance");
	OptionsGroup.PlaceCheckBox("Draw Distance", this, &OptionsDistance);

	OptionsWeapon.SetFileId("opt_distaqdwnce");
	OptionsGroup.PlaceCheckBox("Weapons", this, &OptionsWeapon);

	OptionsInfo.SetFileId("opt_info");
	OptionsGroup.PlaceCheckBox("Info", this, &OptionsInfo);

	OptionsChams.SetFileId("opt_chams");
	OptionsChams.AddItem("Off");
	OptionsChams.AddItem("Normal");
	OptionsChams.AddItem("Flat");
	OptionsGroup.PlaceLabledControl("Chams", this, &OptionsChams);

	OptionsSkeleton.SetFileId("opt_bone");
	OptionsGroup.PlaceCheckBox("Skeleton", this, &OptionsSkeleton);

	OptionsVisibleOnly.SetFileId("opt_vonly");
	OptionsGroup.PlaceCheckBox("Visible Only", this, &OptionsVisibleOnly);

	OptionsAimSpot.SetFileId("opt_aimspot");
	OptionsGroup.PlaceCheckBox("Head Cross", this, &OptionsAimSpot);

	OptionsCompRank.SetFileId("opt_comprank");
	OptionsGroup.PlaceCheckBox("Player Ranks", this, &OptionsCompRank);

	OtherWireframe.SetFileId("opt_wireframe");
	OptionsGroup.PlaceCheckBox("Wireframe", this, &OtherWireframe);
	
	OtherNightMode.SetFileId("otr_nightmode");
	OptionsGroup.PlaceCheckBox("Night Mode", this, &OtherNightMode);


#pragma endregion Setting up the Options controls

#pragma region Filters
	FiltersGroup.SetText("Filters");
	FiltersGroup.SetPosition(732, -16);
	FiltersGroup.SetSize(193, 430);
	RegisterControl(&FiltersGroup);

	FiltersAll.SetFileId("ftr_all");
	FiltersGroup.PlaceCheckBox("All", this, &FiltersAll);

	FiltersPlayers.SetFileId("ftr_players");
	FiltersGroup.PlaceCheckBox("Players", this, &FiltersPlayers);

	FiltersEnemiesOnly.SetFileId("ftr_enemyonly");
	FiltersGroup.PlaceCheckBox("Enemies Only", this, &FiltersEnemiesOnly);

	FiltersWeapons.SetFileId("ftr_weaps");
	FiltersGroup.PlaceCheckBox("Weapons", this, &FiltersWeapons);

	FiltersChickens.SetFileId("ftr_chickens");
	FiltersGroup.PlaceCheckBox("Chickens", this, &FiltersChickens);

	FiltersC4.SetFileId("ftr_c4");
	FiltersGroup.PlaceCheckBox("C4", this, &FiltersC4);

	Granades.SetFileId("frt_granades");
	FiltersGroup.PlaceCheckBox("Granade Trajectory", this, &Granades);
#pragma endregion Setting up the Filters controls

#pragma region Other
	OtherGroup.SetText("Other");
	OtherGroup.SetPosition(392, -16);
	OtherGroup.SetSize(334, 430);
	RegisterControl(&OtherGroup);

	OtherCrosshair.SetFileId("otr_xhair");
	OtherGroup.PlaceCheckBox("Crosshair", this, &OtherCrosshair);

	//OtherRecoilCrosshair.SetFileId("otr_recoilhair");
	//OtherRecoilCrosshair.AddItem("Off");
	//OtherRecoilCrosshair.AddItem("Recoil Position");
	//OtherRecoilCrosshair.AddItem("Radius");
	//OtherGroup.PlaceLabledControl("Recoil Crosshair", this, &OtherRecoilCrosshair);

	OtherRadar.SetFileId("otr_radar");
	OtherGroup.PlaceCheckBox("Radar", this, &OtherRadar);

	OtherNoVisualRecoil.SetFileId("otr_visrecoil");
	OtherGroup.PlaceCheckBox("No Visual Recoil", this, &OtherNoVisualRecoil);

	OtherNoFlash.SetFileId("otr_noflash");
	//OtherNoFlas.SetBoundaries(0.f, 90.f);
	//OtherViewmodelFOV.SetValue(0.f);
	OtherGroup.PlaceCheckBox("No Flash Enable", this, &OtherNoFlash);

	OtherNoFlashVal.SetFileId("otr_noflashval");
	OtherNoFlashVal.SetBoundaries(0, 255);
	OtherViewmodelFOV.SetValue(200);
	OtherGroup.PlaceLabledControl("Max Flash", this, &OtherNoFlashVal);

	OtherNoHands.SetFileId("otr_hands");
	OtherNoHands.AddItem("Off");
	OtherNoHands.AddItem("None");
	OtherNoHands.AddItem("Transparent");
	OtherNoHands.AddItem("Chams");
	OtherNoHands.AddItem("Rainbow");
	OtherNoHands.AddItem("Wireframe");
	OtherGroup.PlaceLabledControl("Modify Hands", this, &OtherNoHands);

	WeaponChams.SetFileId("weapon_chans");
	WeaponChams.AddItem("Off");
	WeaponChams.AddItem("Chams");
	WeaponChams.AddItem("Remove Weapon");
	WeaponChams.AddItem("Rainbow");
	WeaponChams.AddItem("Transparent");
	OtherGroup.PlaceLabledControl("Modify Weapons", this, &WeaponChams);

	OtherViewmodelFOV.SetFileId("otr_viewfov");
	OtherViewmodelFOV.SetBoundaries(0.f, 90.f);
	OtherViewmodelFOV.SetValue(0.f);
	OtherGroup.PlaceLabledControl("Viewmodel FOV Changer", this, &OtherViewmodelFOV);

	OtherFOV.SetFileId("otr_fov");
	OtherFOV.SetBoundaries(0.f, 180.f);
	OtherFOV.SetValue(90.f);
	OtherGroup.PlaceLabledControl("Field of View Changer", this, &OtherFOV);

	OtherNoSky.SetFileId("otr_nosky");
	OtherGroup.PlaceCheckBox("NoSky", this, &OtherNoSky);

	OtherNoSmoke.SetFileId("otr_nosmoke");
	OtherGroup.PlaceCheckBox("NoSmoke", this, &OtherNoSmoke);


#pragma endregion Setting up the Other controls
}

void CMiscTab::Setup()
{
	SetTitle("MISC");

#pragma region Knife
	/*KnifeGroup.SetPosition(16, 16);
	KnifeGroup.SetSize(360, 126);
	KnifeGroup.SetText("Knife Changer");
	RegisterControl(&KnifeGroup);

	KnifeEnable.SetFileId("knife_enable");
	KnifeGroup.PlaceLabledControl("Enable", this, &KnifeEnable);

	KnifeModel.SetFileId("knife_model");
	KnifeModel.AddItem("Karambit");
	KnifeModel.AddItem("Bayonet");
	KnifeModel.AddItem("M9 Bayonet");
	KnifeModel.AddItem("Flip Knife");
	KnifeModel.AddItem("Gut Knife");
	KnifeModel.AddItem("Huntsman Knife");
	KnifeModel.AddItem("Falchion Knife");
	KnifeModel.AddItem("Bowie Knife");
	KnifeModel.AddItem("Butterfly Knife");
	KnifeModel.AddItem("Shadow Daggers");
	KnifeGroup.PlaceLabledControl("Knife", this, &KnifeModel);

	KnifeSkin.SetFileId("knife_skin");
	KnifeSkin.AddItem("Doppler Sapphire");
	KnifeSkin.AddItem("Doppler Ruby");
	KnifeSkin.AddItem("Tiger Tooth");
	KnifeSkin.AddItem("Lore");
	KnifeSkin.AddItem("Forest DDPAT");
	KnifeSkin.AddItem("Crimson Web");
	KnifeSkin.AddItem("Slaughter");
	KnifeSkin.AddItem("Vanilla");
	KnifeSkin.AddItem("Fade");
	KnifeSkin.AddItem("Night");
	KnifeSkin.AddItem("Blue Steel");
	KnifeSkin.AddItem("Stained");
	KnifeSkin.AddItem("Case Hardended");
	KnifeSkin.AddItem("Safari Mesh");
	KnifeSkin.AddItem("Boreal Forest");
	KnifeSkin.AddItem("Ultaviolet");
	KnifeSkin.AddItem("Rust Coat");
	KnifeSkin.AddItem("Romania's Flag <3");


	KnifeGroup.PlaceLabledControl("Skin", this, &KnifeSkin);

	KnifeApply.SetText("Apply Knife");
	KnifeApply.SetCallback(KnifeApplyCallbk);
	KnifeGroup.PlaceLabledControl("", this, &KnifeApply);
	*/
#pragma endregion

#pragma region Other
	OtherGroup.SetPosition(540, -16);
	OtherGroup.SetSize(360, 430);
	OtherGroup.SetText("Other");
	RegisterControl(&OtherGroup);

	OtherAutoJump.SetFileId("otr_autojump");
	OtherGroup.PlaceCheckBox("Bhop", this, &OtherAutoJump);

	OtherEdgeJump.SetFileId("otr_edgejump");
	OtherGroup.PlaceCheckBox("Edge Jump", this, &OtherEdgeJump);

	OtherAutoStrafe.SetFileId("otr_strafe");
	OtherAutoStrafe.AddItem("Off");
	OtherAutoStrafe.AddItem("Legit");
	OtherAutoStrafe.AddItem("Rage");
	OtherGroup.PlaceLabledControl("Auto Strafer", this, &OtherAutoStrafe);

	OtherAutoCounterStafe.SetFileId("otr_autocs");
	OtherGroup.PlaceCheckBox("CStrafe", this, &OtherAutoCounterStafe);

	OtherAutoCounterStafeKey.SetFileId("otr_autocs_key");
	OtherGroup.PlaceLabledControl("CStrafe Key", this, &OtherAutoCounterStafeKey);

	OtherSafeMode.SetFileId("otr_safemode");
	OtherSafeMode.SetState(true);
	OtherGroup.PlaceCheckBox("Safe Mode", this, &OtherSafeMode);

	OtherChatSpam.SetFileId("otr_spam");
	OtherChatSpam.AddItem("Off");
	OtherChatSpam.AddItem("Namestealer");
	OtherChatSpam.AddItem("SENSATION - ChatS");
	OtherChatSpam.AddItem("SENSATION - NameS");
	OtherChatSpam.AddItem("Rekt");
	OtherGroup.PlaceLabledControl("Chat Spam", this, &OtherChatSpam);

	OtherClantag.SetFileId("otr_spam");
	OtherClantag.AddItem("Off");
	OtherClantag.AddItem("SENSATION");
	OtherClantag.AddItem("'SlideShow'");
	OtherClantag.AddItem("zu krass $"); // wyjeb aimware chat name
	OtherGroup.PlaceLabledControl("Clan Tag", this, &OtherClantag);


	OtherTeamChat.SetFileId("otr_teamchat");
	OtherGroup.PlaceCheckBox("Team Chat Only", this, &OtherTeamChat);

	OtherChatDelay.SetFileId("otr_chatdelay");
	OtherChatDelay.SetBoundaries(0.1, 3.0);
	OtherChatDelay.SetValue(0.5);
	OtherGroup.PlaceLabledControl("Spam Delay", this, &OtherChatDelay);

	OtherAirStuck.SetFileId("otr_astuck");
	OtherGroup.PlaceLabledControl("Air Stuck", this, &OtherAirStuck);

	SlowMotion.SetFileId("otr_astuck");
	OtherGroup.PlaceLabledControl("Slow Motion", this, &SlowMotion);


	OtherThirdperson.SetFileId("aa_thirdpsr");
	OtherGroup.PlaceCheckBox("Thirdperson", this, &OtherThirdperson);

	LBYIndicator.SetFileId("otr_lbyidi");
	OtherGroup.PlaceCheckBox("LBY Indicator", this, &LBYIndicator);

	OtherNoScopeBorder.SetFileId("otr_noscope");
	OtherGroup.PlaceCheckBox("No Scope", this, &OtherNoScopeBorder);


	//DisableAll.SetFileId("otr_disableall");
	//OtherGroup.PlaceLabledControl("Disable All", this, &DisableAll);

#pragma endregion other random options

#pragma region FakeLag
	FakeLagGroup.SetPosition(176, -16);
	FakeLagGroup.SetSize(360, 141);
	FakeLagGroup.SetText("Fake Lag");
	RegisterControl(&FakeLagGroup);

	FakeLagEnable.SetFileId("fakelag_enable");
	FakeLagGroup.PlaceCheckBox("Fake Lag", this, &FakeLagEnable);

	FakeLagChoke.SetFileId("fakelag_choke");
	FakeLagChoke.SetBoundaries(0, 16);
	FakeLagChoke.SetValue(0);
	FakeLagGroup.PlaceLabledControl("Choke Factor", this, &FakeLagChoke);

	FakeLagSend.SetFileId("fakelag_send");
	FakeLagSend.SetBoundaries(0, 16);
	FakeLagSend.SetValue(0);
	FakeLagGroup.PlaceLabledControl("Send Factor", this, &FakeLagSend);

	ChokeRandomize.SetFileId("choke_random");
	FakeLagGroup.PlaceCheckBox("Randomize Choke", this, &ChokeRandomize);

	SendRandomize.SetFileId("send_random");
	FakeLagGroup.PlaceCheckBox("Randomize Send", this, &SendRandomize);
#pragma endregion fakelag shit

#pragma region Teleport
	TeleportGroup.SetPosition(176, 176);
	TeleportGroup.SetSize(360, 120);
	TeleportGroup.SetText("Configs");
	RegisterControl(&TeleportGroup);
	RegisterControl(&SaveButton);
	RegisterControl(&LoadButton);

	//TeleportEnable.SetFileId("teleport_enable");
	//TeleportGroup.PlaceCheckBox("Enable", this, &TeleportEnable);

	//TeleportKey.SetFileId("teleport_key");
	//TeleportGroup.PlaceLabledControl("Key", this, &TeleportKey);

	SaveButton.SetText("Save Configuration");
	SaveButton.SetCallback(SaveCallbk);
	TeleportGroup.PlaceLabledControl("Save", this, &SaveButton);

	LoadButton.SetText("Load Configuration");
	LoadButton.SetCallback(LoadCallbk);
	TeleportGroup.PlaceLabledControl("Load", this, &LoadButton);

	//Cfgtext.SetFileId("cfgtext");
	//ButtonGroup.PlaceLabledControl(this, &Cfgtext, "");

	SetNr.AddItem("Legit");
	SetNr.AddItem("Semi Rage");
	SetNr.AddItem("Rage");
	SetNr.AddItem("Custom");
	//SetNr.AddItem("Custom 2");
	//SetNr.AddItem("Disable All");
	TeleportGroup.PlaceLabledControl("CFG", this, &SetNr);

#pragma endregion

#pragma region MPs
	//MPGroup.SetPosition(16, 267);
	//MPGroup.SetText("MPs");
	//MPGroup.SetSize(376, 165);
	//RegisterControl(&MPGroup);

	//MAC10Skin.SetFileId("mac10_skin");
	//MAC10Skin.AddItem("Tornado");
	//MAC10Skin.AddItem("Candy Apple");
	//MAC10Skin.AddItem("Silver");
	//MAC10Skin.AddItem("Forest DDPAT");
	//MAC10Skin.AddItem("Urban DDPAT");
	//MAC10Skin.AddItem("Fade");
	//MAC10Skin.AddItem("Neon Rider");
	//MAC10Skin.AddItem("Ultraviolet");
	//MAC10Skin.AddItem("Palm");
	//MAC10Skin.AddItem("Graven");
	//MAC10Skin.AddItem("Tatter");
	//MAC10Skin.AddItem("Amber Fade");
	//MAC10Skin.AddItem("Heat");
	//MAC10Skin.AddItem("Curse");
	//MAC10Skin.AddItem("Indigo");
	//MAC10Skin.AddItem("Commuter");
	//MAC10Skin.AddItem("Nuclear Garden");
	//MAC10Skin.AddItem("Malachite");
	//MAC10Skin.AddItem("Rangeen");
	//MAC10Skin.AddItem("Lapis Gator");
	//MPGroup.PlaceLabledControl("MAC-10", this, &MAC10Skin);

	//P90Skin.SetFileId("p90_skin");
	//P90Skin.AddItem("Leather");
	//P90Skin.AddItem("Virus");
	//P90Skin.AddItem("Contrast Spray");
	//P90Skin.AddItem("Storm");
	//P90Skin.AddItem("Cold Blooded");
	//P90Skin.AddItem("Glacier Mesh");
	//P90Skin.AddItem("Sand Spray");
	//P90Skin.AddItem("Death by Kitty");
	//P90Skin.AddItem("Ash Wood");
	//P90Skin.AddItem("Fallout Warning");
	//P90Skin.AddItem("Scorched");
	//P90Skin.AddItem("Emerald Dragon");
	//P90Skin.AddItem("Teardown");
	//P90Skin.AddItem("Blind Spot");
	//P90Skin.AddItem("Trigon");
	//P90Skin.AddItem("Desert Warfare");
	//P90Skin.AddItem("Module");
	//P90Skin.AddItem("Asiimov");
	//P90Skin.AddItem("Elite Build");
	//P90Skin.AddItem("Shapewood");
	//P90Skin.AddItem("Shallow Grave");
	//MPGroup.PlaceLabledControl("P90", this, &P90Skin);

	//UMP45Skin.SetFileId("ump45_skin");
	//UMP45Skin.AddItem("Blaze");
	//UMP45Skin.AddItem("Forest DDPAT");
	//UMP45Skin.AddItem("Gunsmoke");
	//UMP45Skin.AddItem("Urban DDPAT");
	//UMP45Skin.AddItem("Grand Prix");
	//UMP45Skin.AddItem("Carbon Fiber");
	//UMP45Skin.AddItem("Caramel");
	//UMP45Skin.AddItem("Fallout Warning");
	//UMP45Skin.AddItem("Scorched");
	//UMP45Skin.AddItem("Bone Pile");
	//UMP45Skin.AddItem("Delusion");
	//UMP45Skin.AddItem("Corporal");
	//UMP45Skin.AddItem("Indigo");
	//UMP45Skin.AddItem("Labyrinth");
	//UMP45Skin.AddItem("Minotaur's Labyrinth");
	//UMP45Skin.AddItem("Riot");
	//UMP45Skin.AddItem("Primal Saber");
	//MPGroup.PlaceLabledControl("UMP-45", this, &UMP45Skin);

	//BIZONSkin.SetFileId("bizon_skin");
	//BIZONSkin.AddItem("Blue Streak");
	//BIZONSkin.AddItem("Modern Hunter");
	//BIZONSkin.AddItem("Forest Leaves");
	//BIZONSkin.AddItem("Bone Mask");
	//BIZONSkin.AddItem("Carbon Fiber");
	//BIZONSkin.AddItem("Sand Dashed");
	//BIZONSkin.AddItem("Urban Dashed");
	//BIZONSkin.AddItem("Brass");
	//BIZONSkin.AddItem("VariCamo");
	//BIZONSkin.AddItem("Irradiated Alert");
	//BIZONSkin.AddItem("Rust Coat");
	//BIZONSkin.AddItem("Water Sigil");
	//BIZONSkin.AddItem("Night Ops");
	//BIZONSkin.AddItem("Cobalt Halftone");
	//BIZONSkin.AddItem("Antique");
	//BIZONSkin.AddItem("Rust Coat");
	//BIZONSkin.AddItem("Osiris");
	//BIZONSkin.AddItem("Chemical Green");
	//BIZONSkin.AddItem("Bamboo Print");
	//BIZONSkin.AddItem("Bamboo Forest");
	//BIZONSkin.AddItem("Fuel Rod");
	//BIZONSkin.AddItem("Photic Zone");
	//BIZONSkin.AddItem("Judgement of Anubis");
	//MPGroup.PlaceLabledControl("PP-Bizon", this, &BIZONSkin);

	//MP7Skin.SetFileId("mp7_skin");
	//MP7Skin.AddItem("Groundwater");
	//MP7Skin.AddItem("Whiteout");
	//MP7Skin.AddItem("Forest DDPAT");
	//MP7Skin.AddItem("Anodized Navy");
	//MP7Skin.AddItem("Skulls");
	//MP7Skin.AddItem("Gunsmoke");
	//MP7Skin.AddItem("Contrast Spray");
	//MP7Skin.AddItem("Bone Mask");
	//MP7Skin.AddItem("Ossified");
	//MP7Skin.AddItem("Orange Peel");
	//MP7Skin.AddItem("VariCamo");
	//MP7Skin.AddItem("Army Recon");
	//MP7Skin.AddItem("Groundwater");
	//MP7Skin.AddItem("Ocean Foam");
	//MP7Skin.AddItem("Full Stop");
	//MP7Skin.AddItem("Urban Hazard");
	//MP7Skin.AddItem("Olive Plaid");
	//MP7Skin.AddItem("Armor Core");
	//MP7Skin.AddItem("Asterion");
	//MP7Skin.AddItem("Nemesis");
	//MP7Skin.AddItem("Special Delivery");
	//MP7Skin.AddItem("Impire");
	//MPGroup.PlaceLabledControl("MP7", this, &MP7Skin);

	//MP9Skin.SetFileId("mp9_skin");
	//MP9Skin.AddItem("Ruby Poison Dart");
	//MP9Skin.AddItem("Bone Mask");
	//MP9Skin.AddItem("Hot Rod");
	//MP9Skin.AddItem("Storm");
	//MP9Skin.AddItem("Bulldozer");
	//MP9Skin.AddItem("Hypnotic");
	//MP9Skin.AddItem("Sand Dashed");
	//MP9Skin.AddItem("Orange Peel");
	//MP9Skin.AddItem("Dry Season");
	//MP9Skin.AddItem("Dark Age");
	//MP9Skin.AddItem("Rose Iron");
	//MP9Skin.AddItem("Green Plaid");
	//MP9Skin.AddItem("Setting Sun");
	//MP9Skin.AddItem("Dart");
	//MP9Skin.AddItem("Deadly Poison");
	//MP9Skin.AddItem("Pandora's Box");
	//MP9Skin.AddItem("Bioleak");
	//MP9Skin.AddItem("Airlock");
	//MPGroup.PlaceLabledControl("MP9", this, &MP9Skin);

	/*#pragma region OverideFov
	FOVGroup.SetPosition(16, 365);
	FOVGroup.SetSize(360, 75);
	FOVGroup.SetText("FOV Changer");
	RegisterControl(&FOVGroup);

	FOVEnable.SetFileId("fov_enable");
	FOVGroup.PlaceLabledControl("Enable", this, &FOVEnable);

	FOVSlider.SetFileId("fov_slider");
	FOVSlider.SetBoundaries(0, 200);
	FOVSlider.SetValue(0);
	FOVGroup.PlaceLabledControl("FOV Amount", this, &FOVSlider);

	#pragma endregion*/
}

//void CPlayersTab::Setup()
//{
//	SetTitle("PlayerList");
//
//#pragma region PList
//
//	pListGroup.SetPosition(16, 16);
//	pListGroup.SetSize(680, 200);
//	pListGroup.SetText("Player List");
//	pListGroup.SetColumns(2);
//	RegisterControl(&pListGroup);
//
//	pListPlayers.SetPosition(26, 46);
//	pListPlayers.SetSize(640, 50);
//	pListPlayers.SetHeightInItems(20);
//	RegisterControl(&pListPlayers);
//
//#pragma endregion
//
//#pragma region Options
//	
//	OptionsGroup.SetPosition(16, 257);
//	OptionsGroup.SetSize(450, 120);
//	OptionsGroup.SetText("Player Options");
//	RegisterControl(&OptionsGroup);
//
//	OptionsFriendly.SetFileId("pl_friendly");
//	OptionsGroup.PlaceLabledControl("Friendly", this, &OptionsFriendly);
//
//	OptionsAimPrio.SetFileId("pl_priority");
//	OptionsGroup.PlaceLabledControl("Priority", this, &OptionsAimPrio);
//
//	OptionsCalloutSpam.SetFileId("pl_callout");
//	OptionsGroup.PlaceLabledControl("Callout Spam", this, &OptionsCalloutSpam);
//
//#pragma endregion
//}
//
//DWORD GetPlayerListIndex(int EntId)
//{
//	player_info_t pinfo;
//	Interfaces::Engine->GetPlayerInfo(EntId, &pinfo);
//
//	// Bot
//	if (pinfo.guid[0] == 'B' && pinfo.guid[1] == 'O')
//	{
//		char buf[64]; sprintf_s(buf, "BOT_420%sAY", pinfo.name);
//		return CRC32(buf, 64);
//	}
//	else
//	{
//		return CRC32(pinfo.guid, 32);
//	}
//}
//
//bool IsFriendly(int EntId)
//{
//	DWORD plistId = GetPlayerListIndex(EntId);
//	if (PlayerList.find(plistId) != PlayerList.end())
//	{
//		return PlayerList[plistId].Friendly;
//	}
//
//	return false;
//}
//
//bool IsAimPrio(int EntId)
//{
//	DWORD plistId = GetPlayerListIndex(EntId);
//	if (PlayerList.find(plistId) != PlayerList.end())
//	{
//		return PlayerList[plistId].AimPrio;
//	}
//
//	return false;
//}
//
//bool IsCalloutTarget(int EntId)
//{
//	DWORD plistId = GetPlayerListIndex(EntId);
//	if (PlayerList.find(plistId) != PlayerList.end())
//	{
//		return PlayerList[plistId].Callout;
//	}
//
//	return false;
//}
//
//void UpdatePlayerList()
//{
//	IClientEntity* pLocal = Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());
//	if (Interfaces::Engine->IsConnected() && Interfaces::Engine->IsInGame() && pLocal && pLocal->IsAlive())
//	{
//		Menu::Window.Playerlist.pListPlayers.ClearItems();
//
//		// Loop through all active entitys
//		for (int i = 0; i < Interfaces::EntList->GetHighestEntityIndex(); i++)
//		{
//			// Get the entity
//
//			player_info_t pinfo;
//			if (i != Interfaces::Engine->GetLocalPlayer() && Interfaces::Engine->GetPlayerInfo(i, &pinfo))
//			{
//				IClientEntity* pEntity = Interfaces::EntList->GetClientEntity(i);
//				int HP = 100; char* Location = "Unknown";
//				char *Friendly = " ", *AimPrio = " ";
//
//				DWORD plistId = GetPlayerListIndex(Menu::Window.Playerlist.pListPlayers.GetValue());
//				if (PlayerList.find(plistId) != PlayerList.end())
//				{
//					Friendly = PlayerList[plistId].Friendly ? "Friendly" : "";
//					AimPrio = PlayerList[plistId].AimPrio ? "AimPrio" : "";
//				}
//
//				if (pEntity && !pEntity->IsDormant())
//				{
//					HP = pEntity->GetHealth();
//					Location = pEntity->GetLastPlaceName();
//				}
//
//				char nameBuffer[512];
//				sprintf_s(nameBuffer, "%-24s %-10s %-10s [%d HP] [Last Seen At %s]", pinfo.name, IsFriendly(i) ? "Friend" : " ", IsAimPrio(i) ? "AimPrio" : " ", HP, Location);
//				Menu::Window.Playerlist.pListPlayers.AddItem(nameBuffer, i);
//
//			}
//
//		}
//
//		DWORD meme = GetPlayerListIndex(Menu::Window.Playerlist.pListPlayers.GetValue());
//
//		// Have we switched to a different player?
//		static int PrevSelectedPlayer = 0;
//		if (PrevSelectedPlayer != Menu::Window.Playerlist.pListPlayers.GetValue())
//		{
//			if (PlayerList.find(meme) != PlayerList.end())
//			{
//				Menu::Window.Playerlist.OptionsFriendly.SetState(PlayerList[meme].Friendly);
//				Menu::Window.Playerlist.OptionsAimPrio.SetState(PlayerList[meme].AimPrio);
//				Menu::Window.Playerlist.OptionsCalloutSpam.SetState(PlayerList[meme].Callout);
//
//			}
//			else
//			{
//				Menu::Window.Playerlist.OptionsFriendly.SetState(false);
//				Menu::Window.Playerlist.OptionsAimPrio.SetState(false);
//				Menu::Window.Playerlist.OptionsCalloutSpam.SetState(false);
//
//			}
//		}
//		PrevSelectedPlayer = Menu::Window.Playerlist.pListPlayers.GetValue();
//
//		PlayerList[meme].Friendly = Menu::Window.Playerlist.OptionsFriendly.GetState();
//		PlayerList[meme].AimPrio = Menu::Window.Playerlist.OptionsAimPrio.GetState();
//		PlayerList[meme].Callout = Menu::Window.Playerlist.OptionsCalloutSpam.GetState();
//	}
//}

void CColorTab::Setup()
{
	SetTitle("COLORS");
#pragma region Colors
	ColorsGroup.SetText("Colors");
	ColorsGroup.SetPosition(176, -16);
	ColorsGroup.SetSize(360, 600);
	RegisterControl(&ColorsGroup);

	CTColorVisR.SetFileId("ct_color_vis_r");
	CTColorVisR.SetBoundaries(0.f, 255.f);
	CTColorVisR.SetValue(120.f);
	ColorsGroup.PlaceLabledControl("CT Visible Red", this, &CTColorVisR);

	CTColorVisG.SetFileId("ct_color_vis_g");
	CTColorVisG.SetBoundaries(0.f, 255.f);
	CTColorVisG.SetValue(210.f);
	ColorsGroup.PlaceLabledControl("CT Visible Green", this, &CTColorVisG);

	CTColorVisB.SetFileId("ct_color_vis_b");
	CTColorVisB.SetBoundaries(0.f, 255.f);
	CTColorVisB.SetValue(26.f);
	ColorsGroup.PlaceLabledControl("CT Visible Blue", this, &CTColorVisB);


	CTColorNoVisR.SetFileId("ct_color_no_vis_r");
	CTColorNoVisR.SetBoundaries(0.f, 255.f);
	CTColorNoVisR.SetValue(15.f);
	ColorsGroup.PlaceLabledControl("CT Not Visible Rred", this, &CTColorNoVisR);

	CTColorNoVisG.SetFileId("ct_color_no_vis_g");
	CTColorNoVisG.SetBoundaries(0.f, 255.f);
	CTColorNoVisG.SetValue(110.f);
	ColorsGroup.PlaceLabledControl("CT Not Visible Green", this, &CTColorNoVisG);

	CTColorNoVisB.SetFileId("ct_color_no_vis_b");
	CTColorNoVisB.SetBoundaries(0.f, 255.f);
	CTColorNoVisB.SetValue(220.f);
	ColorsGroup.PlaceLabledControl("CT Not Visible Blue", this, &CTColorNoVisB);

	ColorSpacer.SetPosition(16, 16);
	ColorSpacer.SetText(" ");
	ColorsGroup.PlaceLabledControl(" ", this, &ColorSpacer);

	TColorVisR.SetFileId("t_color_vis_r");
	TColorVisR.SetBoundaries(0.f, 255.f);
	TColorVisR.SetValue(235.f);
	ColorsGroup.PlaceLabledControl("T Visible Red", this, &TColorVisR);

	TColorVisG.SetFileId("t_color_vis_g");
	TColorVisG.SetBoundaries(0.f, 255.f);
	TColorVisG.SetValue(200.f);
	ColorsGroup.PlaceLabledControl("T Visible Green", this, &TColorVisG);

	TColorVisB.SetFileId("t_color_vis_b");
	TColorVisB.SetBoundaries(0.f, 255.f);
	TColorVisB.SetValue(0.f);
	ColorsGroup.PlaceLabledControl("T Visible Blue", this, &TColorVisB);


	TColorNoVisR.SetFileId("t_color_no_vis_r");
	TColorNoVisR.SetBoundaries(0.f, 255.f);
	TColorNoVisR.SetValue(235.f);
	ColorsGroup.PlaceLabledControl("T Not Visible Red", this, &TColorNoVisR);

	TColorNoVisG.SetFileId("t_color_no_vis_g");
	TColorNoVisG.SetBoundaries(0.f, 255.f);
	TColorNoVisG.SetValue(50.f);
	ColorsGroup.PlaceLabledControl("T Not Visible Green", this, &TColorNoVisG);

	TColorNoVisB.SetFileId("t_color_no_vis_b");
	TColorNoVisB.SetBoundaries(0.f, 255.f);
	TColorNoVisB.SetValue(0.f);
	ColorsGroup.PlaceLabledControl("T Not Visible Blue", this, &TColorNoVisB);

	HandsR.SetFileId("HandsR");
	HandsR.SetBoundaries(0.f, 255.f);
	HandsR.SetValue(255.f);
	ColorsGroup.PlaceLabledControl("Hands Red", this, &HandsR);

	HandsG.SetFileId("HandsG");
	HandsG.SetBoundaries(0.f, 255.f);
	HandsG.SetValue(255.f);
	ColorsGroup.PlaceLabledControl("Hands Green", this, &HandsG);

	HandsB.SetFileId("HandsB");
	HandsB.SetBoundaries(0.f, 255.f);
	HandsB.SetValue(255.f);
	ColorsGroup.PlaceLabledControl("Hands Blue", this, &HandsB);

	WeaponR.SetFileId("weaponr");
	WeaponR.SetBoundaries(0.f, 255.f);
	WeaponR.SetValue(255.f);
	ColorsGroup.PlaceLabledControl("Weapon Red", this, &WeaponR);

	WeaponG.SetFileId("weaponG");
	WeaponG.SetBoundaries(0.f, 255.f);
	WeaponG.SetValue(255.f);
	ColorsGroup.PlaceLabledControl("Weapon Green", this, &WeaponG);

	WeaponB.SetFileId("weaponB");
	WeaponB.SetBoundaries(0.f, 255.f);
	WeaponB.SetValue(255.f);
	ColorsGroup.PlaceLabledControl("Weapon Blue", this, &WeaponB);


#pragma endregion
	/**/
#pragma MenuGroup
	////MenuGroup.SetPosition(16, 258);
	////MenuGroup.SetSize(360, 100);
	////MenuGroup.SetText("Bar Color 1");
	////RegisterControl(&MenuGroup);

	//////Menu Gradient Color 1
	////MenuBar1R.SetFileId("bar_grad_1R");
	////MenuBar1R.SetBoundaries(0, 255);
	////MenuBar1R.SetValue(255);
	////MenuGroup.PlaceLabledControl("Red", this, &MenuBar1R);

	////MenuBar1G.SetFileId("bar_grad_1G");
	////MenuBar1G.SetBoundaries(0, 255);
	////MenuBar1G.SetValue(144);
	////MenuGroup.PlaceLabledControl("Green", this, &MenuBar1G);

	////MenuBar1B.SetFileId("bar_grad_1B");
	////MenuBar1B.SetBoundaries(0, 255);
	////MenuBar1B.SetValue(0);
	////MenuGroup.PlaceLabledControl("Blue", this, &MenuBar1B);


	//Menu Gradient Color 2
	//MenuBar2.SetPosition(408, 258);
	//MenuBar2.SetSize(360, 100);
	//MenuBar2.SetText("Bar Color 2");
	//RegisterControl(&MenuBar2);

	//MenuBar2R.SetFileId("bar_grad_2R");
	//MenuBar2R.SetBoundaries(0, 255);
	//MenuBar2R.SetValue(255);
	//MenuBar2.PlaceLabledControl("Red", this, &MenuBar2R);

	//MenuBar2G.SetFileId("bar_grad_2G");
	//MenuBar2G.SetBoundaries(0, 255);
	//MenuBar2G.SetValue(93);
	//MenuBar2.PlaceLabledControl("Green", this, &MenuBar2G);

	//MenuBar2B.SetFileId("bar_grad_2B");
	//MenuBar2B.SetBoundaries(0, 255);
	//MenuBar2B.SetValue(0);
	//MenuBar2.PlaceLabledControl("Blue", this, &MenuBar2B);


	//MenuOpacity.SetFileId("men_oapacity");
	//MenuOpacity.SetBoundaries(0, 255);
	//MenuOpacity.SetValue(0);
	//MenuGroup.PlaceLabledControl("Menu Opacity", this, &MenuOpacity);

	//Inner
	//MenuInside.SetPosition(16, 379);
	//MenuInside.SetSize(360, 100);
	//MenuInside.SetText("Menu Inner Color");
	//RegisterControl(&MenuInside);

	//MenuInnerR.SetFileId("in_R");
	//MenuInnerR.SetBoundaries(0, 255);
	//MenuInnerR.SetValue(255);
	//MenuInside.PlaceLabledControl("Red", this, &MenuInnerR);

	//MenuInnerG.SetFileId("in_G");
	//MenuInnerG.SetBoundaries(0, 255);
	//MenuInnerG.SetValue(93);
	//MenuInside.PlaceLabledControl("Green", this, &MenuInnerG);

	//MenuInnerB.SetFileId("in_B");
	//MenuInnerB.SetBoundaries(0, 255);
	//MenuInnerB.SetValue(0);
	//MenuInside.PlaceLabledControl("Stuff", this, &MenuInnerB);

	////Menu Background

	//MenuBackround.SetPosition(408, 379);
	//MenuBackround.SetSize(360, 135);
	//MenuBackround.SetText("Menu Backround Color");
	//RegisterControl(&MenuBackround);

	//MenuBGR.SetFileId("bg_R");
	//MenuBGR.SetBoundaries(0, 255);
	//MenuBGR.SetValue(30);
	//MenuBackround.PlaceLabledControl("Red", this, &MenuBGR);

	//MenuBGG.SetFileId("bg_G");
	//MenuBGG.SetBoundaries(0, 255);
	//MenuBGG.SetValue(30);
	//MenuBackround.PlaceLabledControl("Green", this, &MenuBGG);

	//MenuBGB.SetFileId("bg_B");
	//MenuBGB.SetBoundaries(0, 255);
	//MenuBGB.SetValue(30);
	//MenuBackround.PlaceLabledControl("Blue", this, &MenuBGB);

	//MenuOpacity.SetFileId("bg_a");
	//MenuOpacity.SetBoundaries(0, 255);
	//MenuOpacity.SetValue(255);
	//MenuBackround.PlaceLabledControl("Blue", this, &MenuOpacity);

#pragma endregion

}
void CSkinchangerTab::Setup()
{
	SetTitle("SKINCHANGER");

	SkinActive.SetPosition(190, -32);
	SkinActive.SetText("Active");
	RegisterControl(&SkinActive);

	SkinEnable.SetFileId("skin_enable");
	SkinEnable.SetPosition(170, -33);
	RegisterControl(&SkinEnable);

	SkinApply.SetText("Apply");
	SkinApply.SetCallback(KnifeApplyCallbk);
	SkinApply.SetPosition(408, 450);
	SkinApply.SetSize(360, 106);
	RegisterControl(&SkinApply);

#pragma region Knife
	KnifeGroup.SetPosition(176, -16);
	KnifeGroup.SetText("Knife");
	KnifeGroup.SetSize(376, 80);
	RegisterControl(&KnifeGroup);

	KnifeModel.SetFileId("knife_model");
	KnifeModel.AddItem("Bayonet");
	KnifeModel.AddItem("Bowie Knife");
	KnifeModel.AddItem("Butterfly Knife");
	KnifeModel.AddItem("Falchion Knife");
	KnifeModel.AddItem("Flip Knife");
	KnifeModel.AddItem("Gut Knife");
	KnifeModel.AddItem("Huntsman Knife");
	KnifeModel.AddItem("Karambit");
	KnifeModel.AddItem("M9 Bayonet");
	KnifeModel.AddItem("Shadow Daggers");
	KnifeGroup.PlaceLabledControl("Knife", this, &KnifeModel);

	KnifeSkin.SetFileId("knife_skin");
	KnifeSkin.AddItem("None");
	KnifeSkin.AddItem("Crimson Web");
	KnifeSkin.AddItem("Bone Mask");
	KnifeSkin.AddItem("Fade");
	KnifeSkin.AddItem("Night");
	KnifeSkin.AddItem("Blue Steel");
	KnifeSkin.AddItem("Stained");
	KnifeSkin.AddItem("Case Hardened");
	KnifeSkin.AddItem("Slaughter");
	KnifeSkin.AddItem("Safari Mesh");
	KnifeSkin.AddItem("Boreal Forest");
	KnifeSkin.AddItem("Ultraviolet");
	KnifeSkin.AddItem("Urban Masked");
	KnifeSkin.AddItem("Scorched");
	KnifeSkin.AddItem("Rust Coat");
	KnifeSkin.AddItem("Tiger Tooth");
	KnifeSkin.AddItem("Damascus Steel");
	KnifeSkin.AddItem("Damascus Steel");
	KnifeSkin.AddItem("Marble Fade");
	KnifeSkin.AddItem("Rust Coat");
	KnifeSkin.AddItem("Doppler Ruby");
	KnifeSkin.AddItem("Doppler Sapphire");
	KnifeSkin.AddItem("Doppler Blackpearl");
	KnifeSkin.AddItem("Doppler Phase 1");
	KnifeSkin.AddItem("Doppler Phase 2");
	KnifeSkin.AddItem("Doppler Phase 3");
	KnifeSkin.AddItem("Doppler Phase 4");
	KnifeSkin.AddItem("Gamma Doppler Phase 1");
	KnifeSkin.AddItem("Gamma Doppler Phase 2");
	KnifeSkin.AddItem("Gamma Doppler Phase 3");
	KnifeSkin.AddItem("Gamma Doppler Phase 4");
	KnifeSkin.AddItem("Gamma Doppler Emerald");
	KnifeSkin.AddItem("Lore");
	KnifeGroup.PlaceLabledControl("Skin", this, &KnifeSkin);
#pragma endregion

#pragma region Machineguns
	MachinegunsGroup.SetPosition(556, -16);
	MachinegunsGroup.SetText("Machineguns");
	MachinegunsGroup.SetSize(360, 80);
	RegisterControl(&MachinegunsGroup);

	NEGEVSkin.SetFileId("negev_skin");
	NEGEVSkin.AddItem("Anodized Navy");
	NEGEVSkin.AddItem("Man-o'-war");
	NEGEVSkin.AddItem("Palm");
	NEGEVSkin.AddItem("VariCamo");
	NEGEVSkin.AddItem("Palm");
	NEGEVSkin.AddItem("CaliCamo");
	NEGEVSkin.AddItem("Terrain");
	NEGEVSkin.AddItem("Army Sheen");
	NEGEVSkin.AddItem("Bratatat");
	NEGEVSkin.AddItem("Desert-Strike");
	NEGEVSkin.AddItem("Nuclear Waste");
	NEGEVSkin.AddItem("Loudmouth");
	NEGEVSkin.AddItem("Power Loader");
	MachinegunsGroup.PlaceLabledControl("Negev", this, &NEGEVSkin);

	M249Skin.SetFileId("m249_skin");
	M249Skin.AddItem("Contrast Spray");
	M249Skin.AddItem("Blizzard Marbleized");
	M249Skin.AddItem("Jungle DDPAT");
	M249Skin.AddItem("Gator Mesh");
	M249Skin.AddItem("Magma");
	M249Skin.AddItem("System Lock");
	M249Skin.AddItem("Shipping Forecast");
	M249Skin.AddItem("Impact Drill");
	M249Skin.AddItem("Nebula Crusader");
	M249Skin.AddItem("Spectre");
	MachinegunsGroup.PlaceLabledControl("M249", this, &M249Skin);

#pragma endregion

#pragma region Snipers
	Snipergroup.SetPosition(176, 90);
	Snipergroup.SetText("Snipers");
	Snipergroup.SetSize(376, 125);
	RegisterControl(&Snipergroup);

	AWPSkin.SetFileId("awp_skin");
	AWPSkin.AddItem("BOOM");
	AWPSkin.AddItem("Dragon Lore");
	AWPSkin.AddItem("Pink DDPAT");
	AWPSkin.AddItem("Snake Camo");
	AWPSkin.AddItem("Lightning Strike");
	AWPSkin.AddItem("Safari Mesh");
	AWPSkin.AddItem("Corticera");
	AWPSkin.AddItem("Redline");
	AWPSkin.AddItem("Man-o'-war");
	AWPSkin.AddItem("Graphite");
	AWPSkin.AddItem("Electric Hive");
	AWPSkin.AddItem("Pit Viper");
	AWPSkin.AddItem("Asiimov");
	AWPSkin.AddItem("Worm God");
	AWPSkin.AddItem("Medusa");
	AWPSkin.AddItem("Sun in Leo");
	AWPSkin.AddItem("Hyper Beast");
	AWPSkin.AddItem("Elite Build");
	Snipergroup.PlaceLabledControl("AWP", this, &AWPSkin);

	SSG08Skin.SetFileId("sgg08_skin");
	SSG08Skin.AddItem("Lichen Dashed");
	SSG08Skin.AddItem("Dark Water");
	SSG08Skin.AddItem("Blue Spruce");
	SSG08Skin.AddItem("Sand Dune");
	SSG08Skin.AddItem("Palm");
	SSG08Skin.AddItem("Mayan Dreams");
	SSG08Skin.AddItem("Blood in the Water");
	SSG08Skin.AddItem("Tropical Storm");
	SSG08Skin.AddItem("Acid Fade");
	SSG08Skin.AddItem("Slashed");
	SSG08Skin.AddItem("Detour");
	SSG08Skin.AddItem("Abyss");
	SSG08Skin.AddItem("Big Iron");
	SSG08Skin.AddItem("Necropos");
	SSG08Skin.AddItem("Ghost Crusader");
	SSG08Skin.AddItem("Dragonfire");
	Snipergroup.PlaceLabledControl("SGG 08", this, &SSG08Skin);

	SCAR20Skin.SetFileId("scar20_skin");
	SCAR20Skin.AddItem("Splash Jam");
	SCAR20Skin.AddItem("Storm");
	SCAR20Skin.AddItem("Contractor");
	SCAR20Skin.AddItem("Carbon Fiber");
	SCAR20Skin.AddItem("Sand Mesh");
	SCAR20Skin.AddItem("Palm");
	SCAR20Skin.AddItem("Emerald");
	SCAR20Skin.AddItem("Crimson Web");
	SCAR20Skin.AddItem("Cardiac");
	SCAR20Skin.AddItem("Army Sheen");
	SCAR20Skin.AddItem("Cyrex");
	SCAR20Skin.AddItem("Grotto");
	SCAR20Skin.AddItem("Emerald");
	SCAR20Skin.AddItem("Green Marine");
	SCAR20Skin.AddItem("Outbreak");
	SCAR20Skin.AddItem("Bloodsport");
	Snipergroup.PlaceLabledControl("SCAR-20", this, &SCAR20Skin);

	G3SG1Skin.SetFileId("g3sg1_skin");
	G3SG1Skin.AddItem("Desert Storm");
	G3SG1Skin.AddItem("Arctic Camo");
	G3SG1Skin.AddItem("Bone Mask");
	G3SG1Skin.AddItem("Contractor");
	G3SG1Skin.AddItem("Safari Mesh");
	G3SG1Skin.AddItem("Polar Camo");
	G3SG1Skin.AddItem("Jungle Dashed");
	G3SG1Skin.AddItem("VariCamo");
	G3SG1Skin.AddItem("Predator");
	G3SG1Skin.AddItem("Demeter");
	G3SG1Skin.AddItem("Azure Zebra");
	G3SG1Skin.AddItem("Green Apple");
	G3SG1Skin.AddItem("Orange Kimono");
	G3SG1Skin.AddItem("Neon Kimono");
	G3SG1Skin.AddItem("Murky");
	G3SG1Skin.AddItem("Chronos");
	G3SG1Skin.AddItem("Flux");
	G3SG1Skin.AddItem("The Executioner");
	G3SG1Skin.AddItem("Orange Crash");
	Snipergroup.PlaceLabledControl("G3SG1", this, &G3SG1Skin);
#pragma endregion
#pragma endregion

#pragma region Rifles
	Riflegroup.SetPosition(176, 225);
	Riflegroup.SetText("Rifles");
	Riflegroup.SetSize(376, 195);
	RegisterControl(&Riflegroup);

	AK47Skin.SetFileId("ak47_skin");
	AK47Skin.AddItem("First Class");
	AK47Skin.AddItem("Red Laminate");
	AK47Skin.AddItem("Case Hardened");
	AK47Skin.AddItem("Black Laminate");
	AK47Skin.AddItem("Fire Serpent");
	AK47Skin.AddItem("Cartel");
	AK47Skin.AddItem("Emerald Pinstripe");
	AK47Skin.AddItem("Blue Laminate");
	AK47Skin.AddItem("Redline");
	AK47Skin.AddItem("Vulcan");
	AK47Skin.AddItem("Jaguar");
	AK47Skin.AddItem("Jet Set");
	AK47Skin.AddItem("Wasteland Rebel");
	AK47Skin.AddItem("Elite Build");
	AK47Skin.AddItem("Hydroponic");
	AK47Skin.AddItem("Aquamarine Revenge");
	AK47Skin.AddItem("Frontside Misty");
	AK47Skin.AddItem("Point Disarray");
	AK47Skin.AddItem("Fuel Injector");
	AK47Skin.AddItem("Neon Revolution");
	Riflegroup.PlaceLabledControl("AK-47", this, &AK47Skin);

	M41SSkin.SetFileId("m4a1s_skin");
	M41SSkin.AddItem("Dark Water");
	M41SSkin.AddItem("Hyper Beast");
	M41SSkin.AddItem("Boreal Forest");
	M41SSkin.AddItem("VariCamo");
	M41SSkin.AddItem("Nitro");
	M41SSkin.AddItem("Bright Water");
	M41SSkin.AddItem("Atomic Alloy");
	M41SSkin.AddItem("Blood Tiger");
	M41SSkin.AddItem("Guardian");
	M41SSkin.AddItem("Master Piece");
	M41SSkin.AddItem("Knight");
	M41SSkin.AddItem("Cyrex");
	M41SSkin.AddItem("Basilisk");
	M41SSkin.AddItem("Icarus Fell");
	M41SSkin.AddItem("Hot Rod");
	M41SSkin.AddItem("Golden Coi");
	M41SSkin.AddItem("Chantico's Fire");
	M41SSkin.AddItem("Mecha Industries");
	M41SSkin.AddItem("Flashback");
	Riflegroup.PlaceLabledControl("M4A1-S", this, &M41SSkin);

	M4A4Skin.SetFileId("m4a4_skin");
	M4A4Skin.AddItem("Bullet Rain");
	M4A4Skin.AddItem("Zirka");
	M4A4Skin.AddItem("Asiimov");
	M4A4Skin.AddItem("Howl");
	M4A4Skin.AddItem("X-Ray");
	M4A4Skin.AddItem("Desert-Strike");
	M4A4Skin.AddItem("Griffin");
	M4A4Skin.AddItem("Dragon King");
	M4A4Skin.AddItem("Poseidon");
	M4A4Skin.AddItem("Daybreak");
	M4A4Skin.AddItem("Evil Daimyo");
	M4A4Skin.AddItem("Royal Paladin");
	M4A4Skin.AddItem("The BattleStar");
	M4A4Skin.AddItem("Desolate Space");
	M4A4Skin.AddItem("Buzz Kill");
	Riflegroup.PlaceLabledControl("M4A4", this, &M4A4Skin);

	AUGSkin.SetFileId("aug_skin");
	AUGSkin.AddItem("Bengal Tiger");
	AUGSkin.AddItem("Hot Rod");
	AUGSkin.AddItem("Chameleon");
	AUGSkin.AddItem("Torque");
	AUGSkin.AddItem("Radiation Hazard");
	AUGSkin.AddItem("Asterion");
	AUGSkin.AddItem("Daedalus");
	AUGSkin.AddItem("Akihabara Accept");
	AUGSkin.AddItem("Ricochet");
	AUGSkin.AddItem("Fleet Flock");
	AUGSkin.AddItem("Syd Mead");
	Riflegroup.PlaceLabledControl("AUG", this, &AUGSkin);

	FAMASSkin.SetFileId("famas_skin");
	FAMASSkin.AddItem("Contrast Spray");
	FAMASSkin.AddItem("Colony");
	FAMASSkin.AddItem("Cyanospatter");
	FAMASSkin.AddItem("Djinn");
	FAMASSkin.AddItem("Afterimage");
	FAMASSkin.AddItem("Doomkitty");
	FAMASSkin.AddItem("Spitfire");
	FAMASSkin.AddItem("Teardown");
	FAMASSkin.AddItem("Hexane");
	FAMASSkin.AddItem("Pulse");
	FAMASSkin.AddItem("Sergeant");
	FAMASSkin.AddItem("Styx");
	FAMASSkin.AddItem("Neural Net");
	FAMASSkin.AddItem("Survivor");
	FAMASSkin.AddItem("Valence");
	FAMASSkin.AddItem("Roll Cage");
	FAMASSkin.AddItem("Mecha Industries");
	Riflegroup.PlaceLabledControl("FAMAS", this, &FAMASSkin);

	GALILSkin.SetFileId("galil_skin");
	GALILSkin.AddItem("Forest DDPAT");
	GALILSkin.AddItem("Contrast Spray");
	GALILSkin.AddItem("Orange DDPAT");
	GALILSkin.AddItem("Eco");
	GALILSkin.AddItem("Winter Forest");
	GALILSkin.AddItem("Sage Spray");
	GALILSkin.AddItem("VariCamo");
	GALILSkin.AddItem("VariCamo");
	GALILSkin.AddItem("Chatterbox");
	GALILSkin.AddItem("Shattered");
	GALILSkin.AddItem("Kami");
	GALILSkin.AddItem("Blue Titanium");
	GALILSkin.AddItem("Urban Rubble");
	GALILSkin.AddItem("Hunting Blind");
	GALILSkin.AddItem("Sandstorm");
	GALILSkin.AddItem("Tuxedo");
	GALILSkin.AddItem("Cerberus");
	GALILSkin.AddItem("Aqua Terrace");
	GALILSkin.AddItem("Rocket Pop");
	GALILSkin.AddItem("Stone Cold");
	GALILSkin.AddItem("Firefight");
	Riflegroup.PlaceLabledControl("GALIL", this, &GALILSkin);

	SG553Skin.SetFileId("sg552_skin");
	SG553Skin.AddItem("Bulldozer");
	SG553Skin.AddItem("Ultraviolet");
	SG553Skin.AddItem("Damascus Steel");
	SG553Skin.AddItem("Fallout Warning");
	SG553Skin.AddItem("Damascus Steel");
	SG553Skin.AddItem("Pulse");
	SG553Skin.AddItem("Army Sheen");
	SG553Skin.AddItem("Traveler");
	SG553Skin.AddItem("Fallout Warning");
	SG553Skin.AddItem("Cyrex");
	SG553Skin.AddItem("Tiger Moth");
	SG553Skin.AddItem("Atlas");
	Riflegroup.PlaceLabledControl("SG552", this, &SG553Skin);
#pragma endregion



#pragma endregion

#pragma region Pistols
	PistolGroup.SetPosition(556, 220);
	PistolGroup.SetText("Pistols");
	PistolGroup.SetSize(360, 215);
	RegisterControl(&PistolGroup);

	GLOCKSkin.SetFileId("glock_skin");
	GLOCKSkin.AddItem("Groundwater");
	GLOCKSkin.AddItem("Candy Apple");
	GLOCKSkin.AddItem("Fade");
	GLOCKSkin.AddItem("Night");
	GLOCKSkin.AddItem("Dragon Tattoo");
	GLOCKSkin.AddItem("Twilight Galaxy");
	GLOCKSkin.AddItem("Sand Dune");
	GLOCKSkin.AddItem("Brass");
	GLOCKSkin.AddItem("Catacombs");
	GLOCKSkin.AddItem("Sand Dune");
	GLOCKSkin.AddItem("Steel Disruption");
	GLOCKSkin.AddItem("Blue Fissure");
	GLOCKSkin.AddItem("Death Rattle");
	GLOCKSkin.AddItem("Water Elemental");
	GLOCKSkin.AddItem("Reactor");
	GLOCKSkin.AddItem("Grinder");
	GLOCKSkin.AddItem("Bunsen Burner");
	GLOCKSkin.AddItem("Wraith");
	GLOCKSkin.AddItem("Royal Legion");
	GLOCKSkin.AddItem("Weasel");
	GLOCKSkin.AddItem("Wasteland Rebel");
	PistolGroup.PlaceLabledControl("Glock", this, &GLOCKSkin);

	USPSSkin.SetFileId("usps_skin");
	USPSSkin.AddItem("Forest Leaves");
	USPSSkin.AddItem("Dark Water");
	USPSSkin.AddItem("VariCamo");
	USPSSkin.AddItem("Overgrowth");
	USPSSkin.AddItem("Caiman");
	USPSSkin.AddItem("Blood Tiger");
	USPSSkin.AddItem("Serum");
	USPSSkin.AddItem("Night Ops");
	USPSSkin.AddItem("Stainless");
	USPSSkin.AddItem("Guardian");
	USPSSkin.AddItem("Orion");
	USPSSkin.AddItem("Road Rash");
	USPSSkin.AddItem("Royal Blue");
	USPSSkin.AddItem("Business Class");
	USPSSkin.AddItem("Para Green");
	USPSSkin.AddItem("Torque");
	USPSSkin.AddItem("Kill Confirmed");
	USPSSkin.AddItem("Lead Conduit");
	USPSSkin.AddItem("Cyrex");
	PistolGroup.PlaceLabledControl("USP-S", this, &USPSSkin);

	DEAGLESkin.SetFileId("deagle_skin");
	DEAGLESkin.AddItem("Blaze");
	DEAGLESkin.AddItem("Pilot");
	DEAGLESkin.AddItem("Midnight Storm");
	DEAGLESkin.AddItem("Sunset Storm");
	DEAGLESkin.AddItem("Forest DDPAT");
	DEAGLESkin.AddItem("Crimson Web");
	DEAGLESkin.AddItem("Urban DDPAT");
	DEAGLESkin.AddItem("Night");
	DEAGLESkin.AddItem("Hypnotic");
	DEAGLESkin.AddItem("Mudder");
	DEAGLESkin.AddItem("VariCamo");
	DEAGLESkin.AddItem("Golden Koi");
	DEAGLESkin.AddItem("Cobalt Disruption");
	DEAGLESkin.AddItem("Urban Rubble");
	DEAGLESkin.AddItem("Naga");
	DEAGLESkin.AddItem("Hand Cannon");
	DEAGLESkin.AddItem("Heirloom");
	DEAGLESkin.AddItem("Meteorite");
	DEAGLESkin.AddItem("Conspiracy");
	DEAGLESkin.AddItem("Bronze Deco");
	DEAGLESkin.AddItem("Sunset Storm");
	DEAGLESkin.AddItem("Corinthian");
	DEAGLESkin.AddItem("Kumicho Dragon");
	PistolGroup.PlaceLabledControl("Deagle", this, &DEAGLESkin);

	DUALSSkin.SetFileId("duals_skin");
	DUALSSkin.AddItem("Anodized Navy");
	DUALSSkin.AddItem("Ossified");
	DUALSSkin.AddItem("Stained");
	DUALSSkin.AddItem("Contractor");
	DUALSSkin.AddItem("Colony");
	DUALSSkin.AddItem("Demolition");
	DUALSSkin.AddItem("Black Limba");
	DUALSSkin.AddItem("Red Quartz");
	DUALSSkin.AddItem("Cobalt Quartz");
	DUALSSkin.AddItem("Hemoglobin");
	DUALSSkin.AddItem("Urban Shock");
	DUALSSkin.AddItem("Marina");
	DUALSSkin.AddItem("Panther");
	DUALSSkin.AddItem("Retribution");
	DUALSSkin.AddItem("Briar");
	DUALSSkin.AddItem("Duelist");
	DUALSSkin.AddItem("Moon in Libra");
	DUALSSkin.AddItem("Dualing Dragons");
	DUALSSkin.AddItem("Cartel");
	DUALSSkin.AddItem("Ventilators");
	PistolGroup.PlaceLabledControl("Duals", this, &DUALSSkin);

	FIVESEVENSkin.SetFileId("fiveseven_skin");
	FIVESEVENSkin.AddItem("Candy Apple");
	FIVESEVENSkin.AddItem("Bone Mask");
	FIVESEVENSkin.AddItem("Case Hardened");
	FIVESEVENSkin.AddItem("Contractor");
	FIVESEVENSkin.AddItem("Forest Night");
	FIVESEVENSkin.AddItem("Orange Peel");
	FIVESEVENSkin.AddItem("Jungle");
	FIVESEVENSkin.AddItem("Nitro");
	FIVESEVENSkin.AddItem("Red Quartz");
	FIVESEVENSkin.AddItem("Anodized Gunmetal");
	FIVESEVENSkin.AddItem("Nightshade");
	FIVESEVENSkin.AddItem("Silver Quartz");
	FIVESEVENSkin.AddItem("Kami");
	FIVESEVENSkin.AddItem("Copper Galaxy");
	FIVESEVENSkin.AddItem("Neon Kimono");
	FIVESEVENSkin.AddItem("Fowl Play");
	FIVESEVENSkin.AddItem("Hot Shot");
	FIVESEVENSkin.AddItem("Urban Hazard");
	FIVESEVENSkin.AddItem("Monkey Business");
	FIVESEVENSkin.AddItem("Retrobution");
	FIVESEVENSkin.AddItem("Triumvirate");
	PistolGroup.PlaceLabledControl("Five-Seven", this, &FIVESEVENSkin);

	TECNINESkin.SetFileId("tec9_skin");
	TECNINESkin.AddItem("Tornado");
	TECNINESkin.AddItem("Groundwater");
	TECNINESkin.AddItem("Forest DDPAT");
	TECNINESkin.AddItem("Terrace");
	TECNINESkin.AddItem("Urban DDPAT");
	TECNINESkin.AddItem("Ossified");
	TECNINESkin.AddItem("Hades");
	TECNINESkin.AddItem("Brass");
	TECNINESkin.AddItem("VariCamo");
	TECNINESkin.AddItem("Nuclear Threat");
	TECNINESkin.AddItem("Red Quartz");
	TECNINESkin.AddItem("Tornado");
	TECNINESkin.AddItem("Blue Titanium");
	TECNINESkin.AddItem("Army Mesh");
	TECNINESkin.AddItem("Titanium Bit");
	TECNINESkin.AddItem("Sandstorm");
	TECNINESkin.AddItem("Isaac");
	TECNINESkin.AddItem("Toxic");
	TECNINESkin.AddItem("Bamboo Forest");
	TECNINESkin.AddItem("Avalanche");
	TECNINESkin.AddItem("Jambiya");
	TECNINESkin.AddItem("Re-Entry");
	TECNINESkin.AddItem("Fuel Injector");
	PistolGroup.PlaceLabledControl("Tec-9", this, &TECNINESkin);

	P2000Skin.SetFileId("p2000_skin");
	P2000Skin.AddItem("Grassland Leaves");
	P2000Skin.AddItem("Silver");
	P2000Skin.AddItem("Granite Marbleized");
	P2000Skin.AddItem("Forest Leaves");
	P2000Skin.AddItem("Ossified");
	P2000Skin.AddItem("Handgun");
	P2000Skin.AddItem("Fade");
	P2000Skin.AddItem("Scorpion");
	P2000Skin.AddItem("Grassland");
	P2000Skin.AddItem("Corticera");
	P2000Skin.AddItem("Ocean Foam");
	P2000Skin.AddItem("Pulse");
	P2000Skin.AddItem("Amber Fade");
	P2000Skin.AddItem("Red FragCam");
	P2000Skin.AddItem("Chainmail");
	P2000Skin.AddItem("Coach Class");
	P2000Skin.AddItem("Ivory");
	P2000Skin.AddItem("Fire Elemental");
	P2000Skin.AddItem("Asterion");
	P2000Skin.AddItem("Pathfinder");
	P2000Skin.AddItem("Imperial");
	P2000Skin.AddItem("Oceanic");
	P2000Skin.AddItem("Imperial Dragon");
	PistolGroup.PlaceLabledControl("P2000", this, &P2000Skin);

	P250Skin.SetFileId("p250_skin");
	P250Skin.AddItem("Whiteout");
	P250Skin.AddItem("Metallic DDPAT");
	P250Skin.AddItem("Splash");
	P250Skin.AddItem("Gunsmoke");
	P250Skin.AddItem("Modern Hunter");
	P250Skin.AddItem("Bone Mask");
	P250Skin.AddItem("Boreal Forest");
	P250Skin.AddItem("Sand Dune");
	P250Skin.AddItem("Nuclear Threat");
	P250Skin.AddItem("Mehndi");
	P250Skin.AddItem("Facets");
	P250Skin.AddItem("Hive");
	P250Skin.AddItem("Muertos");
	P250Skin.AddItem("Steel Disruption");
	P250Skin.AddItem("Undertow");
	P250Skin.AddItem("Franklin");
	P250Skin.AddItem("Neon Kimono");
	P250Skin.AddItem("Supernova");
	P250Skin.AddItem("Contamination");
	P250Skin.AddItem("Cartel");
	P250Skin.AddItem("Valence");
	P250Skin.AddItem("Crimson Kimono");
	P250Skin.AddItem("Mint Kimono");
	P250Skin.AddItem("Wing Shot");
	P250Skin.AddItem("Asiimov");
	PistolGroup.PlaceLabledControl("P250", this, &P250Skin);

#pragma endregion


#pragma endregion other random options
}

void Menu::SetupMenu()
{
	Window.Setup();

	GUI.RegisterWindow(&Window);
	GUI.BindWindow(VK_INSERT, &Window);
}

void Menu::DoUIFrame()
{
	// General Processing

	// If the "all filter is selected tick all the others
	if (Window.VisualsTab.FiltersAll.GetState())
	{
		Window.VisualsTab.FiltersC4.SetState(true);
		Window.VisualsTab.FiltersChickens.SetState(true);
		Window.VisualsTab.FiltersPlayers.SetState(true);
		Window.VisualsTab.FiltersWeapons.SetState(true);
	}

	if (Window.MiscTab.OtherSkinChanger.GetState())
	{
		//		RegisterTab(&ApocalypseWindow::Setup.SkinchangerTab);
	}


	GUI.Update();
	GUI.Draw();


}


