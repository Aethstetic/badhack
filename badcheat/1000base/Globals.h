#pragma once

class Globals
{
public:
	HWND CSGOWindow;
	bool RenderInit;
	DWORD Espfont;
	DWORD Menufont;
	DWORD Titlefont;
	DWORD Namefont;
	C_BaseEntity* LocalPlayer;
	Vector AimPoints[64][25];
	int EnemyIndex;
	int EntShots[64];
	int EntHits[64];
	Vector RealAngle;
	bool Shot;
	Vector OriginalAng;
	Vector LastAimVec;
	matrix3x4_t	LastAimMatrix[128];
	int MissedShotsResolve[64];
	std::string ResolverStage[64];
	bool IsChokingTicks[64];
	float m_r8time;
private:
}; extern Globals* g_Globals;