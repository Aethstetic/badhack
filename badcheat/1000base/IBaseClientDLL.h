#pragma once
#include "IEntity.h"

class IPanel
{
public:
	const char *GetName(unsigned int vguiPanel)
	{
		typedef const char* (__thiscall* OriginalFn)(PVOID, unsigned int);
		return g_Utils->get_vfunc<OriginalFn>(this, 36)(this, vguiPanel);
	}
};

class CViewSetup
{
public:
	__int32   x;
	__int32   x_old;
	__int32   y;
	__int32   y_old;
	__int32   width;
	__int32   width_old;
	__int32   height;
	__int32   height_old;
	char      pad_0x0020[0x90];
	float     fov;
	float     viewmodel_fov;
	Vector    origin;
	Vector    angles;
	char      pad_0x00D0[0x7C];
};

class ClientClass
{
public:
	void* CreateFn;
	void* CreateEventFn;
	char* NetworkName;
	RecvTable* rtTable;
	ClientClass* pNextClass;
	int iClassID;
};

class IBaseClientDLL
{
public:
	ClientClass * GetAllClasses()
	{
		typedef ClientClass* (__thiscall* GetAllClassesFn)(void*);
		return g_Utils->get_vfunc<GetAllClassesFn>(this, 8)(this);
	}
};

class IGlobalVars
{
public:
	float     realtime;                     // 0x0000
	int       framecount;                   // 0x0004
	float     absoluteframetime;            // 0x0008
	float     absoluteframestarttimestddev; // 0x000C
	float     curtime;                      // 0x0010
	float     frametime;                    // 0x0014
	int       maxClients;                   // 0x0018
	int       tickcount;                    // 0x001C
	float     interval_per_tick;            // 0x0020
	float     interpolation_amount;         // 0x0024
	int       simTicksThisFrame;            // 0x0028
	int       network_protocol;             // 0x002C
	void*     pSaveData;                    // 0x0030
	bool      m_bClient;                    // 0x0031
	bool      m_bRemoteClient;              // 0x0032

private:
	// 100 (i.e., tickcount is rounded down to this base and then the "delta" from this base is networked
	int       nTimestampNetworkingBase;
	// 32 (entindex() % nTimestampRandomizeWindow ) is subtracted from gpGlobals->tickcount to Set the networking basis, prevents
	//  all of the entities from forcing a new PackedEntity on the same tick (i.e., prevents them from getting lockstepped on this)
	int       nTimestampRandomizeWindow;
};

class IClientMode
{
public:
	virtual             ~IClientMode() {}
	virtual int         ClientModeCSNormal(void *) = 0;
	virtual void        Init() = 0;
	virtual void        InitViewport() = 0;
	virtual void        Shutdown() = 0;
	virtual void        Enable() = 0;
	virtual void        Disable() = 0;
	virtual void        Layout() = 0;
	virtual IPanel*     GetViewport() = 0;
	virtual void*       GetViewportAnimationController() = 0;
	virtual void        ProcessInput(bool bActive) = 0;
	virtual bool        ShouldDrawDetailObjects() = 0;
	virtual bool        ShouldDrawEntity(C_BaseEntity *pEnt) = 0;
	virtual bool        ShouldDrawLocalPlayer(C_BaseEntity *pPlayer) = 0;
	virtual bool        ShouldDrawParticles() = 0;
	virtual bool        ShouldDrawFog(void) = 0;
	virtual void        OverrideView(CViewSetup *pSetup) = 0; // 19
	virtual int         KeyInput(int down, int keynum, const char *pszCurrentBinding) = 0; // 20
	virtual void        StartMessageMode(int iMessageModeType) = 0; //21
	virtual IPanel*     GetMessagePanel() = 0; //22
	virtual void        OverrideMouseInput(float *x, float *y) = 0; //23
	virtual bool        CreateMove(float flSampleFrametime, void* pCmd) = 0; // 24
	virtual void        LevelInit(const char *newmap) = 0;
	virtual void        LevelShutdown(void) = 0;
};

enum ClientFrameStage_t
{
	FRAME_UNDEFINED = -1,			// (haven't run any frames yet)
	FRAME_START,

	// A network packet is being recieved
	FRAME_NET_UPDATE_START,
	// Data has been received and we're going to start calling PostDataUpdate
	FRAME_NET_UPDATE_POSTDATAUPDATE_START,
	// Data has been received and we've called PostDataUpdate on all data recipients
	FRAME_NET_UPDATE_POSTDATAUPDATE_END,
	// We've received all packets, we can now do interpolation, prediction, etc..
	FRAME_NET_UPDATE_END,

	// We're about to start rendering the scene
	FRAME_RENDER_START,
	// We've finished rendering the scene.
	FRAME_RENDER_END
};