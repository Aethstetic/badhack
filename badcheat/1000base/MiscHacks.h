#pragma once

class MiscHacks {
public:

	void Move(CUserCmd* pCmd);
	
private:

	void AutoHop(CUserCmd* pCmd);

	void AutoStrafe(CUserCmd* pCmd);

}; extern MiscHacks* g_MiscHacks;