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

#pragma once

#include "Hacks.h"

class CEsp : public CHack
{
public:
	void Init();
	void DrawGranades();
	void Draw();
	void Move(CUserCmd *pCmd, bool &bSendPacket);
	std::vector<IMaterial*> SkyTextures;
	std::vector<IMaterial*> WorldTextures;
private:

	// Other shit
	IClientEntity *BombCarrier;

	struct ESPBox
	{
		int x, y, w, h;
	};

	// Draw a player
	void NoSky();
	void DoFOV();
	void DrawPlayer(player_info_t pinfo, IClientEntity* pEntity);

	// Get player info
	Color GetPlayerColor(IClientEntity* pEntity);
	bool GetBox(ESPBox &result, IClientEntity* pEntity);
	

	// Draw shit about player
	void SpecList();
	void DrawGlow(int r, int g, int b, int a, IClientEntity *pEntity);
	void DrawBox(Color color, ESPBox size);
	void DrawName(ESPBox size, IClientEntity* pEntity, player_info_t pinfo);
	void DrawHealth(ESPBox size, IClientEntity* pEntity);
	void DrawArmor(IClientEntity* pEntity, CEsp::ESPBox size);
	void DrawDistanse(CEsp::ESPBox size, IClientEntity* pEntity);
	float DistanceTo(Vector vecSrc, Vector vecDst);
	void DrawHealth2(ESPBox size, IClientEntity* pEntity);
	void DrawInfo(IClientEntity* pEntity, ESPBox size);
	void DrawGrenades(IClientEntity * pEntity);
	void DrawCross(IClientEntity* pEntity);
	void DrawSkeleton(IClientEntity* pEntity);

	void DrawChicken(IClientEntity* pEntity, ClientClass* cClass);
	void DrawDrop(IClientEntity* pEntity, ClientClass* cClass);
	void DrawBombPlanted(ClientClass* cClass, IClientEntity* pEntity);
	void DrawBomb(IClientEntity* pEntity, ClientClass* cClass);
};

