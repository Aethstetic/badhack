#include "Main.h"

void Resolver::Resolve(C_BaseEntity* Entity)
{
	int index = Entity->EntIndex();
	static Vector Angle;
	Angle = Entity->GetEyeAngles();

	static float moving_lby[65];
	static float moving_sim[65];
	static float stored_lby[65];
	static float old_lby[65];
	static float lby_delta[65];
	static float predicted_yaw[65];
	static bool lby_changes[65];
	static int shots_check[65];
	static float angle_brute[65];
	static float AtTargetAngle;
	static float FixPitch;
	static float FixPitch2;
	static bool HitNS[65];
	static Vector StoredAngle[65];
	static Vector Hitstored[65];
	static int StoredShots[65];
	static int HitShots[65];
	static int HitShotsStored[65];

	if (stored_lby[index] != Entity->GetLowerBodyYaw())
	{
		old_lby[index] = stored_lby[index];
		lby_changes[index] = true;
		stored_lby[index] = Entity->GetLowerBodyYaw();
	}

	lby_delta[index] = g_Math->NormalizeYaw(stored_lby[index] - old_lby[index]);

	if (lby_changes[index])
	{
		if ((Entity->GetSimulationTime() - moving_sim[index]) > .22f)
			predicted_yaw[index] = lby_delta[index];

		lby_changes[index] = false;
	}

	if (stored_lby[index] != Entity->GetLowerBodyYaw())
	{
		old_lby[index] = stored_lby[index];
		Angle.y = Entity->GetLowerBodyYaw();
		lby_changes[index] = true;
		stored_lby[index] = Entity->GetLowerBodyYaw();
		g_Globals->ResolverStage[index] = "LBY UPDATE";// STACKER DONT TOUCH THE RESOLVER STAGES OR I WILL BANN U
	}
	else if (abs(Entity->GetVelocity().Length2D()) > 29.f && (Entity->GetFlags() & FL_ONGROUND))
	{
		Angle.y = Entity->GetLowerBodyYaw();
		moving_lby[index] = Entity->GetLowerBodyYaw();
		moving_sim[index] = Entity->GetSimulationTime();
		lby_changes[index] = false;
		predicted_yaw[index] = 0;
		g_Globals->MissedShotsResolve[index] = 0;
		angle_brute[index] = 0;
		g_Globals->ResolverStage[index] = "MOVING RESOLVE";// STACKER DONT TOUCH THE RESOLVER STAGES OR I WILL BANN U
	}
	else if ((Entity->GetFlags() & FL_ONGROUND))
	{
		if (shots_check[index] != g_Globals->MissedShotsResolve[index])
		{
			angle_brute[index] += predicted_yaw[index];
			shots_check[index] = g_Globals->MissedShotsResolve[index];
		}

		Angle.y = g_Math->NormalizeYaw(angle_brute[index] + moving_lby[index]);
		g_Globals->ResolverStage[index] = "MOVING-LBY + " + std::to_string(angle_brute[index]);// STACKER DONT TOUCH THE RESOLVER STAGES OR I WILL BANN U
	}
	else
	{
		Angle.y = Entity->GetLowerBodyYaw();
		g_Globals->ResolverStage[index] = "AIR RESOLVE";// STACKER DONT TOUCH THE RESOLVER STAGES OR I WILL BANN U
	}

	Entity->SetEyeAngles(Angle);
}

void Resolver::Apply(ClientFrameStage_t curStage)
{
	if (I::Engine->IsInGame() && I::Engine->IsConnected()) {
		for (int i = 1; i <= I::Engine->GetMaxClients(); ++i) {
			C_BaseEntity* Entity = I::EntityList->GetClientEntity(i);
			if (!Entity
				|| !Entity->IsAlive()
				|| Entity->IsDormant()
				|| Entity == g_Globals->LocalPlayer)
				continue;

			if (curStage == FRAME_NET_UPDATE_POSTDATAUPDATE_START && g_Globals->LocalPlayer->IsAlive() && Entity->GetTeam() != g_Globals->LocalPlayer->GetTeam())
			{
				Resolve(Entity);
			}
			else if (curStage == FRAME_NET_UPDATE_END)
			{
				auto VarMap = reinterpret_cast<uintptr_t>(Entity) + 36;
				auto VarMapSize = *reinterpret_cast<int*>(VarMap + 20);

				for (auto index = 0; index < VarMapSize; index++)
					*reinterpret_cast<uintptr_t*>(*reinterpret_cast<uintptr_t*>(VarMap) + index * 12) = 0; // bameware
			}
		}
	}
}