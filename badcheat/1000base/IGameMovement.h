#pragma once

class IGameMovement
{
public:
	void ProcessMovement(C_BaseEntity *pPlayer, void *pMove)
	{
		typedef void(__thiscall* ProcessMovementFn)(void*, C_BaseEntity*, void*);
		return g_Utils->get_vfunc<ProcessMovementFn>(this, 1)(this, pPlayer, pMove);
	}

	void StartTrackPredictionErrors(C_BaseEntity *pPlayer)
	{
		typedef void(__thiscall* StartTrack)(void*, C_BaseEntity*);
		return g_Utils->get_vfunc<StartTrack>(this, 3)(this, pPlayer);
	}

	void FinishTrackPredictionErrors(C_BaseEntity *pPlayer)
	{
		typedef void(__thiscall* ProcessMovementFn)(void*, C_BaseEntity*);
		return g_Utils->get_vfunc<ProcessMovementFn>(this, 4)(this, pPlayer);
	}
};