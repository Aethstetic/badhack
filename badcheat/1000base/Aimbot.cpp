#include "Main.h"

bool Aimbot::Hitscan(C_BaseEntity* Entity, C_BaseCombatWeapon* pWeapon, Vector *Aimpoint, float *LastDmg)
{
	RecoilScale = I::CVar->FindVar("weapon_recoil_scale");
	static float DamageArray[25];
	float tempDmg = 0;
	Vector tempHitbox;

	int HitboxForMuti[] = { 2, 2, 4, 4, 6, 6 };

	float angToLocal = g_Math->CalcAngle(g_Globals->LocalPlayer->GetOrigin(), Entity->GetOrigin()).y;

	Vector2D MutipointXY = { (g_Vars->Aimbot.BodyPointscale * sin(g_Math->GRD_TO_BOG(angToLocal))),(g_Vars->Aimbot.BodyPointscale * cos(g_Math->GRD_TO_BOG(angToLocal))) };
	Vector2D MutipointXY180 = { (g_Vars->Aimbot.BodyPointscale * sin(g_Math->GRD_TO_BOG(angToLocal + 180))) ,(g_Vars->Aimbot.BodyPointscale * cos(g_Math->GRD_TO_BOG(angToLocal + 180))) };
	Vector2D Mutipoint[] = { Vector2D(MutipointXY.x, MutipointXY.y), Vector2D(MutipointXY180.x, MutipointXY180.y) };

	float Velocity = abs(Entity->GetVelocity().Length2D());

	static bool initHitscan = true;

	std::vector<int> Scan;

	Scan.push_back(HITBOX_HEAD);

	if (!g_Vars->Aimbot.HeadOnly) // somebody fix this aids soon please
	{
		Scan.push_back(HITBOX_PELVIS);
		Scan.push_back(19); // pelvis mutipoint 1
		Scan.push_back(20); // pelvis mutipoint 2
		Scan.push_back(HITBOX_THORAX);
		Scan.push_back(21); // thorax mutipoint 1
		Scan.push_back(22); // thorax mutipoint 2
		Scan.push_back(HITBOX_LOWER_CHEST);
		Scan.push_back(HITBOX_UPPER_CHEST);
		Scan.push_back(23); // upper chest mutipoint 1
		Scan.push_back(24); // upper chest mutipoint 2
		Scan.push_back(HITBOX_LEFT_UPPER_ARM);
		Scan.push_back(HITBOX_RIGHT_UPPER_ARM);
		Scan.push_back(HITBOX_LEFT_THIGH);
		Scan.push_back(HITBOX_RIGHT_THIGH);
		Scan.push_back(HITBOX_RIGHT_CALF);
		Scan.push_back(HITBOX_LEFT_CALF);

		(Velocity <= 29) ? Scan.push_back(HITBOX_LEFT_FOOT) : DamageArray[HITBOX_LEFT_FOOT] = 0;
		(Velocity <= 29) ? Scan.push_back(HITBOX_RIGHT_FOOT) : DamageArray[HITBOX_RIGHT_FOOT] = 0;

		if (initHitscan)
			initHitscan = false;
	}
	else
	{
		if (!initHitscan)
		{
			for (int i = 1; i < 25; i++)
				DamageArray[i] = 0;

			initHitscan = true;
		}
	}

	// fill aimrecords
	if (!g_Backtrack->FillAimInfo(Entity))
		return false;

	static Vector Hitbox;

	LagData rec;
	while (FindBestRecord(Entity, &rec))
	{
		/* scan hitboxes */
		for (int hitbox : Scan)
		{
			if (hitbox < 19)
				Hitbox = Entity->GetHitboxPosition(hitbox, rec.matrix) + Vector(0, 0, (hitbox == 0) ? -5 + g_Vars->Aimbot.HeadHeight : 0);
			else if (hitbox > 18 && hitbox < 25)
			{
				Hitbox = Entity->GetHitboxPosition(HitboxForMuti[hitbox - 19], rec.matrix);
				Hitbox = Vector(Hitbox.x + Mutipoint[((hitbox - 19) % 2)].x, Hitbox.y - Mutipoint[((hitbox - 19) % 2)].y, Hitbox.z);
			}

			/* calculate damage */
			float damage = g_Autowall->CanHit(g_Globals->LocalPlayer->GetEyePosition(), Hitbox);
			if (damage >= g_Vars->Aimbot.MinDamage && g_Vars->Aimbot.Autowall)
				DamageArray[hitbox] = damage;
			else
				DamageArray[hitbox] = 0;

			/* get the hitbox that does highest damage */
			if (DamageArray[hitbox] > tempDmg)
			{
				tempHitbox = Hitbox;
				tempDmg = DamageArray[hitbox];
			}
		}

		if (tempDmg > g_Vars->Aimbot.MinDamage)
		{
			*Aimpoint = tempHitbox;
			*LastDmg = tempDmg;

			best_tick = TIME_TO_TICKS(rec.simtime);

			std::memcpy(matrix2, rec.matrix, sizeof(rec.matrix));

			Vector HitchanceAngle = g_Math->CalcAngle(g_Globals->LocalPlayer->GetEyePosition(), tempHitbox) - g_Globals->LocalPlayer->GetAimPunchAngle() * RecoilScale->GetFloat();
			hitchanced = g_Vars->Aimbot.HitChance > 0 ? g_MiscFunc->HitChance(Entity, pWeapon, HitchanceAngle, g_Vars->Aimbot.HitChance) : true;

			/* restore them back to original data */
			g_Backtrack->RestoreInfo(Entity, g_Backtrack->RestoreRecord[Entity->EntIndex()].second);
			return true;
		}
	}

	/* restore them back to original data */
	g_Backtrack->RestoreInfo(Entity, g_Backtrack->RestoreRecord[Entity->EntIndex()].second);
	return false;
}


bool Aimbot::CanFireWeapon(CUserCmd* cmd)
{
	auto local_player = g_Globals->LocalPlayer;
	if (!local_player || !local_player->IsAlive())
		return false;

	if (local_player->GetFlags() & FL_ATCONTROLS)
		return false;

	C_BaseCombatWeapon *weapon = (C_BaseCombatWeapon*)local_player->GetActiveWeapon();
	if (!weapon)
		return false;

	if (weapon->GetAmmo() == 0)
		return false;

	return (weapon->GetNextAttack() < I::Globals->curtime) && (weapon->GetNextAttack() < I::Globals->curtime);
}

bool Aimbot::FindBestRecord(C_BaseEntity *player, LagData* record)
{
	auto &lag_records = g_Backtrack->PlayerRecords[player->EntIndex()];

	LagData recentLR = *g_Backtrack->AimRecords.begin();
	g_Backtrack->AimRecords.pop_front();

	/*
	*	fuck you stickrpg, imo this is the cleaner way to do it. (fixing lagcomp goes here)
	*/

	g_Backtrack->RestoreInfo(player, recentLR);
	*record = recentLR;
	return true;
}

void Aimbot::StopMovement(CUserCmd* cmd)
{
	Vector Velocity = g_Globals->LocalPlayer->GetVelocity();
	float speed = Velocity.Length();

	Vector direction;
	Vector RealView;
	g_Math->VectorAngles(Velocity, direction);
	I::Engine->GetViewAngles(RealView);
	direction.y = RealView.y - direction.y;

	Vector forward;
	g_Math->AngleVectors(direction, &forward);
	Vector negated_direction = forward * -speed;

	cmd->forwardmove = negated_direction.x;
	cmd->sidemove = negated_direction.y;
}

void Aimbot::Aim(CUserCmd* pCmd, bool *bSendPacket)
{
	if (g_Vars->Aimbot.Enable) {

		static float Damage;
		static Vector Aimpoint;
		static bool HitchanceCheck;
		static float ShotTime = 0;
		static bool Shot = false;

		C_BaseCombatWeapon *pWeapon = (C_BaseCombatWeapon*)g_Globals->LocalPlayer->GetActiveWeapon();

		if (!pWeapon || g_Globals->LocalPlayer->IsKnifeorNade()) return;

		g_Globals->EnemyIndex = -1;

		static C_BaseEntity* Target = nullptr;
		for (int i = 1; i <= I::Engine->GetMaxClients(); ++i) {
			C_BaseEntity* Entity = I::EntityList->GetClientEntity(i);
			if (!Entity
				|| !Entity->IsAlive()
				|| Entity->IsDormant()
				|| Entity == g_Globals->LocalPlayer
				|| Entity->HasImmunity()
				|| Entity->GetTeam() == g_Globals->LocalPlayer->GetTeam())
				continue;

			if (pWeapon->GetItemDefenitionIndex() == 64) // reversed aimware by octal (c)
			{
				const auto v7 = I::Globals->curtime;
				if (g_Globals->m_r8time <= (float)(I::Globals->frametime + v7))
					g_Globals->m_r8time = v7 + 0.25f;
				else
					pCmd->buttons |= IN_ATTACK;
			}
			else
			{
				g_Globals->m_r8time = 0.0f;
			}

			// we cant shoot yet... duh
			if (!CanFireWeapon(pCmd))
				return;

			g_Globals->EnemyIndex = -1;

			static C_BaseEntity* Target = nullptr;

			for (int i = 1; i <= I::Engine->GetMaxClients(); ++i) {
				C_BaseEntity* Entity = I::EntityList->GetClientEntity(i);
				if (!Entity
					|| !Entity->IsAlive()
					|| Entity->IsDormant()
					|| Entity == g_Globals->LocalPlayer
					|| Entity->HasImmunity()
					|| Entity->GetTeam() == g_Globals->LocalPlayer->GetTeam())
					continue;

				if (Hitscan(Entity, pWeapon, &Aimpoint, &Damage))
				{
					Target = Entity;
					g_Globals->EnemyIndex = Entity->EntIndex();
					break;
				}
				else
					continue;
			}

			if (Hitscan(Entity, pWeapon, &Aimpoint, &Damage))
			{
				Target = Entity;
				g_Globals->EnemyIndex = Entity->EntIndex();
				break;
			}
			else
				continue;
		}

		if (Shot)
		{
			*bSendPacket = true;
			Shot = false;
		}

		/* not able to shoot yet */
		if (!pWeapon->CanFire() || ShotTime >= I::Globals->curtime || g_Globals->EnemyIndex == -1)
			return;

		g_Globals->EnemyIndex = Target->EntIndex();

		/* autostop */
		if (g_Vars->Aimbot.AutoStop && g_Globals->LocalPlayer->GetVelocity().Length() >= (pWeapon->GetCSWpnData()->max_speed_alt * .33f) && !GetAsyncKeyState(VK_SPACE))
			StopMovement(pCmd);

		if (!(pCmd->buttons & IN_ATTACK) && hitchanced && !*bSendPacket)
		{
			pCmd->viewangles = g_Math->CalcAngle(g_Globals->LocalPlayer->GetEyePosition(), Aimpoint) - g_Globals->LocalPlayer->GetAimPunchAngle() * RecoilScale->GetFloat();
			Shot = true;

			/* used for shot records */
			g_Globals->LastAimVec = Aimpoint;
			std::memcpy(g_Globals->LastAimMatrix, matrix2, sizeof(matrix2));
			g_MissedShots->AddSnapshot(Target);

			/* aimbot stuff */
			pCmd->buttons |= IN_ATTACK;
			pCmd->tick_count = best_tick + g_MiscFunc->GetLerpTicks();
			ShotTime = g_MiscFunc->FireRate(pWeapon) + I::Globals->curtime; //cancer

			if (pWeapon->GetItemDefenitionIndex() == 64)
				g_Globals->m_r8time = (float)(0.25f + I::Globals->curtime + 0.002f);

			//ShotTime = g_MiscFunc->FireRate(pWeapon) + I::Globals->curtime; // cancer rebuilt firerate function, im so sorry
		}
	}
}