#include "pch.h"
#include "Hooks.h"
#include "Patterns.h"
#include "Draw.h"
#include "GameClasses.h"
#include "EventHandler.h"
#include "Game.h"
#include "Action.h"
#include "Helper.h"

Detour64 detours;
HWND hWnd;
Draw draw;
extern Addr Patterns;
bool g_HijackCtrl = false;
UINT_PTR* g_GameContext = 0;
static WNDPROC OriginalWndProcHandler = nullptr;

fn_wglSwapBuffers o_wglSwapBuffers;
fn_GetLocalPlayer o_GetLocalPlayer;
fn_OnCursorDoAction o_OnCursorDoAction;
fn_CursorWorldContextMenu o_CursorWorldConextMenu;
fn_OnDispatchNetMessage o_OnDispatchNetMessage;
fn_GUIManagerRender o_Render;


bool h_OnDispatchNetMessage(UINT_PTR* a1, UINT_PTR* a2)
{
	return o_OnDispatchNetMessage(a1, a2);
}


DWORD* h_OnCursorDoAction(UINT_PTR a1, ActionPtr actionPtr, float* position)
{
	printf("[+] click at (%.1f, %.1f)\n", position[0], position[1]);

	auto action = actionPtr.Action;
	auto dispatcher = action->Dispatcher;
	auto dispatcherInstance = dispatcher->DispatcherInstance;
	auto dispatcherVTable = dispatcherInstance->DispatcherVTable;
	auto dispatcherFunc = dispatcherVTable->dispatcherFunc;
	auto param1 = action->Index;
	auto param2 = action->TileX;
	auto param3 = action->TileY;
	/*
	uint64_t action = *reinterpret_cast<uint64_t*>(actionPtr + 1);
	uint64_t dispatcher = *reinterpret_cast<uint64_t*>(action + 0x40);

	uint64_t dispatcherInstance = *reinterpret_cast<uint64_t*>(dispatcher + 0x38);
	uint64_t dispatcherVTable = *reinterpret_cast<uint64_t*>(dispatcherInstance);
	uint64_t dispatcherFunc =
		*reinterpret_cast<uint64_t*>(dispatcherVTable + 0x10);
	uint32_t param1 =
		*reinterpret_cast<uint32_t*>(action + 0x50); // modelId, index
	uint32_t param2 = *reinterpret_cast<uint32_t*>(action + 0x54); // tileX
	uint32_t param3 = *reinterpret_cast<uint32_t*>(action + 0x58); // tileY
	*/
	printf("[+] actionPtr %p, action %p, dispatcher %p, dispatcherInstance %p, Index %d, TileX %d, TileY %d, dispatcherFunc %p \n", actionPtr, action, dispatcher, dispatcherInstance, param1, param2, param3, dispatcherFunc);

	return o_OnCursorDoAction(a1, actionPtr, position);
}

UINT_PTR h_CursorWorldContextMenu(UINT_PTR* GameContext, int a2, int a3, int a4, int a5)
{
	g_GameContext = GameContext;

	return o_CursorWorldConextMenu(GameContext, a2, a3, a4, a5);
}


Vec2 Get_ScreenDimension()
{
	GLint m_viewport[4];

	glGetIntegerv(GL_VIEWPORT, m_viewport);

	Vec2 dim;
	dim.x = m_viewport[2];
	dim.y = m_viewport[3];

	return dim;
}


void h_Render(UINT_PTR* a1)
{
	glDisable(GL_TEXTURE_2D);
	glBegin(GL_LINES);
	glColor3f(1.0f, 0.0f, 0.0f); // RGB value
	glVertex2f(0, 0); // Line Origin (top left)
	glVertex2f(800, 600); // Line end
	glEnd();
	glEnable(GL_TEXTURE_2D);
	return o_Render(a1);
}



bool h_wglSwapBuffers(HDC hDc)
{
	glDisable(GL_TEXTURE_2D);
	glBegin(GL_LINES);
	glColor3f(1.0f, 0.0f, 0.0f); // RGB value
	glVertex2f(0, 0); // Line Origin (top left)
	glVertex2f(800, 600); // Line end
	glEnd();
	glEnable(GL_TEXTURE_2D);
	return o_wglSwapBuffers(hDc);
}


BOOL hook_function(PVOID& t1, PBYTE t2, bool hook, const char* s = NULL)
{
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	if (hook)
		DetourAttach(&t1, t2);
	else
		DetourDetach(&t1, t2);
	if (DetourTransactionCommit() != NO_ERROR) {
#ifdef _DEBUG
		printf("[Hook] - Failed to hook %s.\n", s);
#endif
		return false;
	}
	else {
#ifdef _DEBUG
		printf("[Hook] - Successfully hooked %s.\n", s);
#endif
		return true;
	}
}


bool __stdcall Unload()
{
	if (detours.Clearhook())
	{
		o_wglSwapBuffers = (fn_wglSwapBuffers)detour_iat_ptr("SwapBuffers", o_wglSwapBuffers, (HMODULE)HdnGetModuleBase("rs2client.exe"));
		//hook_function((PVOID&)o_wglSwapBuffers, (PBYTE)h_wglSwapBuffers, false);

		SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG_PTR)OriginalWndProcHandler);

		return true;
	}

	return false;
}

LRESULT CALLBACK hWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (false)
	{
		SetCursor((HCURSOR)LoadImageA(NULL, MAKEINTRESOURCEA(32515), IMAGE_CURSOR, 0, 0, LR_SHARED));
	}

	if (uMsg == WM_KEYUP)
	{
		if (wParam == VK_INSERT)
		{
			auto test = RS::GetLocalPlayerPos();
			printf("%f %f\n", test[0], test[2]);
		}
		if (wParam == VK_NUMPAD0)
		{
			auto test = RS::GetMouseIntersectWorldPos();
			GAction::Walk(test);
		}
		if (wParam == VK_NUMPAD1)
		{
			Tile2D test;
			test.x = 2917;
			test.y = 3551;
			GAction::Walk(test);
		}
		if (wParam == VK_OEM_3)
		{
			UINT_PTR* PlayerObj = *(UINT_PTR**)(g_GameContext[1] + 0x1780);
			auto player = o_GetLocalPlayer(PlayerObj);
			printf("Found %p players and %d entities %s.\n", RS::GetLocalPlayer(), RS::GetEntityCount(), RS::GetLocalPlayer()->Name);

		}

		if (wParam == VK_CONTROL)
		{
			g_HijackCtrl = !g_HijackCtrl;
		}
	}

	if (wParam == VK_HOME)
		Unload();

	return CallWindowProc(OriginalWndProcHandler, hWnd, uMsg, wParam, lParam);
}



bool hooks()
{
	findPatterns();

	hWnd = FindWindowEx(FindWindow(L"JagWindow", 0), 0, L"JagOpenGLView", 0);

	if (!hWnd)
		return false;

	o_GetLocalPlayer = (fn_GetLocalPlayer)Patterns.Func_GetLocalPlayer;

	o_OnCursorDoAction = (fn_OnCursorDoAction)Patterns.Func_OnCursorDoAction;

	o_OnCursorDoAction = (fn_OnCursorDoAction)detours.Hook(o_OnCursorDoAction, h_OnCursorDoAction, 15);

	o_CursorWorldConextMenu = (fn_CursorWorldContextMenu)Patterns.Func_OnCursorWorldContextMenu;

	o_CursorWorldConextMenu = (fn_CursorWorldContextMenu)detours.Hook(o_CursorWorldConextMenu, h_CursorWorldContextMenu, 15);

	o_OnDispatchNetMessage = (fn_OnDispatchNetMessage)Patterns.Func_OnDispatchMessage;

	o_OnDispatchNetMessage = (fn_OnDispatchNetMessage)detours.Hook(o_OnDispatchNetMessage, h_OnDispatchNetMessage, 18);

	o_wglSwapBuffers = (fn_wglSwapBuffers)detour_iat_ptr("SwapBuffers", h_wglSwapBuffers, (HMODULE)HdnGetModuleBase("rs2client.exe"));

	OriginalWndProcHandler = (WNDPROC)SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG_PTR)hWndProc);

	return true;
}


/*
o_Render = (fn_GUIManagerRender)0x7ff64e44ccc0;

o_Render = (fn_GUIManagerRender)detours.Hook(o_Render, h_Render, 15);

o_wglSwapBuffers = (fn_wglSwapBuffers)GetImportB("OPENGL32.dll", "wglSwapBuffers");

hook_function((PVOID&)o_wglSwapBuffers, (PBYTE)h_wglSwapBuffers, true);
*/