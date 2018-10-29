#include "Main.h"

////////////////////////////////////// Traces //////////////////////////////////////

const Vector &world_space_center(C_BaseEntity* player)
{
	Vector vecOrigin = player->GetOrigin();
	auto collide = (ICollideable*)player->GetCollideable();

	Vector min = collide->OBBMins() + vecOrigin;
	Vector max = collide->OBBMaxs() + vecOrigin;

	Vector size = max - min;
	size /= 2.f;
	size += min;

	return size;
}

void Autowall::ClipTraceToPlayers(Vector& absStart, Vector absEnd, unsigned int mask, CTraceFilter* filter, trace_t* tr)
{
	trace_t playerTrace;
	Ray_t ray;
	float smallestFraction = tr->fraction;
	const float maxRange = 60.0f;

	ray.Init(absStart, absEnd);

	for (int i = 1; i <= I::Globals->maxClients; i++)
	{
		C_BaseEntity *player = I::EntityList->GetClientEntity(i);

		if (!player || !player->IsAlive() || player->IsDormant())
			continue;

		if (filter && filter->ShouldHitEntity(player, mask) == false)
			continue;

		float range = g_Math->DistanceToRay(world_space_center(player), absStart, absEnd);
		if (range < 0.0f || range > maxRange)
			continue;

		I::Trace->ClipRayToEntity(ray, mask | CONTENTS_HITBOX, player, &playerTrace);
		if (playerTrace.fraction < smallestFraction)
		{
			*tr = playerTrace;
			smallestFraction = playerTrace.fraction;
		}
	}
}

////////////////////////////////////// Legacy Functions //////////////////////////////////////
void Autowall::GetBulletTypeParameters(float& maxRange, float& maxDistance, char* bulletType, bool sv_penetration_type)
{
	if (sv_penetration_type)
	{
		maxRange = 35.0;
		maxDistance = 3000.0;
	}
	else
	{
		//Play tribune to framerate. Thanks, stringcompare
		//Regardless I doubt anyone will use the old penetration system anyway; so it won't matter much.
		if (!strcmp(bulletType, ("BULLET_PLAYER_338MAG")))
		{
			maxRange = 45.0;
			maxDistance = 8000.0;
		}
		if (!strcmp(bulletType, ("BULLET_PLAYER_762MM")))
		{
			maxRange = 39.0;
			maxDistance = 5000.0;
		}
		if (!strcmp(bulletType, ("BULLET_PLAYER_556MM")) || !strcmp(bulletType, ("BULLET_PLAYER_556MM_SMALL")) || !strcmp(bulletType, ("BULLET_PLAYER_556MM_BOX")))
		{
			maxRange = 35.0;
			maxDistance = 4000.0;
		}
		if (!strcmp(bulletType, ("BULLET_PLAYER_57MM")))
		{
			maxRange = 30.0;
			maxDistance = 2000.0;
		}
		if (!strcmp(bulletType, ("BULLET_PLAYER_50AE")))
		{
			maxRange = 30.0;
			maxDistance = 1000.0;
		}
		if (!strcmp(bulletType, ("BULLET_PLAYER_357SIG")) || !strcmp(bulletType, ("BULLET_PLAYER_357SIG_SMALL")) || !strcmp(bulletType, ("BULLET_PLAYER_357SIG_P250")) || !strcmp(bulletType, ("BULLET_PLAYER_357SIG_MIN")))
		{
			maxRange = 25.0;
			maxDistance = 800.0;
		}
		if (!strcmp(bulletType, ("BULLET_PLAYER_9MM")))
		{
			maxRange = 21.0;
			maxDistance = 800.0;
		}
		if (!strcmp(bulletType, ("BULLET_PLAYER_45ACP")))
		{
			maxRange = 15.0;
			maxDistance = 500.0;
		}
		if (!strcmp(bulletType, ("BULLET_PLAYER_BUCKSHOT")))
		{
			maxRange = 0.0;
			maxDistance = 0.0;
		}
	}
}

////////////////////////////////////// Misc Functions //////////////////////////////////////
bool Autowall::IsBreakableEntity(C_BaseEntity* entity)
{
	typedef bool(__thiscall *isBreakbaleEntityFn)(C_BaseEntity*);
	static auto is_breakable_entityFn = reinterpret_cast<isBreakbaleEntityFn>(g_Utils->FindPatternIDA("client_panorama.dll", "55 8B EC 83 E4 F8 83 EC 70 6A 58")); // mutiny

	if (is_breakable_entityFn)
	{
		// 0x27C = m_takedamage

		auto backupval = *reinterpret_cast<int*>((uint32_t)entity + 0x27C);
		auto className = entity->GetClientClass()->NetworkName;

		if (entity != I::EntityList->GetClientEntity(0))
		{
			// CFuncBrush:
			// (className[1] != 'F' || className[4] != 'c' || className[5] != 'B' || className[9] != 'h')
			if ((className[1] == 'B' && className[9] == 'e' && className[10] == 'S' && className[16] == 'e') // CBreakableSurface
				|| (className[1] != 'B' || className[5] != 'D')) // CBaseDoor because fuck doors
			{
				*reinterpret_cast<int*>((uint32_t)entity + 0x27C) = 2;
			}
		}

		bool retn = is_breakable_entityFn(entity);

		*reinterpret_cast<int*>((uint32_t)entity + 0x27C) = backupval;

		return retn;
	}
	else
		return false;
}

void Autowall::ScaleDamage(trace_t &enterTrace, CSWeaponInfo *weaponData, float& currentDamage)
{
	//Cred. to N0xius for reversing this.
	//TODO: _xAE^; look into reversing this yourself sometime

	bool hasHeavyArmor = ((C_BaseEntity*)enterTrace.hit_entity)->GetHeavyArmor();
	int armorValue = ((C_BaseEntity*)enterTrace.hit_entity)->ArmorValue();

	//Fuck making a new function, lambda beste. ~ Does the person have armor on for the hitbox checked?
	auto IsArmored = [&enterTrace]()->bool
	{
		C_BaseEntity* targetEntity = (C_BaseEntity*)enterTrace.hit_entity;
		switch (enterTrace.hitgroup)
		{
		case HITGROUP_HEAD:
			return !!(C_BaseEntity*)targetEntity->HasHelmet(); //Fuck compiler errors - force-convert it to a bool via (!!)
		case HITGROUP_GENERIC:
		case HITGROUP_CHEST:
		case HITGROUP_STOMACH:
		case HITGROUP_LEFTARM:
		case HITGROUP_RIGHTARM:
			return true;
		default:
			return false;
		}
	};

	switch (enterTrace.hitgroup)
	{
	case HITGROUP_HEAD:
		currentDamage *= hasHeavyArmor ? 2.f : 4.f; //Heavy Armor does 1/2 damage
		break;
	case HITGROUP_STOMACH:
		currentDamage *= 1.25f;
		break;
	case HITGROUP_LEFTLEG:
	case HITGROUP_RIGHTLEG:
		currentDamage *= 0.75f;
		break;
	default:
		break;
	}

	if (armorValue > 0 && IsArmored())
	{
		float bonusValue = 1.f, armorBonusRatio = 0.5f, armorRatio = weaponData->armor_ratio / 2.f;

		//Damage gets modified for heavy armor users
		if (hasHeavyArmor)
		{
			armorBonusRatio = 0.33f;
			armorRatio *= 0.5f;
			bonusValue = 0.33f;
		}

		auto NewDamage = currentDamage * armorRatio;

		if (hasHeavyArmor)
			NewDamage *= 0.85f;

		if (((currentDamage - (currentDamage * armorRatio)) * (bonusValue * armorBonusRatio)) > armorValue)
			NewDamage = currentDamage - (armorValue / armorBonusRatio);

		currentDamage = NewDamage;
	}
}

bool Autowall::DidHitWorld(C_BaseEntity * ent)
{
	return ent == I::EntityList->GetClientEntity(0);
}

bool Autowall::DidHitNonWorldEntity(C_BaseEntity * ent)
{
	return ent != nullptr && !DidHitWorld(ent);
}

////////////////////////////////////// Main Autowall Functions //////////////////////////////////////
bool Autowall::TraceToExit(trace_t& enterTrace, trace_t& exitTrace, Vector startPosition, Vector direction)
{
	/*
	Masks used:
	MASK_SHOT_HULL					 = 0x600400B
	CONTENTS_HITBOX					 = 0x40000000
	MASK_SHOT_HULL | CONTENTS_HITBOX = 0x4600400B
	*/

	Vector start, end;
	float maxDistance = 90.f, rayExtension = 4.f, currentDistance = 0;
	int firstContents = 0;

	while (currentDistance <= maxDistance)
	{
		//Add extra distance to our ray
		currentDistance += rayExtension;

		//Multiply the direction vector to the distance so we go outwards, add our position to it.
		start = startPosition + direction * currentDistance;

		if (!firstContents)
			firstContents = I::Trace->GetPointContents(start, MASK_SHOT_HULL | CONTENTS_HITBOX, nullptr); /*0x4600400B*/
		int pointContents = I::Trace->GetPointContents(start, MASK_SHOT_HULL | CONTENTS_HITBOX, nullptr);

		if (!(pointContents & MASK_SHOT_HULL) || pointContents & CONTENTS_HITBOX && pointContents != firstContents) /*0x600400B, *0x40000000*/
		{
			//Let's setup our end position by deducting the direction by the extra added distance
			end = start - (direction * rayExtension);

			//Let's cast a ray from our start pos to the end pos
			g_MiscFunc->TraceLine(start, end, MASK_SHOT_HULL | CONTENTS_HITBOX, nullptr, &exitTrace);

			//Let's check if a hitbox is in-front of our enemy and if they are behind of a solid wall
			if (exitTrace.startsolid && exitTrace.surface.flags & SURF_HITBOX)
			{
				g_MiscFunc->TraceLine(start, startPosition, MASK_SHOT_HULL, exitTrace.hit_entity, &exitTrace);

				if (exitTrace.hitgroup != 0 && !exitTrace.startsolid)
				{
					start = exitTrace.endpos;
					return true;
				}

				continue;
			}

			//Can we hit? Is the wall solid?
			if (exitTrace.DidHit() && !exitTrace.startsolid)
			{
				//Is the wall a breakable? If so, let's shoot through it.
				if (IsBreakableEntity(enterTrace.hit_entity) && IsBreakableEntity(exitTrace.hit_entity))
					return true;

				if (enterTrace.surface.flags & SURF_NODRAW || !(exitTrace.surface.flags & SURF_NODRAW) && (exitTrace.plane.normal.Dot(direction) <= 1.f))
				{
					float multAmount = exitTrace.fraction * 4.f;
					start -= direction * multAmount;
					return true;
				}

				continue;
			}

			if (!exitTrace.DidHit() || exitTrace.startsolid)
			{
				if (DidHitNonWorldEntity(enterTrace.hit_entity) && IsBreakableEntity(enterTrace.hit_entity))
				{
					exitTrace = enterTrace;
					exitTrace.endpos = start + direction;
					return true;
				}

				continue;
			}
		}
	}
	return false;
}

#define CHAR_TEX_FLESH		'F'
#define CHAR_TEX_GRATE		'G'
#define CHAR_TEX_PLASTIC	'L'
#define CHAR_TEX_METAL		'M'
#define CHAR_TEX_CARDBOARD	'U'
#define CHAR_TEX_WOOD		'W'
#define CHAR_TEX_GLASS		'Y'

bool Autowall::HandleBulletPenetration(CSWeaponInfo* weaponData, trace_t& enterTrace, Vector& eyePosition, Vector direction, int& possibleHitsRemaining, float& currentDamage, float penetrationPower, bool sv_penetration_type, float ff_damage_reduction_bullets, float ff_damage_bullet_penetration)
{
	//Because there's been issues regarding this- putting this here.
	if (&currentDamage == nullptr)
		throw std::invalid_argument("currentDamage is null!\n");

	trace_t exitTrace;
	C_BaseEntity* pEnemy = (C_BaseEntity*)enterTrace.hit_entity;
	surfacedata_t *enterSurfaceData = I::PhysicsProps->GetSurfaceData(enterTrace.surface.surfaceProps);
	int enterMaterial = enterSurfaceData->game.material;

	float enterSurfPenetrationModifier = enterSurfaceData->game.flPenetrationModifier;
	float enterDamageModifier = enterSurfaceData->game.flDamageModifier;
	float thickness, modifier, lostDamage, finalDamageModifier, combinedPenetrationModifier;
	bool isSolidSurf = ((enterTrace.contents >> 3) & CONTENTS_SOLID);
	bool isLightSurf = ((enterTrace.surface.flags >> 7) & SURF_LIGHT);

	if (possibleHitsRemaining <= 0
		//Test for "DE_CACHE/DE_CACHE_TELA_03" as the entering surface and "CS_ITALY/CR_MISCWOOD2B" as the exiting surface.
		//Fixes a wall in de_cache which seems to be broken in some way. Although bullet penetration is not recorded to go through this wall
		//Decals can be seen of bullets within the glass behind of the enemy. Hacky method, but works.
		//You might want to have a check for this to only be activated on de_cache.
		|| (enterTrace.surface.name == (const char*)0x2227c261 && exitTrace.surface.name == (const char*)0x2227c868)
		|| (!possibleHitsRemaining && !isLightSurf && !isSolidSurf && enterMaterial != CHAR_TEX_GRATE && enterMaterial != CHAR_TEX_GLASS)
		|| weaponData->penetration <= 0.f
		|| !TraceToExit(enterTrace, exitTrace, enterTrace.endpos, direction)
		&& !(I::Trace->GetPointContents(enterTrace.endpos, MASK_SHOT_HULL, nullptr) & MASK_SHOT_HULL))
		return false;

	surfacedata_t *exitSurfaceData = I::PhysicsProps->GetSurfaceData(exitTrace.surface.surfaceProps);
	int exitMaterial = exitSurfaceData->game.material;
	float exitSurfPenetrationModifier = exitSurfaceData->game.flPenetrationModifier;
	float exitDamageModifier = exitSurfaceData->game.flDamageModifier;

	//Are we using the newer penetration system?
	if (sv_penetration_type)
	{
		if (enterMaterial == CHAR_TEX_GRATE || enterMaterial == CHAR_TEX_GLASS)
		{
			combinedPenetrationModifier = 3.f;
			finalDamageModifier = 0.05f;
		}
		else if (isSolidSurf || isLightSurf)
		{
			combinedPenetrationModifier = 1.f;
			finalDamageModifier = 0.16f;
		}
		else if (enterMaterial == CHAR_TEX_FLESH && (g_Globals->LocalPlayer->GetTeam() == pEnemy->GetTeam() && ff_damage_reduction_bullets == 0.f)) //TODO: Team check config
		{
			//Look's like you aren't shooting through your teammate today
			if (ff_damage_bullet_penetration == 0.f)
				return false;

			//Let's shoot through teammates and get kicked for teamdmg! Whatever, atleast we did damage to the enemy. I call that a win.
			combinedPenetrationModifier = ff_damage_bullet_penetration;
			finalDamageModifier = 0.16f;
		}
		else
		{
			combinedPenetrationModifier = (enterSurfPenetrationModifier + exitSurfPenetrationModifier) / 2.f;
			finalDamageModifier = 0.16f;
		}

		//Do our materials line up?
		if (enterMaterial == exitMaterial)
		{
			if (exitMaterial == CHAR_TEX_CARDBOARD || exitMaterial == CHAR_TEX_WOOD)
				combinedPenetrationModifier = 3.f;
			else if (exitMaterial == CHAR_TEX_PLASTIC)
				combinedPenetrationModifier = 2.f;
		}

		//Calculate thickness of the wall by getting the length of the range of the trace and squaring
		thickness = (exitTrace.endpos - enterTrace.endpos).LengthSqr();
		modifier = fmaxf(1.f / combinedPenetrationModifier, 0.f);

		//This calculates how much damage we've lost depending on thickness of the wall, our penetration, damage, and the modifiers set earlier
		lostDamage = fmaxf(
			((modifier * thickness) / 24.f)
			+ ((currentDamage * finalDamageModifier)
				+ (fmaxf(3.75f / penetrationPower, 0.f) * 3.f * modifier)), 0.f);

		//Did we loose too much damage?
		if (lostDamage > currentDamage)
			return false;

		//We can't use any of the damage that we've lost
		if (lostDamage > 0.f)
			currentDamage -= lostDamage;

		//Do we still have enough damage to deal?
		if (currentDamage < 1.f)
			return false;

		eyePosition = exitTrace.endpos;
		--possibleHitsRemaining;

		return true;
	}
	else //Legacy penetration system
	{
		combinedPenetrationModifier = 1.f;

		if (isSolidSurf || isLightSurf)
			finalDamageModifier = 0.99f; //Good meme :^)
		else
		{
			finalDamageModifier = fminf(enterDamageModifier, exitDamageModifier);
			combinedPenetrationModifier = fminf(enterSurfPenetrationModifier, exitSurfPenetrationModifier);
		}

		if (enterMaterial == exitMaterial && (exitMaterial == CHAR_TEX_METAL || exitMaterial == CHAR_TEX_WOOD))
			combinedPenetrationModifier += combinedPenetrationModifier;

		thickness = (exitTrace.endpos - enterTrace.endpos).LengthSqr();

		if (sqrt(thickness) <= combinedPenetrationModifier * penetrationPower)
		{
			currentDamage *= finalDamageModifier;
			eyePosition = exitTrace.endpos;
			--possibleHitsRemaining;

			return true;
		}

		return false;
	}
}

bool Autowall::FireBullet(C_BaseCombatWeapon* pWeapon, Vector& direction, float& currentDamage)
{
	if (!pWeapon)
		return false;

	bool sv_penetration_type;
	//	  Current bullet travel Power to penetrate Distance to penetrate Range               Player bullet reduction convars			  Amount to extend ray by
	float currentDistance = 0.f, penetrationPower, penetrationDistance, maxRange, ff_damage_reduction_bullets, ff_damage_bullet_penetration, rayExtension = 40.f;
	Vector eyePosition = g_Globals->LocalPlayer->GetEyePosition();

	//For being superiour when the server owners think your autowall isn't well reversed. Imagine a meme HvH server with the old penetration system- pff
	static ConVar* penetrationSystem = I::CVar->FindVar(("sv_penetration_type"));
	static ConVar* damageReductionBullets = I::CVar->FindVar(("ff_damage_reduction_bullets"));
	static ConVar* damageBulletPenetration = I::CVar->FindVar(("ff_damage_bullet_penetration"));

	sv_penetration_type = penetrationSystem->GetInt();
	ff_damage_reduction_bullets = damageReductionBullets->GetFloat();
	ff_damage_bullet_penetration = damageBulletPenetration->GetFloat();

	CSWeaponInfo* weaponData = pWeapon->GetCSWpnData();
	trace_t enterTrace;

	//We should be skipping g_Globals->LocalPlayer when casting a ray to players.
	CTraceFilter filter;
	filter.pSkip = g_Globals->LocalPlayer;

	if (!weaponData)
		return false;

	maxRange = weaponData->range;

	GetBulletTypeParameters(penetrationPower, penetrationDistance, "", sv_penetration_type);
	penetrationPower = 35.0;
	penetrationDistance = 3000.0;

	if (sv_penetration_type)
		penetrationPower = weaponData->penetration;

	//This gets set in FX_Firebullets to 4 as a pass-through value.
	//CS:GO has a maximum of 4 surfaces a bullet can pass-through before it 100% stops.
	//Excerpt from Valve: https://steamcommunity.com/sharedfiles/filedetails/?id=275573090
	//"The total number of surfaces any bullet can penetrate in a single flight is capped at 4." -CS:GO Official
	int possibleHitsRemaining = 4;

	//Set our current damage to what our gun's initial damage reports it will do
	currentDamage = weaponData->damage;

	//If our damage is greater than (or equal to) 1, and we can shoot, let's shoot.
	while (possibleHitsRemaining > 0 && currentDamage >= 1.f)
	{
		//Calculate max bullet range
		maxRange -= currentDistance;

		//Create endpoint of bullet
		Vector end = eyePosition + direction * maxRange;

		g_MiscFunc->TraceLine(eyePosition, end, MASK_SHOT_HULL | CONTENTS_HITBOX, g_Globals->LocalPlayer, &enterTrace);
		ClipTraceToPlayers(eyePosition, end + direction * rayExtension, MASK_SHOT_HULL | CONTENTS_HITBOX, &filter, &enterTrace);

		//We have to do this *after* tracing to the player.
		surfacedata_t *enterSurfaceData = I::PhysicsProps->GetSurfaceData(enterTrace.surface.surfaceProps);
		float enterSurfPenetrationModifier = enterSurfaceData->game.flPenetrationModifier;
		int enterMaterial = enterSurfaceData->game.material;

		//"Fraction == 1" means that we didn't hit anything. We don't want that- so let's break on it.
		if (enterTrace.fraction == 1.f) 
			break;

		//calculate the damage based on the distance the bullet traveled.
		currentDistance += enterTrace.fraction * maxRange;

		//Let's make our damage drops off the further away the bullet is.
		currentDamage *= pow(weaponData->range_modifier, (currentDistance / 500.f));

		//Sanity checking / Can we actually shoot through?
		if (currentDistance > penetrationDistance && weaponData->penetration > 0.f || enterSurfPenetrationModifier < 0.1f)
			break;

		//This looks gay as fuck if we put it into 1 long line of code.
		bool canDoDamage = (enterTrace.hitgroup != HITGROUP_GEAR && enterTrace.hitgroup != HITGROUP_GENERIC);
		bool isPlayer = ((enterTrace.hit_entity)->GetClientClass()->iClassID == 35);
		bool isEnemy = (((C_BaseEntity*)g_Globals->LocalPlayer)->GetTeam() != ((C_BaseEntity*)enterTrace.hit_entity)->GetTeam());
		bool onTeam = (((C_BaseEntity*)enterTrace.hit_entity)->GetTeam() == (int32_t)3 || ((C_BaseEntity*)enterTrace.hit_entity)->GetTeam() == (int32_t)2);

		//TODO: Team check config
		if ((canDoDamage && isPlayer && isEnemy) && onTeam)
		{
			ScaleDamage(enterTrace, weaponData, currentDamage);
			return true;
		}

		//Calling HandleBulletPenetration here reduces our penetrationCounter, and if it returns true, we can't shoot through it.
		if (!HandleBulletPenetration(weaponData, enterTrace, eyePosition, direction, possibleHitsRemaining, currentDamage, penetrationPower, sv_penetration_type, ff_damage_reduction_bullets, ff_damage_bullet_penetration))
			break;
	}

	return false;
}

static Vector umomaim;

float GetHitgroupDamageMult(int iHitGroup)
{
	switch (iHitGroup)
	{
	case HITGROUP_GENERIC:
		return 0.5f;
	case HITGROUP_HEAD:
		return 4.0f;
	case HITGROUP_CHEST:
		return 1.f;
	case HITGROUP_STOMACH:
		return 1.25f;
	case HITGROUP_LEFTARM:
		return 0.65f;
	case HITGROUP_RIGHTARM:
		return 0.65f;
	case HITGROUP_LEFTLEG:
		return 0.75f;
	case HITGROUP_RIGHTLEG:
		return 0.75f;
	case HITGROUP_GEAR:
		return 0.5f;
	default:
		return 1.0f;
	}
}

void ScaleDamage(int hitgroup, C_BaseEntity *enemy, float weapon_armor_ratio, float &current_damage)
{
	current_damage *= GetHitgroupDamageMult(hitgroup);

	if (enemy->ArmorValue() > 0)
	{
		if (hitgroup == HITGROUP_HEAD)
		{
			if (enemy->HasHelmet())
				current_damage *= (weapon_armor_ratio * .5f);
		}
		else
		{
			current_damage *= (weapon_armor_ratio * .5f);
		}
	}
}

bool HandleBulletPenetration2(CSWeaponInfo *wpn_data, FireBulletData &data, bool extracheck)
{
	surfacedata_t *enter_surface_data = I::PhysicsProps->GetSurfaceData(data.enter_trace.surface.surfaceProps);
	int enter_material = enter_surface_data->game.material;
	float enter_surf_penetration_mod = enter_surface_data->game.flPenetrationModifier;

	data.trace_length += data.enter_trace.fraction * data.trace_length_remaining;
	data.current_damage *= pow(wpn_data->range_modifier, (data.trace_length * 0.002f));

	if ((data.trace_length > 3000.f) || (enter_surf_penetration_mod < 0.1f))
		data.penetrate_count = 0;

	if (data.penetrate_count <= 0)
		return false;

	Vector dummy;
	trace_t trace_exit;
	if (!g_MiscFunc->TraceToExit(dummy, &data.enter_trace, data.enter_trace.endpos, data.direction, &trace_exit))
		return false;

	surfacedata_t *exit_surface_data = I::PhysicsProps->GetSurfaceData(trace_exit.surface.surfaceProps);
	int exit_material = exit_surface_data->game.material;

	float exit_surf_penetration_mod = exit_surface_data->game.flPenetrationModifier;
	float final_damage_modifier = 0.16f;
	float combined_penetration_modifier = 0.0f;

	if (((data.enter_trace.contents & CONTENTS_GRATE) != 0) || (enter_material == 89) || (enter_material == 71))
	{
		combined_penetration_modifier = 3.0f;
		final_damage_modifier = 0.05f;
	}
	else
	{
		combined_penetration_modifier = (enter_surf_penetration_mod + exit_surf_penetration_mod) * 0.5f;
	}

	if (enter_material == exit_material)
	{
		if (exit_material == 87 || exit_material == 85)
			combined_penetration_modifier = 3.0f;
		else if (exit_material == 76)
			combined_penetration_modifier = 2.0f;
	}

	float v34 = fmaxf(0.f, 1.0f / combined_penetration_modifier);
	float v35 = (data.current_damage * final_damage_modifier) + v34 * 3.0f * fmaxf(0.0f, (3.0f / wpn_data->penetration) * 1.25f);
	float thickness = VectorLength(trace_exit.endpos - data.enter_trace.endpos);

	if (extracheck)
	{
		if (!g_MiscFunc->VectortoVectorVisible(trace_exit.endpos, umomaim))
			return false;
	}

	thickness *= thickness;
	thickness *= v34;
	thickness /= 24.0f;

	float lost_damage = fmaxf(0.0f, v35 + thickness);

	if (lost_damage > data.current_damage)
		return false;

	if (lost_damage >= 0.0f)
		data.current_damage -= lost_damage;

	if (data.current_damage < 1.0f)
		return false;

	data.src = trace_exit.endpos;
	data.penetrate_count--;

	return true;
}

////////////////////////////////////// Usage Calls //////////////////////////////////////
float Autowall::CanHit(Vector &vecEyePos, Vector &point)
{
	Vector angles, direction;
	Vector tmp = point - g_Globals->LocalPlayer->GetEyePosition();
	float currentDamage = 0;

	g_Math->VectorAngles(tmp, angles);
	g_Math->AngleVectors(angles, &direction);
	direction.NormalizeInPlace();

	if (FireBullet(g_Globals->LocalPlayer->GetActiveWeapon(), direction, currentDamage))
		return currentDamage;

	return -1; //That wall is just a bit too thick buddy
}

bool Autowall::CanHitFloatingPoint(const Vector &point, const Vector &source, float *damage_given)
{
	umomaim = point;

	if (!g_Globals->LocalPlayer)
		return false;

	FireBulletData &data = FireBulletData(source);
	data.filter = CTraceFilter();
	data.filter.pSkip = g_Globals->LocalPlayer;

	Vector angles = g_Math->CalcAngle(data.src, point);
	g_Math->AngleVectors(angles, &data.direction);
	VectorNormalize(data.direction);

	C_BaseCombatWeapon *pWeapon = (C_BaseCombatWeapon*)g_Globals->LocalPlayer->GetActiveWeapon();

	if (!pWeapon)
		return false;

	data.penetrate_count = 1;
	data.trace_length = 0.0f;

	CSWeaponInfo *weaponData = pWeapon->GetCSWpnData();

	if (!weaponData)
		return false;

	data.current_damage = (float)weaponData->damage;

	data.trace_length_remaining = weaponData->range - data.trace_length;

	Vector end = data.src + data.direction * data.trace_length_remaining;

	g_MiscFunc->TraceLine(data.src, end, MASK_SHOT | CONTENTS_GRATE, g_Globals->LocalPlayer, &data.enter_trace);

	if (g_MiscFunc->VectortoVectorVisible(point, data.src))
	{
		*damage_given = data.current_damage;
		return true;
	}

	if (HandleBulletPenetration2(weaponData, data, true))
	{
		*damage_given = data.current_damage;
		return true;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////////
// Wow I took way too long on autowall.
// Special Thanks to the following (No order intended)
// Killerra
// Flaw
// Zbe
// Heep
// Polak (indirectly; IsBreakable Fix)
// Esoterik (Autowall UC Post)
// N0xius (ScaleDamage)
// Requiii
// drew1ind#8957 (Scaling for ConVar)
// The non-autistic slice of CS:GO-UC