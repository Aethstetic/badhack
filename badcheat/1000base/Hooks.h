#pragma once

using CreateMoveFn = bool(__stdcall*)(float, CUserCmd*);
extern CreateMoveFn oCreateMove;

typedef void(__thiscall* LockCursorFn)(void*);
extern LockCursorFn oLockCursor;

typedef void(__thiscall* PaintTraverseFn)(PVOID, unsigned int, bool, bool);
extern PaintTraverseFn oPaintTraverse;

using DrawModelExecuteFn = void*(__stdcall*)(void*, void*, const ModelRenderInfo_t&, matrix3x4_t*);
extern DrawModelExecuteFn oDrawModelExecute;

typedef void(__stdcall *FrameStageNotifyFn)(ClientFrameStage_t);
extern FrameStageNotifyFn oFrameStageNotify;

typedef bool(__thiscall *FireEventClientSideFn)(void*, IGameEvent*);
extern FireEventClientSideFn oFireEventClientSide;

typedef void(__fastcall *SceneEndFn)(void*);
extern SceneEndFn oSceneEnd;

/*typedef void(__stdcall *DrawModelExecuteFn)(void*, void*, const ModelRenderInfo_t&, matrix3x4_t*);
extern DrawModelExecuteFn oDrawModelExecute;*/

namespace Hooks {
	extern bool __stdcall CreateMove(float flInputSampleTime, CUserCmd* cmd);
	extern void __stdcall LockCursor();
	extern void __fastcall PaintTraverse(PVOID pPanels, int edx, unsigned int vguiPanel, bool forceRepaint, bool allowForce);
	extern void __stdcall DrawModelExecute(void* context, void* state, const ModelRenderInfo_t &pInfo, matrix3x4_t *pCustomBoneToWorld);
	extern void __stdcall FrameStageNotify(ClientFrameStage_t curStage);
	extern bool __fastcall FireEventClientSide(void *ecx, void* edx, IGameEvent* pEvent);
	extern void __fastcall SceneEnd(void *ecx, void *edx);
}