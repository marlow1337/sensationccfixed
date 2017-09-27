#pragma once

#include "Interfaces.h"
#include "Hacks.h"

namespace R
{
	void Resolve();
	void Other(CUserCmd * pCmd, bool & bSendPacket, unsigned int vguiPanel, int viewangles, unsigned __int8 swap);

};

namespace Globals
{
	extern CUserCmd* UserCmd;
	extern IClientEntity* Target;
	extern int Shots;
	extern bool change;
	extern int TargetID;
}