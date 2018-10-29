#include "Main.h"

static auto linegoesthrusmoke = g_Utils->FindPatternIDA("client_panorama.dll", "55 8B EC 83 EC 08 8B 15 ? ? ? ? 0F 57 C0");

SceneEndFn oSceneEnd;
void __fastcall Hooks::SceneEnd(void *ecx, void *edx) 
{
	if (!g_Globals->LocalPlayer || !I::Engine->IsInGame() || !I::Engine->IsConnected())
		return oSceneEnd(ecx);

	oSceneEnd(ecx); //call original

	g_Chams->ApplyChams((C_BaseEntity*)(edx)); //render chams

	g_Glow->RenderGlow(); //render glow

	//no smoke
	if (I::Engine->IsInGame())
	{
		std::vector<const char*> dontdraw =
		{
			"particle/vistasmokev1/vistasmokev1_fire",
			"particle/vistasmokev1/vistasmokev1_emods",
			"particle/vistasmokev1/vistasmokev1_emods_impactdust",
			"particle/vistasmokev1/vistasmokev1_smokegrenade"
		};

		for (auto mat_n : dontdraw)
		{
			IMaterial* mat = I::MaterialSystem->FindMaterial(mat_n, "Model textures");
			mat->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, true);
		}

		static auto smokecout = *(DWORD*)(linegoesthrusmoke + 0x8);
		*(int*)(smokecout) = 0;
	}
}