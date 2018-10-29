#pragma once

template<class T, class U>
T clamp(T in, U low, U high)
{
	if (in <= low)
		return low;

	if (in >= high)
		return high;

	return in;
}

class CMenu {
public:
	void Menu();
private:
	void CheckBox(int x, int y, std::string name, bool* item);
	void Slider(int x, int y, float max, std::string name, int* item);
	void SliderCombo(int x, int y, float indexes, std::string name, std::string itemname[], int* item);
}; extern CMenu* g_Menu;