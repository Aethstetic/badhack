#pragma once

#define TIME_TO_TICKS( dt )		( (int)( 0.5 + (float)(dt) / I::Globals->interval_per_tick ) )
#define TICKS_TO_TIME( t )		( I::Globals->interval_per_tick *( t ) )

class MiscFunc {
public:

	void MovementFix(Vector& OriginalAng, CUserCmd* pCmd);
	void AnimFix(ClientFrameStage_t curStage);
	float FireRate(C_BaseCombatWeapon* pWeapon);
	bool HitChance(C_BaseEntity* Entity, C_BaseCombatWeapon* pWeapon, Vector Angle, int chance);
	void EnginePrediction(CUserCmd* pCmd);
	void EndPrediction(CUserCmd* pCmd);
	int GetLerpTicks();
	void ThirdPerson(ClientFrameStage_t curStage);
	void Spoof();
	bool VectortoVectorVisible(Vector src, Vector point);
	void LoadNamedSky(const char *sky_name);

	IMaterial* CreateMaterial();

	void TraceLine(Vector& vecAbsStart, Vector& vecAbsEnd, unsigned int mask, C_BaseEntity *ignore, trace_t *ptr);
	bool TraceToExit(Vector& vecEnd, trace_t* pEnterTrace, Vector vecStart, Vector vecDir, trace_t* pExitTrace);

	float curtime, frametime;
}; extern MiscFunc* g_MiscFunc;