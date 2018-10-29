#include "Main.h"

void Utils::InitHooks()
{
	std::unique_ptr<CVMTHookManager> ClientMode;
	ClientMode = std::make_unique<CVMTHookManager>(I::ClientMode);
	std::unique_ptr<CVMTHookManager> Surface;
	Surface = std::make_unique<CVMTHookManager>(I::Surface);
	std::unique_ptr<CVMTHookManager> Panel;
	Panel = std::make_unique<CVMTHookManager>(I::Panel);
	std::unique_ptr<CVMTHookManager> Client;
	Client = std::make_unique<CVMTHookManager>(I::Client);
	std::unique_ptr<CVMTHookManager> GameEvent;
	GameEvent = std::make_unique<CVMTHookManager>(I::GameEvent);
	std::unique_ptr<CVMTHookManager> RenderView;
	RenderView = std::make_unique<CVMTHookManager>(I::RenderView);
	//std::unique_ptr<CVMTHookManager> ModelRender;
	//ModelRender = std::make_unique<CVMTHookManager>(I::ModelRender);

	oLockCursor = (LockCursorFn)Surface->dwHookMethod((DWORD)Hooks::LockCursor, 67);
	oCreateMove = (CreateMoveFn)ClientMode->dwHookMethod((DWORD)Hooks::CreateMove, 24);
	oPaintTraverse = (PaintTraverseFn)Panel->dwHookMethod((DWORD)Hooks::PaintTraverse, 41);
	oFrameStageNotify = (FrameStageNotifyFn)Client->dwHookMethod((DWORD)Hooks::FrameStageNotify, 37);
	oFireEventClientSide = (FireEventClientSideFn)GameEvent->dwHookMethod((DWORD)Hooks::FireEventClientSide, 9);
	oSceneEnd = (SceneEndFn)RenderView->dwHookMethod((DWORD)Hooks::SceneEnd, 9);
	//oDrawModelExecute = (DrawModelExecuteFn)ModelRender->dwHookMethod((DWORD)Hooks::DrawModelExecute, 21);
}

bool Utils::WorldToScreen(const Vector &origin, Vector &screen)
{
	const auto screenTransform = [&origin, &screen]() -> bool
	{
		static std::uintptr_t pViewMatrix;
		if (!pViewMatrix)
		{
			pViewMatrix = static_cast<std::uintptr_t>(g_Utils->FindPatternIDA("client_panorama.dll", "0F 10 05 ? ? ? ? 8D 85 ? ? ? ? B9"));
			pViewMatrix += 3;
			pViewMatrix = *reinterpret_cast<std::uintptr_t*>(pViewMatrix);
			pViewMatrix += 176;
		}

		const VMatrix& w2sMatrix = *reinterpret_cast<VMatrix*>(pViewMatrix);
		screen.x = w2sMatrix.m[0][0] * origin.x + w2sMatrix.m[0][1] * origin.y + w2sMatrix.m[0][2] * origin.z + w2sMatrix.m[0][3];
		screen.y = w2sMatrix.m[1][0] * origin.x + w2sMatrix.m[1][1] * origin.y + w2sMatrix.m[1][2] * origin.z + w2sMatrix.m[1][3];
		screen.z = 0.0f;

		float w = w2sMatrix.m[3][0] * origin.x + w2sMatrix.m[3][1] * origin.y + w2sMatrix.m[3][2] * origin.z + w2sMatrix.m[3][3];

		if (w < 0.001f)
		{
			screen.x *= 100000;
			screen.y *= 100000;
			return true;
		}

		float invw = 1.f / w;
		screen.x *= invw;
		screen.y *= invw;

		return false;
	};

	if (!screenTransform())
	{
		int iScreenWidth, iScreenHeight;
		I::Engine->GetScreenSize(iScreenWidth, iScreenHeight);

		screen.x = (iScreenWidth * 0.5f) + (screen.x * iScreenWidth) * 0.5f;
		screen.y = (iScreenHeight * 0.5f) - (screen.y * iScreenHeight) * 0.5f;

		return true;
	}
	return false;
}

DWORD Utils::FindPatternIDA(std::string moduleName, std::string pattern)
{
	const char* pat = pattern.c_str();
	DWORD firstMatch = 0;
	DWORD rangeStart = (DWORD)GetModuleHandleA(moduleName.c_str());
	MODULEINFO miModInfo; GetModuleInformation(GetCurrentProcess(), (HMODULE)rangeStart, &miModInfo, sizeof(MODULEINFO));
	DWORD rangeEnd = rangeStart + miModInfo.SizeOfImage;
	for (DWORD pCur = rangeStart; pCur < rangeEnd; pCur++)
	{
		if (!*pat)
			return firstMatch;

		if (*(PBYTE)pat == '\?' || *(BYTE*)pCur == getByte(pat))
		{
			if (!firstMatch)
				firstMatch = pCur;

			if (!pat[2])
				return firstMatch;

			if (*(PWORD)pat == '\?\?' || *(PBYTE)pat != '\?')
				pat += 3;

			else
				pat += 2;    //one ?
		}
		else
		{
			pat = pattern.c_str();
			firstMatch = 0;
		}
	}
	return NULL;
}

unsigned int Utils::FindPattern(const char* module_name, const BYTE* mask, const char* mask_string)
{
	/// Get module address
	const unsigned int module_address = reinterpret_cast<unsigned int>(GetModuleHandle(module_name));

	/// Get module information to the size
	MODULEINFO module_info;
	GetModuleInformation(GetCurrentProcess(), reinterpret_cast<HMODULE>(module_address), &module_info, sizeof(MODULEINFO));

	auto IsCorrectMask = [](const unsigned char* data, const unsigned char* mask, const char* mask_string) -> bool
	{
		for (; *mask_string; ++mask_string, ++mask, ++data)
			if (*mask_string == 'x' && *mask != *data)
				return false;

		return (*mask_string) == 0;
	};

	/// Iterate until we find a matching mask
	for (unsigned int c = 0; c < module_info.SizeOfImage; c += 1)
	{
		/// does it match?
		if (IsCorrectMask(reinterpret_cast<unsigned char*>(module_address + c), mask, mask_string))
			return (module_address + c);
	}

	return 0;
}