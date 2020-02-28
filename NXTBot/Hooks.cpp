#include "pch.h"
#include "Hooks.h"
#include "Patterns.h"
#include "GameClasses.h"
#include "EventHandler.h"
#include "Game.h"
#include "Inventory.h"
#include "ItemDef.h"
#include "Render.h"
#include "Matrix.h"
#include "Helper.h"

Detour64 detours;
HWND hWnd;
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
	auto action = actionPtr.Action;
	auto dispatcher = action->Dispatcher;
	auto dispatcherInstance = dispatcher->DispatcherInstance;
	auto dispatcherVTable = dispatcherInstance->DispatcherVTable;
	auto dispatcherFunc = dispatcherVTable->dispatcherFunc;
	auto param1 = action->Index;
	auto param2 = action->TileX;
	auto param3 = action->TileY;

	printf("[+] actionPtr %p, action %p, dispatcher %p, dispatcherInstance %p, Index %d, TileX %d, TileY %d, dispatcherFunc %p \n", actionPtr, action, dispatcher, dispatcherInstance, param1, param2, param3, dispatcherFunc);

	return o_OnCursorDoAction(a1, actionPtr, position);
}

UINT_PTR h_CursorWorldContextMenu(UINT_PTR* GameContext, int a2, int a3, int a4, int a5)
{
	g_GameContext = GameContext;

	return o_CursorWorldConextMenu(GameContext, a2, a3, a4, a5);
}


bool h_wglSwapBuffers(HDC hDc)
{
	return o_wglSwapBuffers(hDc);
}




bool __stdcall Unload()
{
	if (detours.Clearhook())
	{
		o_wglSwapBuffers = (fn_wglSwapBuffers)detour_iat_ptr("SwapBuffers", o_wglSwapBuffers, (HMODULE)HdnGetModuleBase("rs2client.exe"));

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
		}
		if (wParam == VK_NUMPAD0)
		{
			auto test = RS::GetClosestPlayer();

			if (!test)
				return 0;
		}
		if (wParam == VK_NUMPAD1)
		{
			printf("gcontext = %p\n", g_GameContext);
		}
		if (wParam == VK_NUMPAD2)
		{
			RS::GetClosestMonster();
		}
		if (wParam == VK_NUMPAD3)
		{

		}
		if (wParam == VK_OEM_3)
		{
			UINT_PTR* PlayerObj = *(UINT_PTR**)(g_GameContext[1] + 0x1780);
			auto player = o_GetLocalPlayer(PlayerObj);
			printf("Found local player players %p and %d entities %s.\n", RS::GetLocalPlayer(), RS::GetEntityCount(), RS::GetLocalPlayer()->Name);
			
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
	if (!findPatterns())
	{
		printf("[!] Failed to pattern scan!\n");
		return false;
	}

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

	//o_Yeet = (fn_Yeet)0x7FF6E48B96A0;

	//o_Yeet = (fn_Yeet)detours.Hook(o_Yeet, h_Yeet, 14);

	o_wglSwapBuffers = (fn_wglSwapBuffers)detour_iat_ptr("SwapBuffers", h_wglSwapBuffers, (HMODULE)HdnGetModuleBase("rs2client.exe"));

	OriginalWndProcHandler = (WNDPROC)SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG_PTR)hWndProc);

	return true;
}