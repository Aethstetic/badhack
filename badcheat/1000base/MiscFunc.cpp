#include "Main.h"

void MiscFunc::MovementFix(Vector& OriginalAng, CUserCmd* pCmd)
{
	Vector vMove = Vector(pCmd->forwardmove, pCmd->sidemove, 0.0f);
	float flSpeed = vMove.Length();
	Vector qMove;
	g_Math->VectorAngles(vMove, qMove);
	float normalized = fmod(pCmd->viewangles.y + 180.f, 360.f) - 180.f;
	float normalizedx = fmod(pCmd->viewangles.x + 180.f, 360.f) - 180.f;
	float flYaw = DEG2RAD((normalized - OriginalAng.y) + qMove.y);
	if (normalizedx >= 90.0f || normalizedx <= -90.0f) pCmd->forwardmove = -cos(flYaw) * flSpeed;
	else pCmd->forwardmove = cos(flYaw) * flSpeed;
	pCmd->sidemove = sin(flYaw) * flSpeed;
}

void MiscFunc::TraceLine(Vector& vecAbsStart, Vector& vecAbsEnd, unsigned int mask, C_BaseEntity *ignore, trace_t *ptr)
{
	Ray_t ray;
	ray.Init(vecAbsStart, vecAbsEnd);
	CTraceFilter filter;
	filter.pSkip = ignore;

	I::Trace->TraceRay(ray, mask, &filter, ptr);
}

bool DidHitNonWorldEntity(C_BaseEntity* m_pEnt)
{
	return m_pEnt != nullptr && m_pEnt == I::EntityList->GetClientEntity(0);
}

bool MiscFunc::TraceToExit(Vector& vecEnd, trace_t* pEnterTrace, Vector vecStart, Vector vecDir, trace_t* pExitTrace)
{
	auto distance = 0.0f;

	while (distance < 90.0f)
	{
		distance += 4.0f;
		vecEnd = vecStart + vecDir * distance;

		auto point_contents = I::Trace->GetPointContents(vecEnd, MASK_SHOT_HULL | CONTENTS_HITBOX, NULL);

		if (point_contents & MASK_SHOT_HULL && (!(point_contents & CONTENTS_HITBOX)))
			continue;

		auto new_end = vecEnd - (vecDir * 4.0f);

		TraceLine(vecEnd, new_end, 0x4600400B, 0, pExitTrace);

		if (pExitTrace->startsolid && pExitTrace->surface.flags & SURF_HITBOX)
		{
			TraceLine(vecEnd, vecStart, 0x600400B, pExitTrace->hit_entity, pExitTrace);

			if ((pExitTrace->fraction < 1.0f || pExitTrace->allsolid) && !pExitTrace->startsolid)
			{
				vecEnd = pExitTrace->endpos;
				return true;
			}

			continue;
		}

		if (!(pExitTrace->fraction < 1.0 || pExitTrace->allsolid || pExitTrace->startsolid) || pExitTrace->startsolid)
		{
			if (pExitTrace->hit_entity)
			{
				if (!DidHitNonWorldEntity(pExitTrace->hit_entity))
					return true;
			}

			continue;
		}

		if (((pExitTrace->surface.flags >> 7) & 1) && !((pExitTrace->surface.flags >> 7) & 1))
			continue;

		if (pExitTrace->plane.normal.Dot(vecDir) < 1.0f)
		{
			float fraction = pExitTrace->fraction * 4.0f;
			vecEnd = vecEnd - (vecDir * fraction);

			return true;
		}
	}

	return false;
}

bool MiscFunc::HitChance(C_BaseEntity* Entity, C_BaseCombatWeapon* pWeapon, Vector Angle, int chance)
{
	if (!chance)
		return true;

	int iHit = 0;
	int iHitsNeed = (int)((float)256 * ((float)chance / 100.f));
	bool bHitchance = false;

	Vector forward, right, up;
	g_Math->AngleVectors(Angle, &forward, &right, &up);

	pWeapon->AccuracyPenalty();

	for (auto i = 0; i < 256; ++i) {

		float RandomA = g_Math->RandomFloat(0.0f, 1.0f);
		float RandomB = 1.0f - RandomA * RandomA;

		RandomA = g_Math->RandomFloat(0.0f, M_PI_F * 2.0f);
		RandomB *= pWeapon->GetInaccuracy();

		float SpreadX1 = (cos(RandomA) * RandomB);
		float SpreadY1 = (sin(RandomA) * RandomB);

		float RandomC = g_Math->RandomFloat(0.0f, 1.0f);
		float RandomF = RandomF = 1.0f - RandomC * RandomC;

		RandomC = g_Math->RandomFloat(0.0f, M_PI_F * 2.0f);
		RandomF *= pWeapon->GetSpread();

		float SpreadX2 = (cos(RandomC) * RandomF);
		float SpreadY2 = (sin(RandomC) * RandomF);

		float fSpreadX = SpreadX1 + SpreadX2;
		float fSpreadY = SpreadY1 + SpreadY2;

		Vector vSpreadForward;
		vSpreadForward[0] = forward[0] + (fSpreadX * right[0]) + (fSpreadY * up[0]);
		vSpreadForward[1] = forward[1] + (fSpreadX * right[1]) + (fSpreadY * up[1]);
		vSpreadForward[2] = forward[2] + (fSpreadX * right[2]) + (fSpreadY * up[2]);
		vSpreadForward.NormalizeInPlace();

		Vector qaNewAngle;
		g_Math->VectorAngles(vSpreadForward, qaNewAngle);
		g_Math->NormalizeAngles(qaNewAngle);

		Vector vEnd;
		g_Math->AngleVectors(qaNewAngle, &vEnd);

		vEnd = g_Globals->LocalPlayer->GetEyePosition() + (vEnd * 8192.f);

		trace_t tr;
		Ray_t ray;

		ray.Init(g_Globals->LocalPlayer->GetEyePosition(), vEnd);
		I::Trace->ClipRayToEntity(ray, MASK_SHOT | CONTENTS_GRATE, Entity, &tr);

		if (tr.hit_entity == Entity)
			iHit++;

		if ((int)(((float)iHit / 256) * 100.f) >= chance) {
			bHitchance = true;
			break;
		}

		if ((256 - 1 - i + iHit) < iHitsNeed)
			break;
	}

	return bHitchance;
}

void MiscFunc::AnimFix(ClientFrameStage_t curStage)
{
	if (I::Engine->IsInGame() && I::Engine->IsConnected())
	{
		for (int i = 1; i <= I::Engine->GetMaxClients(); ++i)
		{
			C_BaseEntity* Entity = I::EntityList->GetClientEntity(i);
			if (!Entity
				|| !Entity->IsAlive()
				|| Entity->IsDormant())
				continue;

			auto& AnimState = *reinterpret_cast<uintptr_t*>(uintptr_t(Entity) + ptrdiff_t(0x3884));
			if (!AnimState)
				continue;

			auto& GoalFeetYaw = *reinterpret_cast<float*>(uintptr_t(AnimState) + ptrdiff_t(0x80));

			static uintptr_t StoredAnimState[65];
			static float StoredSimTime[65];
			static std::array<float, 24> poses[64];
			static AnimationLayer StoredAnimLayer[65][15];
			int animcount = Entity->GetNumAnimOverlays();

			if (curStage == FRAME_RENDER_START)
			{
				if (Entity->GetSimulationTime() != StoredSimTime[i])
				{
					Entity->ClientSideAnimation() = true;
					Entity->UpdateClientAnimation();

					poses[i] = Entity->m_flPoseParameter();
					memcpy(StoredAnimLayer[i], Entity->GetAnimOverlayStruct(), (sizeof(AnimationLayer) * Entity->GetNumAnimOverlays()));

					StoredSimTime[i] = Entity->GetSimulationTime();
				}

				Entity->ClientSideAnimation() = false;

				Entity->m_flPoseParameter() = poses[i];
				std::memcpy(Entity->GetAnimOverlayStruct(), StoredAnimLayer[i], (sizeof(AnimationLayer) * Entity->GetNumAnimOverlays()));

				Entity->SetAbsAngles(Vector(0, GoalFeetYaw, 0));
			}
		}
	}
}

int MiscFunc::GetLerpTicks()
{
	static const auto cl_interp_ratio = I::CVar->FindVar("cl_interp_ratio");
	static const auto cl_updaterate = I::CVar->FindVar("cl_updaterate");
	static const auto cl_interp = I::CVar->FindVar("cl_interp");

	return TIME_TO_TICKS(max(cl_interp->GetFloat(), cl_interp_ratio->GetFloat() / cl_updaterate->GetFloat()));
}

float MiscFunc::FireRate(C_BaseCombatWeapon* pWeapon) // im retarded and m_flNextPrimaryAttack always crashed so i spent like 30 min doing this
{
	if (pWeapon)
	{
		std::string WeaponName = pWeapon->GetName();

		if (WeaponName == "weapon_glock")
			return 0.15f;
		else if (WeaponName == "weapon_hkp2000")
			return 0.169f;
		else if (WeaponName == "weapon_p250")//the cz and p250 have the same name idky same with other guns
			return 0.15f;
		else if (WeaponName == "weapon_tec9")
			return 0.12f;
		else if (WeaponName == "weapon_elite")
			return 0.12f;
		else if (WeaponName == "weapon_fiveseven")
			return 0.15f;
		else if (WeaponName == "weapon_deagle")
			return 0.224f;
		else if (WeaponName == "weapon_nova")
			return 0.882f;
		else if (WeaponName == "weapon_sawedoff")
			return 0.845f;
		else if (WeaponName == "weapon_mag7")
			return 0.845f;
		else if (WeaponName == "weapon_xm1014")
			return 0.35f;
		else if (WeaponName == "weapon_mac10")
			return 0.075f;
		else if (WeaponName == "weapon_ump45")
			return 0.089f;
		else if (WeaponName == "weapon_mp9")
			return 0.070f;
		else if (WeaponName == "weapon_bizon")
			return 0.08f;
		else if (WeaponName == "weapon_mp7")
			return 0.08f;
		else if (WeaponName == "weapon_p90")
			return 0.070f;
		else if (WeaponName == "weapon_galilar")
			return 0.089f;
		else if (WeaponName == "weapon_ak47")
			return 0.1f;
		else if (WeaponName == "weapon_sg556")
			return 0.089f;
		else if (WeaponName == "weapon_m4a1")
			return 0.089f;
		else if (WeaponName == "weapon_aug")
			return 0.089f;
		else if (WeaponName == "weapon_m249")
			return 0.08f;
		else if (WeaponName == "weapon_negev")
			return 0.0008f;
		else if (WeaponName == "weapon_ssg08")
			return 1.25f;
		else if (WeaponName == "weapon_awp")
			return 1.463f;
		else if (WeaponName == "weapon_g3sg1")
			return 0.25f;
		else if (WeaponName == "weapon_scar20")
			return 0.25f;
		else
			return 0;
	}
}

BYTE bMoveData[200];

void MiscFunc::EnginePrediction(CUserCmd* pCmd)
{
	float curtime = I::Globals->curtime;
	float frametime = I::Globals->frametime;

	I::Globals->curtime = (float)g_Globals->LocalPlayer->GetTickBase() * I::Globals->interval_per_tick;
	I::Globals->frametime = I::Globals->interval_per_tick;

	I::MoveHelper->SetHost(g_Globals->LocalPlayer);

	I::Prediction->SetupMove(g_Globals->LocalPlayer, pCmd, nullptr, bMoveData);
	I::GameMovement->ProcessMovement(g_Globals->LocalPlayer, bMoveData);
	I::Prediction->FinishMove(g_Globals->LocalPlayer, pCmd, bMoveData);

	// call predicted things here.

	I::MoveHelper->SetHost(0);
	I::Globals->curtime = curtime;
	I::Globals->frametime = frametime;
}

void MiscFunc::ThirdPerson(ClientFrameStage_t curStage)
{
	if (I::Engine->IsInGame() && I::Engine->IsConnected())
	{
		if (g_Globals->LocalPlayer)
		{
			static bool init = false;

			if (g_Vars->Misc.ThirdPerson && g_Globals->LocalPlayer->IsAlive())
			{
				if (init)
				{
					ConVar* sv_cheats = I::CVar->FindVar("sv_cheats");
					*(int*)((DWORD)&sv_cheats->fnChangeCallback + 0xC) = 0; // ew
					sv_cheats->SetValue(1);
					I::Engine->ExecuteClientCmd("thirdperson");
				}
				init = false;
			}
			else
			{
				if (!init)
				{
					ConVar* sv_cheats = I::CVar->FindVar("sv_cheats");
					*(int*)((DWORD)&sv_cheats->fnChangeCallback + 0xC) = 0; // ew
					sv_cheats->SetValue(1);
					I::Engine->ExecuteClientCmd("firstperson");
				}
				init = true;
			}

			if (curStage == FRAME_RENDER_START && g_Vars->Misc.ThirdPerson && g_Globals->LocalPlayer->IsAlive() && (g_Vars->HvH.Yaws > 0 || g_Vars->HvH.Pitch > 0)) //&& !g_Globals->LocalPlayer->IsKnifeorNade()
			{
				*(Vector*)((DWORD)g_Globals->LocalPlayer + 0x31C8) = g_Globals->RealAngle; // lol
			}
		}
	}
}

void MiscFunc::Spoof()//didnt work 2 gud
{
	static bool init = true;
	if (init)
	{
		ConVar* sv_cheats = I::CVar->FindVar("sv_cheats");
		*(int*)((DWORD)&sv_cheats->fnChangeCallback + 0xC) = 0;
		sv_cheats->SetValue(1);
		init = false;
	}
}

void MiscFunc::LoadNamedSky(const char *sky_name)
{
	static auto fnLoadNamedSkys = (void(__fastcall*)(const char*))g_Utils->FindPatternIDA("engine.dll", "55 8B EC 81 EC ? ? ? ? 56 57 8B F9 C7 45");

	fnLoadNamedSkys(sky_name);
}

IMaterial* MiscFunc::CreateMaterial() //credits to monarch
{
	IMaterial* materialFlatIgnoreZ;

	std::ofstream("csgo\\materials\\despacito.vmt") << R"#("UnlitGeneric"
{
  "$basetexture" "vgui/white_additive"
  "$ignorez"      "0"
  "$envmap"       ""
  "$nofog"        "1"
  "$model"        "1"
  "$nocull"       "0"
  "$selfillum"    "1"
  "$halflambert"  "1"
  "$znearer"      "0"
  "$flat"         "1"
  "$reflectivity" "[1 1 1]"
}
)#";

	materialFlatIgnoreZ = I::MaterialSystem->FindMaterial("despacito", "Model textures");
	std::remove("csgo\\materials\\despacito.vmt");

	return materialFlatIgnoreZ;
}

bool MiscFunc::VectortoVectorVisible(Vector src, Vector point)
{
	trace_t Trace;
	TraceLine(src, point, MASK_SOLID, g_Globals->LocalPlayer, &Trace);

	if (Trace.fraction == 1.0f)
	{
		return true;
	}

	return false;

}