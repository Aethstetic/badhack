#pragma once

struct LayerRecord
{
	LayerRecord()
	{
		order = 0;
		sequence = 0;
		weight = 0.f;
		cycle = 0.f;
	}

	LayerRecord(const LayerRecord &src)
	{
		order = src.order;
		sequence = src.sequence;
		weight = src.weight;
		cycle = src.cycle;
	}

	int order;
	int sequence;
	float weight;
	float cycle;
};

struct LagData
{
	LagData() {
		max.Init();
		mins.Init();
		angles.Init();
		origin.Init();
		velocity.Init();
		abs_origin.Init();
		render_angles.Init();
		lby = 0.0f;
		simtime = 0.0f;
		trace_length = 0.0f;
		priority = 0;
		matrix_built = false;
	}

	void SaveRecord(C_BaseEntity *player);

	float lby;
	float simtime;
	float trace_length;

	int priority;

	bool matrix_built;

	Vector max;
	Vector mins;
	Vector angles;
	Vector origin;
	Vector velocity;
	Vector abs_origin;
	Vector render_angles;

	EntityFlags flags;
	matrix3x4_t	matrix[128];

	std::array<float_t, 24> poses;
	std::array<LayerRecord, 15> layers;
};

class Backtracking {

public:

	void StoreRecords();
	bool IsTickValid(int tick);
	void RestoreInfo(C_BaseEntity* player, LagData record);
	bool FillAimInfo(C_BaseEntity* player);

	std::pair<LagData, LagData> RestoreRecord[64];
	std::deque<LagData> PlayerRecords[64];
	std::deque<LagData> AimRecords;

private:

	int GetRecordPriority(C_BaseEntity *player, LagData* lag_record);
	bool ValidPlayer(C_BaseEntity *player);
	void CleanRecords(int Idx, std::deque<LagData>& records);
	float NextUpdate[64];
	bool FirstUpdate[64], SawUpdate[64];
};
extern Backtracking* g_Backtrack;