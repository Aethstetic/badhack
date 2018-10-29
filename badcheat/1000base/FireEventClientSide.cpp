#include "Main.h"

FireEventClientSideFn oFireEventClientSide;
bool __fastcall Hooks::FireEventClientSide(void *ecx, void* edx, IGameEvent* pEvent) //shitty, unreliable hook
{
	if (!pEvent)
		return false;

	if (I::Engine->IsInGame())
	{
		if (g_Globals->LocalPlayer && g_Globals->LocalPlayer->IsAlive())
		{
			if (!strcmp(pEvent->GetName(), "player_hurt"))
			{
				if (I::Engine->GetPlayerForUserID(pEvent->GetInt("attacker", 0)) == I::Engine->GetLocalPlayer())
				{	

				}
			}
		}
	}

	return oFireEventClientSide(ecx, pEvent);
}