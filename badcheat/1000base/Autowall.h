#pragma once

struct FireBulletData
{
	FireBulletData(const Vector &eye_pos)
		: src(eye_pos)
	{
	}

	Vector           src;
	trace_t          enter_trace;
	Vector           direction;
	CTraceFilter    filter;
	float           trace_length;
	float           trace_length_remaining;
	float           current_damage;
	int             penetrate_count;
};



class Autowall {
public:

	float CanHit(Vector &vecEyePos, Vector &point);
	bool FireBullet(C_BaseCombatWeapon* pWeapon, Vector& direction, float& currentDamage);
	bool HandleBulletPenetration(CSWeaponInfo* weaponData, trace_t& enterTrace, Vector& eyePosition, Vector direction, int& possibleHitsRemaining, float& currentDamage, float penetrationPower, bool sv_penetration_type, float ff_damage_reduction_bullets, float ff_damage_bullet_penetration);
	bool TraceToExit(trace_t& enterTrace, trace_t& exitTrace, Vector startPosition, Vector direction);
	void ScaleDamage(trace_t &enterTrace, CSWeaponInfo *weaponData, float& currentDamage);
	bool IsBreakableEntity(C_BaseEntity* entity);
	void GetBulletTypeParameters(float& maxRange, float& maxDistance, char* bulletType, bool sv_penetration_type);
	void ClipTraceToPlayers(Vector& absStart, Vector absEnd, unsigned int mask, CTraceFilter* filter, trace_t* tr);

	bool CanHitFloatingPoint(const Vector &point, const Vector &source, float *damage_given);

	bool DidHitNonWorldEntity(C_BaseEntity * ent);
	bool DidHitWorld(C_BaseEntity * ent);

}; extern Autowall* g_Autowall;
