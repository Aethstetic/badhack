#include "Main.h"

void LagData::SaveRecord(C_BaseEntity *Entity)
{
	int i = Entity->EntIndex();

	origin = Entity->GetOrigin();
	abs_origin = Entity->GetAbsOrigin();
	angles = Entity->GetEyeAngles();
	simtime = Entity->GetSimulationTime();
	mins = Entity->GetCollideable()->OBBMins();
	max = Entity->GetCollideable()->OBBMaxs();
	flags = Entity->GetFlags();
	velocity = Entity->GetVelocity();

	int layerCount = Entity->GetNumAnimOverlays();
	for (int i = 0; i < layerCount; i++)
	{
		AnimationLayer current_layer = *Entity->GetAnimOverlay(i);
		layers[i].order = current_layer.m_nOrder;
		layers[i].sequence = current_layer.m_nSequence;
		layers[i].weight = current_layer.m_flWeight;
		layers[i].cycle = current_layer.m_flCycle;
	}

	poses = Entity->m_flPoseParameter();

	Ray_t ray;
	trace_t trace;
	CTraceFilter filter;
	filter.type = TraceType::TRACE_WORLD_ONLY;

	ray.Init(Entity->GetOrigin(), g_Globals->LocalPlayer->GetEyePosition());
	I::Trace->TraceRay(ray, MASK_ALL, &filter, &trace);
	trace_length = trace.fraction;	
}

void Backtracking::StoreRecords()
{
	static auto sv_unlag = I::CVar->FindVar("sv_unlag");
	if (I::Globals->maxClients <= 1)
	{
		for (int i = 0; i < 64; i++)
			PlayerRecords[i].clear();

		return;
	}

	for (auto i = 1; i <= I::Globals->maxClients; i++)
	{
		C_BaseEntity* entity = I::EntityList->GetClientEntity(i);

		auto &Records = PlayerRecords[i];
		if (!ValidPlayer(entity))
			continue;

		/* clear old records */
		CleanRecords(i, Records);

		if (entity->IsDormant() || !entity->IsAlive())
		{
			FirstUpdate[i] = false;
			SawUpdate[i] = false;
			continue;
		}

		LagData CurTick;
		if (Records.size() > 0 && Records.back().simtime == entity->GetSimulationTime())
			continue;

		/* save info */
		CurTick.SaveRecord(entity);

		/* setup bones and fix pvs */
		*reinterpret_cast<int*>(uintptr_t(entity) + 0xA30) = I::Globals->framecount;
		*reinterpret_cast<int*>(uintptr_t(entity) + 0xA28) = 0;

		entity->InvalidateBoneCache();
		entity->SetupBones(CurTick.matrix, 128, 256, CurTick.simtime);

		/* add the records */
		Records.push_back(CurTick);
	}
}

int Backtracking::GetRecordPriority(C_BaseEntity *Entity, LagData* lag_record)
{
	int i = Entity->EntIndex();
	int priority = 0;

	static float old_lby[64];
	if (Entity->GetLowerBodyYaw() != old_lby[i] || lag_record->simtime >= NextUpdate[i] && SawUpdate[i])
	{
		if (!FirstUpdate[i] && Entity->GetLowerBodyYaw() != old_lby[i]) 
			FirstUpdate[i] = true;
		else 
		{
			if (Entity->GetLowerBodyYaw() != old_lby[i]) 
				priority = 1;
			else if (TIME_TO_TICKS(Entity->GetSimulationTime() - lag_record->simtime) <= 11 && Entity->GetFlags() & FL_ONGROUND)
				priority = 2;

			lag_record->angles.y = Entity->GetLowerBodyYaw();
			NextUpdate[i] = lag_record->simtime + 1.1f + I::Globals->interval_per_tick;
			SawUpdate[i] = true;

			// need this for lby timer.
			//can_predict_lby[i] = true;
		}
	}

	old_lby[i] = Entity->GetLowerBodyYaw();
	return priority;
}

bool Backtracking::FillAimInfo(C_BaseEntity* Entity)
{
	// clear records so we can re-fill them
	auto& Records = PlayerRecords[Entity->EntIndex()];
	AimRecords.clear();

	// save current player data for restoring
	RestoreRecord[Entity->EntIndex()].second.SaveRecord(Entity);

	LagData least_thick = LagData();
	LagData oldest_record = LagData();
	for (auto it : Records)
	{
		if (it.priority > 0 && AimRecords.empty()) 
			AimRecords.emplace_back(it);

		if (it.trace_length <= least_thick.trace_length)
			least_thick = it;
	}

	//if (AimRecords.size() == 0)
	//	AimRecords.push_back(Records.back());

	//if (AimRecords.size() == 1)
		AimRecords.emplace_back(least_thick);

	//if (AimRecords.size() == 2)
		AimRecords.push_back(Records.front());

	// sort the records by their priority level.
	std::sort(AimRecords.begin(), AimRecords.end(), [](LagData const &a, LagData const &b) { return a.priority < b.priority; });
	return AimRecords.size() > 0;
}

void Backtracking::RestoreInfo(C_BaseEntity* Entity, LagData record)
{
	Entity->m_flPoseParameter() = record.poses;
	int layerCount = Entity->GetNumAnimOverlays();
	for (int i = 0; i < layerCount; ++i)
	{
		AnimationLayer current_layer = *Entity->GetAnimOverlay(i);
		current_layer.m_nOrder = record.layers[i].order;
		current_layer.m_nSequence = record.layers[i].sequence;
		current_layer.m_flWeight = record.layers[i].weight;
		current_layer.m_flCycle = record.layers[i].cycle;
	}

	Entity->SetAbsOrigin(record.origin);
	Entity->GetVelocity() = record.velocity;
	Entity->GetFlags() = record.flags;
	Entity->GetCollideable()->OBBMins() = record.mins;
	Entity->GetCollideable()->OBBMaxs() = record.max;

	Entity->InvalidateBoneCache();
}

void Backtracking::CleanRecords(int Idx, std::deque<LagData>& records)
{
	auto& Records = records; // Should use rbegin but can't erase
	for (auto backtrack_info = Records.begin(); backtrack_info != Records.end(); backtrack_info++)
	{
		if (!IsTickValid(TIME_TO_TICKS(backtrack_info->simtime)))
		{
			Records.erase(backtrack_info);

			if (!Records.empty())
				backtrack_info = Records.begin();

			else break;
		}
	}
}

bool Backtracking::IsTickValid(int Tick)
{
	float Correct = 0;
	Correct += I::Engine->GetNetChannelInfo()->GetLatency(FLOW_OUTGOING);
	Correct += TICKS_TO_TIME(g_MiscFunc->GetLerpTicks());
	Correct = clamp(Correct, 0, 1);

	auto LagDelta = Correct - (I::Globals->curtime - TICKS_TO_TIME(Tick));
	return abs(LagDelta) < .15f;
}

bool Backtracking::ValidPlayer(C_BaseEntity *Entity)
{
	if (!Entity || !g_Globals->LocalPlayer)
		return false;

	if (Entity->HasImmunity())
		return false;

	if (Entity == g_Globals->LocalPlayer)
		return false;

	if (Entity->GetTeam() == g_Globals->LocalPlayer->GetTeam())
		return false;

	return true;
}