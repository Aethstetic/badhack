#pragma once
#include "Backtracking.h"

class Aimbot {
public:
	void Aim(CUserCmd* pCmd, bool *bSendPacket);
	void StopMovement(CUserCmd* cmd);
	bool CanFireWeapon(CUserCmd * cmd);

private:
	bool Hitscan(C_BaseEntity* Entity, C_BaseCombatWeapon* pWeapon, Vector *Aimpoint, float *LastDmg);
	bool FindBestRecord(C_BaseEntity *player, LagData* record);

	matrix3x4_t matrix[128];
	matrix3x4_t matrix2[128];

	int best_tick = 0;
	bool hitchanced = false;
	Vector Angle;
	LagData restore_record;
	ConVar* RecoilScale;

}; extern Aimbot* g_Aimbot;