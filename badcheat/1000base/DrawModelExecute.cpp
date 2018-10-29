#include "Main.h"

DrawModelExecuteFn oDrawModelExecute;
void __stdcall Hooks::DrawModelExecute(void* context, void* state, const ModelRenderInfo_t &pInfo, matrix3x4_t *pCustomBoneToWorld)
{
	if (!pInfo.pModel || !I::Engine->IsInGame())
	{
		oDrawModelExecute(context, state, pInfo, pCustomBoneToWorld);
		return;
	}
	std::string model_name = I::ModelInfo->GetModelName((model_t*)pInfo.pModel);

	static IMaterial* IgnoreZ = g_Chams->CreateMaterial(true, false, false);
	static IMaterial* NotIgnoreZ = g_Chams->CreateMaterial(false, false, false);
	float color[4] = { 0.f, 0.f, 0.f, .9f };

	if (model_name.find("models/player") != std::string::npos && IgnoreZ && NotIgnoreZ)
	{
		auto entity = I::EntityList->GetClientEntity(pInfo.entity_index);
		if (!entity || !entity->IsAlive() || entity->GetTeam() == g_Globals->LocalPlayer->GetTeam())
			return;

		auto& records = g_Backtrack->PlayerRecords[entity->EntIndex()];
		if (records.empty())
			return;

		matrix3x4_t matrix[128];
		std::memcpy(matrix, records.front().matrix, sizeof(records.front().matrix));

		I::RenderView->SetColorModulation(color);
		I::RenderView->SetBlend(0.9f);
		I::ModelRender->ForcedMaterialOverride(IgnoreZ);
		oDrawModelExecute(context, state, pInfo, matrix);

		I::RenderView->SetColorModulation(color);
		I::RenderView->SetBlend(0.9f);
		I::ModelRender->ForcedMaterialOverride(NotIgnoreZ);
	}

	oDrawModelExecute(context, state, pInfo, pCustomBoneToWorld);
	I::ModelRender->ForcedMaterialOverride(nullptr);
}