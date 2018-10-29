#include "Main.h"

CreateMoveFn oCreateMove;
bool __stdcall Hooks::CreateMove(float flInputSampleTime, CUserCmd* pCmd)
{
	if (!I::Engine->IsInGame() || !I::Engine->IsConnected() || !g_Globals->LocalPlayer || !g_Globals->LocalPlayer->IsAlive())
		return oCreateMove(flInputSampleTime, pCmd);

	oCreateMove(flInputSampleTime, pCmd);
	if (!pCmd->command_number)
		return true;

	bool bSendPacket = true;
	g_Globals->OriginalAng = pCmd->viewangles;

	g_MiscFunc->Spoof();

	g_AntiAim->FakeLag(&bSendPacket);

	g_MiscHacks->Move(pCmd);

	g_MiscFunc->EnginePrediction(pCmd);

	g_AntiAim->Move(pCmd, &bSendPacket);

	g_Aimbot->Aim(pCmd, &bSendPacket);

	g_Math->Clamp(pCmd->viewangles);

	g_MiscFunc->MovementFix(g_Globals->OriginalAng, pCmd);

	g_Globals->Shot = (pCmd->buttons & IN_ATTACK);

	if (!bSendPacket)
		g_Globals->RealAngle = pCmd->viewangles;

	uintptr_t* yeet;
	__asm mov yeet, ebp;
	*(bool*)(*yeet - 0x1C) = bSendPacket;

	return false;
}
