#include "Main.h"

void MiscHacks::Move(CUserCmd* pCmd)
{
	if (g_Vars->Misc.AutoHop)
		AutoHop(pCmd);

	if (g_Vars->Misc.AutoStrafe)
		AutoStrafe(pCmd);
}

void MiscHacks::AutoHop(CUserCmd* pCmd)
{
	if (pCmd->buttons & IN_JUMP)
	{
		if (!(g_Globals->LocalPlayer->GetFlags() & FL_ONGROUND))
			pCmd->buttons &= ~IN_JUMP;
	}
}

void MiscHacks::AutoStrafe(CUserCmd* pCmd)
{
	if (!(g_Globals->LocalPlayer->GetFlags() & FL_ONGROUND) && GetAsyncKeyState(VK_SPACE))
	{
		pCmd->forwardmove = (10000.f / g_Globals->LocalPlayer->GetVelocity().Length2D() > 450.f) ? 450.f : 10000.f / g_Globals->LocalPlayer->GetVelocity().Length2D();
		pCmd->sidemove = (pCmd->mousedx != 0) ? (pCmd->mousedx < 0.0f) ? -450.f : 450.f : (pCmd->command_number % 2) == 0 ? -450.f : 450.f;
	}
}