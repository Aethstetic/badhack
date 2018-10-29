#pragma once

#include "CHandle.h"
#include "IClientNetworkable.h"
#include "IClientUnknown.h"
#include "IClientRenderable.h"
#include "IClientThinkable.h"
#include "IBaseClientDLL.h"
#include "IVEngineClient.h"
#include "IStudioRender.h"
#include "IEntity.h"
#include "ISurface.h"
#include "SurfaceData.h"
#include "IEngineTrace.h"
#include "IPrediction.h"
#include "IMoveHelper.h"
#include "IGameMovement.h"
#include "IVModelInfo.h"
#include "IGameEvents.h"
#include "CRenderView.h"
#include "IMaterialSystem.h"
#include "IVModelRender.h"
#include "IDebugOverlay.h"
#include "AnimationList.h"
#include "GlowManager.h"
#include "CVar.h"
#include "IRenderBeams.h"

namespace I {
	extern IBaseClientDLL* Client;
	extern IVEngineClient* Engine;
	extern IClientEntityList* EntityList;
	extern IGlobalVars* Globals;
	extern IClientMode* ClientMode;
	extern ISurface* Surface;
	extern IPanel* Panel;
	extern IVModelInfo* ModelInfo;
	extern IPhysicsSurfaceProps* PhysicsProps;
	extern IEngineTrace* Trace;
	extern ICVar* CVar;
	extern IPrediction* Prediction;
	extern IMoveHelper* MoveHelper;
	extern IGameMovement* GameMovement;
	extern IGameEventManager* GameEvent;
	extern IMaterialSystem* MaterialSystem;
	extern CRenderView* RenderView;
	extern IVModelRender* ModelRender;
	extern CUtlVectorSimple* AnimationList;
	extern IVDebugOverlay* DebugOverlay;
	extern CGlowObjectManager* GlowManager;
	extern IViewRenderBeams* RenderBeams;
}

class Interfaces {
public:
	template< class T > T get_interface(std::string szModuleName, std::string szInterfaceName, bool bSkip = false)
	{
		if (szModuleName.empty() || szInterfaceName.empty())
			return nullptr;
		typedef PVOID(*CreateInterfaceFn)(const char* pszName, int* piReturnCode);
		CreateInterfaceFn hInterface = nullptr;
		while (!hInterface)
		{
			hInterface = (CreateInterfaceFn)GetProcAddress(GetModuleHandleA(szModuleName.c_str()), "CreateInterface");
			Sleep(5);
		}

		char pszBuffer[256];
		for (int i = 0; i < 100; i++)
		{
			sprintf_s(pszBuffer, "%s%0.3d", szInterfaceName.c_str(), i);
			PVOID pInterface = hInterface(pszBuffer, nullptr);

			if (pInterface && pInterface != NULL)
			{
				if (bSkip)
					sprintf_s(pszBuffer, "%s%0.3d", szInterfaceName.c_str(), i + 1);

				Sleep(5);
				break;
			}
		}

		return (T)hInterface(pszBuffer, nullptr);
	}

	template< class T > T get_interface2(std::string szModuleName, std::string szInterfaceName)
	{
		if (szModuleName.empty() || szInterfaceName.empty())
			return nullptr;
		typedef PVOID(*CreateInterfaceFn)(const char* pszName, int* piReturnCode);
		CreateInterfaceFn hInterface = nullptr;
		while (!hInterface)
		{
			hInterface = (CreateInterfaceFn)GetProcAddress(GetModuleHandleA(szModuleName.c_str()), "CreateInterface");
			Sleep(5);
		}

		return (T)hInterface(szInterfaceName.c_str(), nullptr);
	}

	void InitInterfaces();

private:
}; extern Interfaces* g_Interfaces;