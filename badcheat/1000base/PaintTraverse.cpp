#include "Main.h"

PaintTraverseFn oPaintTraverse;
void __fastcall Hooks::PaintTraverse(PVOID pPanels, int edx, unsigned int vguiPanel, bool forceRepaint, bool allowForce)
{
	oPaintTraverse(pPanels, vguiPanel, forceRepaint, allowForce);

	PCHAR szPanelName = (PCHAR)I::Panel->GetName(vguiPanel);

	if (strstr(szPanelName, "MatSystemTopPanel"))
	{	
		GetKeyState(VK_MBUTTON) ? I::Surface->DrawT(100, 100, Color(255, 255, 255, 255), g_Globals->Espfont, false, ">") : I::Surface->DrawT(100, 100, Color(255, 255, 255, 255), g_Globals->Espfont, false, "<");

		g_Visuals->SetupDraw();

		g_MissedShots->DrawHitmarkers();

		g_Menu->Menu();
	}
}