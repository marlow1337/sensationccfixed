/*
Rest In Peace ApocalypseCheats
*/

#pragma once

#include "Hacks.h"

class CLegitBot : public CHack
{
public:
	void Init();
	void Draw();
	void Move(CUserCmd *pCmd, bool &bSendPacket);
private:
	// Targetting
	int GetTargetCrosshair();
	bool TargetMeetsRequirements(IClientEntity* pEntity);
	float FovToPlayer(Vector View, IClientEntity* pEntity, int HitBox, Vector ViewOffSet);
	int HitScan(IClientEntity* pEntity);
	bool AimAtPoint(Vector point, CUserCmd *pCmd, bool &bSendPacket, IClientEntity* pLocal);

	void SyncWeaponSettings();

	// Functionality
	void DoAimbot(bool &bSendpacket, CUserCmd *pCmd);
	void DoTrigger(CUserCmd *pCmd);
	void DoRecoilControl(CUserCmd *pCmd);

	// Aimbot
	bool IsLocked;
	int TargetID;
	int HitBox;
	int HitBox2;
	Vector AimPoint;

	float Speed;
	float FoV;
	bool RecoilControl;
	bool PSilent;
	float Inacc;
};