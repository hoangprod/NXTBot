#include "pch.h"
#include "Hooks.h"
#include "Patterns.h"
#include "Draw.h"
#include "GameClasses.h"
#include "EventHandler.h"
#include "Game.h"
#include "Action.h"
#include "Inventory.h"
#include "ItemDef.h"
#include "JagexList.h"
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
fn_GetWOrldTranslation o_GetWOrldTranslation = (fn_GetWOrldTranslation)0x7ff64e1b3320;
fn_GUIManagerRender o_Render;


bool h_OnDispatchNetMessage(UINT_PTR* a1, UINT_PTR* a2)
{

	return o_OnDispatchNetMessage(a1, a2);
}


DWORD* h_OnCursorDoAction(UINT_PTR a1, ActionPtr actionPtr, float* position)
{
	//printf("[+] click at (%.1f, %.1f)\n", position[0], position[1]);

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


Vec2 Get_ScreenDimension()
{
	GLint m_viewport[4];

	glGetIntegerv(GL_VIEWPORT, m_viewport);

	Vec2 dim;
	dim.x = m_viewport[2];
	dim.y = m_viewport[3];

	return dim;
}



bool h_wglSwapBuffers(HDC hDc)
{

	auto test = o_wglSwapBuffers(hDc);
	glDisable(GL_TEXTURE_2D);
	glBegin(GL_LINES);
	glColor3f(1.0f, 0.0f, 0.0f); // RGB value
	glVertex2f(0, 0); // Line Origin (top left)
	glVertex2f(800, 600); // Line end
	glEnd();
	glEnable(GL_TEXTURE_2D);
	return test;
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
			 RS::LoopEntityList();
		}
		if (wParam == VK_NUMPAD0)
		{
			auto test = RS::GetClosestPlayer();
			float* result = o_GetWOrldTranslation((UINT_PTR*)test->camera);
			printf("%f %f %f\n", result[0], result[1], result[2]);
		}
		if (wParam == VK_NUMPAD1)
		{
			printf("gcontext = %p\n", g_GameContext);
		}
		if (wParam == VK_NUMPAD2)
		{
			auto inventory = Inventory::GetContainerObj(static_cast<uint32_t>(ContainerType::Backpack));
			auto bank = Inventory::GetContainerObj(static_cast<uint32_t>(ContainerType::Bank));
			auto coin = Inventory::GetContainerObj(static_cast<uint32_t>(ContainerType::CoinPouch));
			printf("inventory: %p %p %p\n", inventory, bank, coin);
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

	o_wglSwapBuffers = (fn_wglSwapBuffers)detour_iat_ptr("SwapBuffers", h_wglSwapBuffers, (HMODULE)HdnGetModuleBase("rs2client.exe"));

	OriginalWndProcHandler = (WNDPROC)SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG_PTR)hWndProc);

	return true;
}