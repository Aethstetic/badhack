#pragma once
#include "Main.h"

class Chams 
{
public:
	void InitKeyValues(KeyValues* kv_, std::string name_)
	{
		auto client_string = GetModuleHandle("client_panorama.dll") ? "client_panorama.dll" : "client.dll";
		static auto address = g_Utils->FindPatternIDA(client_string, "55 8B EC 51 33 C0 C7 45");
		using Fn = void(__thiscall*)(void* thisptr, const char* name);
		reinterpret_cast<Fn>(address)(kv_, name_.c_str());
	}

	void LoadFromBuffer(KeyValues* vk_, std::string name_, std::string buffer_)
	{
		auto client_string = GetModuleHandle("client_panorama.dll") ? "client_panorama.dll" : "client.dll";
		static auto address = g_Utils->FindPatternIDA(client_string, "55 8B EC 83 E4 F8 83 EC 34 53 8B 5D 0C 89 4C 24 04");
		using Fn = void(__thiscall*)(void* thisptr, const char* resourceName, const char* pBuffer, void* pFileSystem, const char* pPathID, void* pfnEvaluateSymbolProc);
		reinterpret_cast<Fn>(address)(vk_, name_.c_str(), buffer_.c_str(), nullptr, nullptr, nullptr);
	}

	IMaterial* CreateMaterial(bool ignorez, bool lit, bool wireframe)
	{
		static auto created = 0;
		std::string type = lit ? "VertexLitGeneric" : "UnlitGeneric";

		std::string matdata;

		matdata =
		"\"" + type + "\"\
		\n{\
		\n\t\"$basetexture\" \"vgui/white_additive\"\
		\n\t\"$model\" \"1\"\
		\n\t\"$flat\" \"0\"\
		\n\t\"$nocull\" \"0\"\
		\n\t\"$selfillum\" \"1\"\
		\n\t\"$halflambert\" \"1\"\
		\n\t\"$nofog\" \"1\"\
		\n\t\"$ignorez\" \"" + std::to_string(ignorez) + "\"\
		\n\t\"$znearer\" \"0\"\
		\n\t\"$wireframe\" \"" + std::to_string(wireframe) + "\"\
        \n}\n";

		auto matname = "custom_" + std::to_string(created);
		++created;
		auto keyValues = static_cast<KeyValues*>(malloc(sizeof(KeyValues)));
		InitKeyValues(keyValues, type.c_str());
		LoadFromBuffer(keyValues, matname, matdata);
		auto material = I::MaterialSystem->CreateMaterial(matname.c_str(), keyValues);
		material->IncrementReferenceCount();
		return material;
	}

	/*IMaterial* CreateMaterial(bool ignorez, bool lit, bool wireframe, bool shiny)
	{
		IMaterial* material = nullptr; //VertexLitGeneric
		std::ofstream("csgo\\materials\\yayeet.vmt") << R"#("VertexLitGeneric"
		{
			"$basetexture" "vgui/white_additive"
			"$ignorez"      "1"
			"$envmap"       ""
			"$nofog"        "1"
			"$model"        "1"
			"$nocull"       "0"
			"$selfillum"    "1"
			"$halflambert"  "1"
			"$znearer"      "0"
			"$flat"         "0"
			"$reflectivity" "[1 1 1]"
		}
		)#";
		return I::MaterialSystem->FindMaterial("yayeet", "Model textures");
		std::remove("csgo\\materials\\yayeet.vmt");
	}*/

	void ApplyChams(C_BaseEntity* ent)
	{
		if (g_Vars->Visuals.Chams) 
		{
			// ------------ textured ------------ //
			static IMaterial* IgnorezTextured = CreateMaterial(true, true, false);
			static IMaterial* NotIgnorezTextured = CreateMaterial(false, true, false);

			IMaterial* used_material = nullptr;
			IMaterial* used_material_ignorez = nullptr;

			used_material_ignorez = IgnorezTextured;
			used_material = NotIgnorezTextured;

			if (!IgnorezTextured || !NotIgnorezTextured)
				return;

			for (int i = 1; i <= 64; ++i) 
			{
				auto ent = static_cast<C_BaseEntity*>(I::EntityList->GetClientEntity(i));

				if (!ent || !g_Globals->LocalPlayer || !ent->IsAlive() || ent->IsDormant())
					continue;

				if (ent->GetTeam() == g_Globals->LocalPlayer->GetTeam())
					continue;

				float invisible[4] = { .235f, .47f, .705f, 1.f };

				used_material_ignorez->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, true);
				I::RenderView->SetColorModulation(invisible);
				I::ModelRender->ForcedMaterialOverride(used_material_ignorez);
				ent->DrawModel(0x1, 255);
				I::ModelRender->ForcedMaterialOverride(nullptr);

				float visible[4] = { .588f, .784f, .235f, 1.f };

				used_material->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);
				I::RenderView->SetColorModulation(visible);
				I::ModelRender->ForcedMaterialOverride(used_material);
				ent->DrawModel(0x1, 255);
				I::ModelRender->ForcedMaterialOverride(nullptr);
			}
		}
	}
}; extern Chams* g_Chams;