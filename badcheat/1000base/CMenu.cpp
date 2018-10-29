#include "Main.h"

void CMenu::Menu() {

	static bool PressedButtons = false;

	if (!PressedButtons && GetAsyncKeyState(VK_DELETE))
	{
		PressedButtons = true;
	}
	else if (PressedButtons && !GetAsyncKeyState(VK_DELETE))
	{
		PressedButtons = false;
		g_Vars->Menu.Opened = !g_Vars->Menu.Opened;
	}

	static Vector2D MenuPos;
	static Vector2D StoredMenuPos;
	static Vector2D StoredMousePos;

	static int DragX = 0;
	static int DragY = 0;

	static std::string Title = "jefferyhook"; // insert p2c name here

	static std::string Tab1 = "Aimbot";
	static std::string Tab2 = "Visuals";
	static std::string Tab3 = "Misc";
	static std::string Tab4 = "HvH";

	static bool IsDraggingWindow = false;

	bool click = false;

	if (g_Vars->Menu.Opened) {

		if (GetAsyncKeyState(VK_LBUTTON))
			click = true;

		if (I::Surface->MouseInRegion(MenuPos.x + 577.f, MenuPos.y + 1.f, 22.f, 22) && click)
			g_Vars->Menu.Opened = !g_Vars->Menu.Opened;

		Vector2D MousePos = I::Surface->GetMousePosition();

		if (IsDraggingWindow && !click)
		{
			IsDraggingWindow = false;
		}

		if (IsDraggingWindow && click)
		{
			MenuPos.x = MousePos.x - DragX;
			MenuPos.y = MousePos.y - DragY;
		}

		if (I::Surface->MouseInRegion(MenuPos.x, MenuPos.y, 576, 23))
		{
			IsDraggingWindow = true;
			DragX = MousePos.x - MenuPos.x;
			DragY = MousePos.y - MenuPos.y;
		}

		I::Surface->FilledRect(MenuPos.x, MenuPos.y, 600, 450, Color(76, 76, 76, 255)); // Main Window
		I::Surface->DrawT(MenuPos.x + 300, MenuPos.y, Color(255, 255, 255, 255), g_Globals->Titlefont, true, Title.c_str()); // Title
		I::Surface->FilledRect(MenuPos.x + 577, MenuPos.y + 1, 22, 22, Color(233, 0, 41, 255)); // Red Box to close menu

		I::Surface->FilledRect(MenuPos.x + 1, MenuPos.y + 24, 148, 30, Color(99, 99, 99, 255)); // Aimbot Box
		I::Surface->DrawT(MenuPos.x + 75, MenuPos.y + 31, Color(255, 255, 255, 255), g_Globals->Menufont, true, Tab1.c_str()); // Aimbot Text
		I::Surface->FilledRect(MenuPos.x + 150, MenuPos.y + 24, 149, 30, Color(99, 99, 99, 255)); // Visuals Box
		I::Surface->DrawT(MenuPos.x + 225, MenuPos.y + 31, Color(255, 255, 255, 255), g_Globals->Menufont, true, Tab2.c_str()); // Visuals Text
		I::Surface->FilledRect(MenuPos.x + 300, MenuPos.y + 24, 149, 30, Color(99, 99, 99, 255)); // Misc Box
		I::Surface->DrawT(MenuPos.x + 375, MenuPos.y + 31, Color(255, 255, 255, 255), g_Globals->Menufont, true, Tab3.c_str()); // Misc Text
		I::Surface->FilledRect(MenuPos.x + 450, MenuPos.y + 24, 149, 30, Color(99, 99, 99, 255)); // HvH Box
		I::Surface->DrawT(MenuPos.x + 525, MenuPos.y + 31, Color(255, 255, 255, 255), g_Globals->Menufont, true, Tab4.c_str()); // HvH Text

		static int Tab = 1;

		if (I::Surface->MouseInRegion(MenuPos.x + 1, MenuPos.y + 24, 148, 30) && GetAsyncKeyState(VK_LBUTTON))
			Tab = 1;
		else if (I::Surface->MouseInRegion(MenuPos.x + 150, MenuPos.y + 24, 149, 30) && GetAsyncKeyState(VK_LBUTTON))
			Tab = 2;
		else if (I::Surface->MouseInRegion(MenuPos.x + 300, MenuPos.y + 24, 149, 30) && GetAsyncKeyState(VK_LBUTTON))
			Tab = 3;
		else if (I::Surface->MouseInRegion(MenuPos.x + 450, MenuPos.y + 24, 149, 30) && GetAsyncKeyState(VK_LBUTTON))
			Tab = 4;

		/*
		The check boxes need to be 22 pixels away from each other 
		the sliders need to be 48 away from checkboxes
		sliders are 45 away from other sliders

		im lazy so im not making it automatic
		*/

		if (Tab == 1) // Aimbot Tab selected
		{
			CheckBox(MenuPos.x + 10.f, MenuPos.y + 64.f, "Enabled", &g_Vars->Aimbot.Enable);

			if (g_Vars->Aimbot.Enable)
			{
				CheckBox(MenuPos.x + 10.f, MenuPos.y + 86.f, "AutoWall", &g_Vars->Aimbot.Autowall);
				CheckBox(MenuPos.x + 10.f, MenuPos.y + 108.f, "Head Only", &g_Vars->Aimbot.HeadOnly);
				CheckBox(MenuPos.x + 10.f, MenuPos.y + 130.f, "AutoStop", &g_Vars->Aimbot.AutoStop);

				Slider(MenuPos.x + 10.f, MenuPos.y + 178.f, 100.f, "Min Damage", &g_Vars->Aimbot.MinDamage);
				Slider(MenuPos.x + 10.f, MenuPos.y + 223.f, 100.f, "HitChance", &g_Vars->Aimbot.HitChance);
				Slider(MenuPos.x + 10.f, MenuPos.y + 268.f, 10.f, "Head Height", &g_Vars->Aimbot.HeadHeight);
				Slider(MenuPos.x + 10.f, MenuPos.y + 313.f, 5.f, "Body PointScale", &g_Vars->Aimbot.BodyPointscale);
				static std::string LagFix[3] = { "off", "Normal", "Dont use" };
				SliderCombo(MenuPos.x + 10.f, MenuPos.y + 358.f, 2.f, "LagFix", LagFix, &g_Vars->Aimbot.LagFix);
			}
		}
		else if (Tab == 2) // Visuals Tab selected
		{                                      
			CheckBox(MenuPos.x + 10.f, MenuPos.y + 64 + 0 * 22.f, "ESP Enabled", &g_Vars->Visuals.Enable); 

			if (g_Vars->Visuals.Enable)
			{
				CheckBox(MenuPos.x + 10.f, MenuPos.y + 64 + 1 * 22.f, "Name ESP", &g_Vars->Visuals.Name);
				CheckBox(MenuPos.x + 10.f, MenuPos.y + 64 + 2 * 22.f, "Box ESP", &g_Vars->Visuals.Box);
				CheckBox(MenuPos.x + 10.f, MenuPos.y + 64 + 3 * 22.f, "Weapon ESP", &g_Vars->Visuals.Weapon);
				CheckBox(MenuPos.x + 10.f, MenuPos.y + 64 + 4 * 22.f, "Health Bar", &g_Vars->Visuals.HealthBar);
				CheckBox(MenuPos.x + 10.f, MenuPos.y + 64 + 5 * 22.f, "Ammunition", &g_Vars->Visuals.Ammo);
				CheckBox(MenuPos.x + 10.f, MenuPos.y + 64 + 6 * 22.f, "Colored Models", &g_Vars->Visuals.Chams);
				CheckBox(MenuPos.x + 10.f, MenuPos.y + 64 + 7 * 22.f, "Glow Outline", &g_Vars->Visuals.Glow);
				CheckBox(MenuPos.x + 10.f, MenuPos.y + 64 + 8 * 22.f, "Skeleton", &g_Vars->Visuals.Skeleton);
				CheckBox(MenuPos.x + 10.f, MenuPos.y + 64 + 9 * 22.f, "Information", &g_Vars->Visuals.Info);
			}
		}
		else if (Tab == 3) // Misc Tab selected
		{
			CheckBox(MenuPos.x + 10.f, MenuPos.y + 64 + 0 * 22.f, "AutoHop", &g_Vars->Misc.AutoHop);
			CheckBox(MenuPos.x + 10.f, MenuPos.y + 64 + 1 * 22.f, "AutoStrafe", &g_Vars->Misc.AutoStrafe);
			CheckBox(MenuPos.x + 10.f, MenuPos.y + 64 + 2 * 22.f, "ThirdPerson", &g_Vars->Misc.ThirdPerson);
			CheckBox(MenuPos.x + 10.f, MenuPos.y + 64 + 3 * 22.f, "Clamp Angles", &g_Vars->Misc.ClampAngles);
		}
		else if (Tab == 4) // HvH Tab selected
		{
			CheckBox(MenuPos.x + 10.f, MenuPos.y + 64.f, "AtTargets", &g_Vars->HvH.AtTargets);

			static std::string Pitch[7] = { "off", "Down", "Fake up", "Fake Zero", "Fake Half Down", "Fake Jitter", "Black" };
			SliderCombo(MenuPos.x + 10.f, MenuPos.y + 112.f, 6.f, "Pitch", Pitch, &g_Vars->HvH.Pitch);

			static std::string Yaws[6] = { "off", "Freestanding", "Side Switch", "Backward", "TJitter", "180z" };
			SliderCombo(MenuPos.x + 10.f, MenuPos.y + 157.f, 5.f, "Yaw", Yaws, &g_Vars->HvH.Yaws);

			Slider(MenuPos.x + 10.f, MenuPos.y + 202.f, 15.f, "FakeLag", &g_Vars->HvH.FakeLag);
		}
	}
}

void CMenu::CheckBox(int x, int y, std::string name, bool* item)
{
	I::Surface->OutlinedRect(x, y, 18.f, 18.f, Color(99.f, 99.f, 99.f, 255));

	static bool pressed = false;

	if (!GetAsyncKeyState(VK_LBUTTON) && I::Surface->MouseInRegion(x, y, 18.f, 18))
	{
		if(pressed)
			*item = !*item;
		pressed = false;
	}

	if (GetAsyncKeyState(VK_LBUTTON) && I::Surface->MouseInRegion(x, y, 18.f, 18) && !pressed)
	{
		pressed = true;
	}

	if (*item == true)
		I::Surface->FilledRect(x, y, 18.f, 18.f, Color(99.f, 99.f, 99.f, 255));

	I::Surface->DrawT(x + 23.f, y + 1.f, Color(255.f, 255.f, 255.f, 255), g_Globals->Menufont, false, name.c_str());
}

// minimum can only be 0 bec im bad at math
void CMenu::Slider(int x, int y, float max, std::string name, int* item)
{
	I::Surface->OutlinedRect(x, y, 192.f, 1.f, Color(99.f, 99.f, 99.f, 255));

	float PixelValue = max / 192;

	if (GetAsyncKeyState(VK_LBUTTON) && I::Surface->MouseInRegion(x, y - 12, 193, 24))
	{
		*item = (I::Surface->GetMousePosition().x - x) * PixelValue;
	}
	
	char buffer[24];
	sprintf_s(buffer, "[%d]", *item);
	RECT textsize = I::Surface->GetTextSizeRect(g_Globals->Menufont, name.c_str());
	I::Surface->DrawT((x + 102 + (textsize.right / 2)), y - 26.f, Color(255.f, 255.f, 255.f, 255), g_Globals->Menufont, false, buffer);
	I::Surface->DrawT(x + 96.f, y - 25.f, Color(255.f, 255.f, 255.f, 255), g_Globals->Menufont, true, name.c_str());

	I::Surface->FilledRect((x + (*item / PixelValue)), y - 4.f, 4.f, 8.f, Color(99.f, 99.f, 99.f, 255));
}

void CMenu::SliderCombo(int x, int y, float indexes, std::string name, std::string itemname[], int* item)
{
	I::Surface->OutlinedRect(x, y, 192.f, 1.f, Color(99.f, 99.f, 99.f, 255));

	float PixelValue = indexes / 192;

	if (GetAsyncKeyState(VK_LBUTTON) && I::Surface->MouseInRegion(x, y - 12.f, 193.f, 24))
	{
		*item = (I::Surface->GetMousePosition().x - x) * PixelValue;
	}

	name += "->";

	RECT textsize = I::Surface->GetTextSizeRect(g_Globals->Menufont, name.c_str());
	I::Surface->DrawT((x + 102 + (textsize.right / 2)), y - 26.f, Color(255.f, 255.f, 255.f, 255), g_Globals->Menufont, false, itemname[*item].c_str());
	I::Surface->DrawT(x + 96.f, y - 25.f, Color(255.f, 255.f, 255.f, 255), g_Globals->Menufont, true, name.c_str());

	I::Surface->FilledRect((x + (*item / PixelValue)), y - 4.f, 4.f, 8.f, Color(99.f, 99.f, 99.f, 255));
}