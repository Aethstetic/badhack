#include "Main.h"

IBaseClientDLL* I::Client = nullptr;
IVEngineClient* I::Engine = nullptr;
IClientEntityList* I::EntityList = nullptr;
IGlobalVars* I::Globals = nullptr;
IClientMode* I::ClientMode = nullptr;
ISurface* I::Surface = nullptr;
IPanel* I::Panel = nullptr;
IVModelInfo* I::ModelInfo = nullptr;
IPhysicsSurfaceProps* I::PhysicsProps = nullptr;
IEngineTrace* I::Trace = nullptr;
ICVar* I::CVar = nullptr;
IPrediction* I::Prediction = nullptr;
IMoveHelper* I::MoveHelper = nullptr;
IGameMovement* I::GameMovement = nullptr;
IGameEventManager* I::GameEvent = nullptr;
IMaterialSystem* I::MaterialSystem = nullptr;
CRenderView* I::RenderView = nullptr;
IVModelRender* I::ModelRender = nullptr;
CUtlVectorSimple* I::AnimationList = nullptr;
IVDebugOverlay* I::DebugOverlay = nullptr;
CGlowObjectManager* I::GlowManager = nullptr;
IViewRenderBeams* I::RenderBeams = nullptr;

void Interfaces::InitInterfaces()
{
	I::Client = g_Interfaces->get_interface<IBaseClientDLL*>("client_panorama.dll", "VClient");
	I::Engine = g_Interfaces->get_interface<IVEngineClient*>("engine.dll", "VEngineClient");
	I::EntityList = g_Interfaces->get_interface<IClientEntityList*>("client_panorama.dll", "VClientEntityList");
	I::Surface = g_Interfaces->get_interface<ISurface*>("vguimatsurface.dll", "VGUI_Surface");
	I::Prediction = g_Interfaces->get_interface<IPrediction*>("client_panorama.dll", "VClientPrediction");
	I::Panel = g_Interfaces->get_interface<IPanel*>("vgui2.dll", "VGUI_Panel");
	I::ModelInfo = g_Interfaces->get_interface<IVModelInfo*>("engine.dll", "VModelInfoClient");
	I::PhysicsProps = g_Interfaces->get_interface<IPhysicsSurfaceProps*>("vphysics.dll", "VPhysicsSurfaceProps");
	I::Trace = g_Interfaces->get_interface<IEngineTrace*>("engine.dll", "EngineTraceClient");
	I::CVar = g_Interfaces->get_interface<ICVar*>("vstdlib.dll", "VEngineCvar");
	I::MoveHelper = **reinterpret_cast<IMoveHelper***>((g_Utils->FindPattern("client_panorama.dll", (PBYTE)"\x8B\x0D\x00\x00\x00\x00\x8B\x46\x08\x68", "xx????xxxx") + 2)); // bameware
	I::GameMovement = g_Interfaces->get_interface<IGameMovement*>("client_panorama.dll", "GameMovement");
	I::GameEvent = g_Interfaces->get_interface2<IGameEventManager*>("engine.dll", "GAMEEVENTSMANAGER002");
	I::MaterialSystem = g_Interfaces->get_interface<IMaterialSystem*>("materialsystem.dll", "VMaterialSystem");
	I::RenderView = g_Interfaces->get_interface<CRenderView*>("engine.dll", "VEngineRenderView");
	I::ModelRender = g_Interfaces->get_interface<IVModelRender*>("engine.dll", "VEngineModel");
	I::GlowManager = *reinterpret_cast<CGlowObjectManager**>(g_Utils->FindPatternIDA("client_panorama.dll", "0F 11 05 ? ? ? ? 83 C8 01") + 3);
	I::DebugOverlay = g_Interfaces->get_interface<IVDebugOverlay*>("client_panorama.dll", "VDebugOverlay");

	I::RenderBeams = *(IViewRenderBeams**)(g_Utils->FindPatternIDA("client_panorama.dll", "A1 ? ? ? ? FF 10 A1 ? ? ? ? B9") + 0x1);

	I::ClientMode = **reinterpret_cast<IClientMode***> ((*reinterpret_cast<uintptr_t**>(I::Client))[10] + 0x5u);
	I::Globals = **reinterpret_cast<IGlobalVars***>((*reinterpret_cast<uintptr_t**>(I::Client))[0] + 0x1Bu);
	//I::AnimationList = **reinterpret_cast<CUtlVectorSimple***>(g_Utils->FindPatternIDA("client_panorama.dll", "A1 ?? ?? ?? ?? F6 44 F0 04 01 74 0B")); // mutiny
}