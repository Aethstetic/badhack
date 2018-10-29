#include "Main.h"
#include "Listener.h"

enum EFontFlags
{
	FONTFLAG_NONE,
	FONTFLAG_ITALIC = 0x001,
	FONTFLAG_UNDERLINE = 0x002,
	FONTFLAG_STRIKEOUT = 0x004,
	FONTFLAG_SYMBOL = 0x008,
	FONTFLAG_ANTIALIAS = 0x010,
	FONTFLAG_GAUSSIANBLUR = 0x020,
	FONTFLAG_ROTARY = 0x040,
	FONTFLAG_DROPSHADOW = 0x080,
	FONTFLAG_ADDITIVE = 0x100,
	FONTFLAG_OUTLINE = 0x200,
	FONTFLAG_CUSTOM = 0x400,
	FONTFLAG_BITMAP = 0x800,
};

void Initialize() {
	while (!(g_Globals->CSGOWindow = FindWindowA("Valve001", NULL))) // Get CSGO window handle into global vars
		std::this_thread::sleep_for(std::chrono::milliseconds(200));

	g_Interfaces->InitInterfaces(); // initialization of interfaces
	g_pNetvars = new CNetVars(); // initialization of NetVar manager
	g_Utils->InitHooks(); // initialization of hooks
	CGameEvents::InitializeEventListeners(); // initialization of event listeners

	// initialization of fonts
	g_Globals->Espfont = I::Surface->FontCreate();
	g_Globals->Menufont = I::Surface->FontCreate();
	g_Globals->Titlefont = I::Surface->FontCreate();
	g_Globals->Namefont = I::Surface->FontCreate();

	I::Surface->SetFontGlyphSet(g_Globals->Espfont, "Small Fonts", 8, 400, 0, 0, FONTFLAG_OUTLINE); //	I::Surface->SetFontGlyphSet(g_Globals->Espfont, "Courier New", 14, 300, 0, 0, FONTFLAG_OUTLINE);
	I::Surface->SetFontGlyphSet(g_Globals->Menufont, "Trebuchet", 16, 550, 0, 0, FONTFLAG_ANTIALIAS);
	I::Surface->SetFontGlyphSet(g_Globals->Titlefont, "Trebuchet", 23, 500, 0, 0, FONTFLAG_ANTIALIAS);
	I::Surface->SetFontGlyphSet(g_Globals->Namefont, "Verdana", 12, 550, 0, 0, FONTFLAG_DROPSHADOW | FONTFLAG_ANTIALIAS);
}

bool __stdcall DllMain(void* dll_instance, unsigned long reason_to_call, void* reserved) {
	switch (reason_to_call) {
	case DLL_PROCESS_ATTACH: // if DLL attached to CSGO
		DisableThreadLibraryCalls((HMODULE)dll_instance);
		CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Initialize, dll_instance, 0, 0);
		break;
	}

	return true;
}