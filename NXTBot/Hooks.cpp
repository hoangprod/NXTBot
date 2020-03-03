#include "pch.h"
#include "Hooks.h"
#include "Patterns.h"
#include "GameClasses.h"
#include "Game.h"
#include "Inventory.h"
#include "ItemDef.h"
#include "Render.h"
#include "Matrix.h"
#include "Tile.h"
#include "PlayableEntity.h"
#include "Widgets.h"
#include "Wisp.h"
#include "Manager.h"
#include "Helper.h"

Detour64 detours;
HWND hWnd;
extern Addr Patterns;
bool g_HijackCtrl = false;
UINT_PTR* g_GameContext = 0;
UINT_PTR g_Module = 0;
static WNDPROC OriginalWndProcHandler = nullptr;

fn_wglSwapBuffers o_wglSwapBuffers;
fn_GetLocalPlayer o_GetLocalPlayer;
fn_OnCursorDoAction o_OnCursorDoAction;
fn_CursorWorldContextMenu o_CursorWorldConextMenu;
fn_OnDispatchNetMessage o_OnDispatchNetMessage;
fn_GUIManagerRender o_Render;

nlohmann::json itemList;


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

	//printf("[+] actionPtr %p, action %p, dispatcher %p, dispatcherInstance %p, Index %d, TileX %d, TileY %d, dispatcherFunc %p -> \n", actionPtr, action, dispatcher, dispatcherInstance, param1, param2, param3, dispatcherFunc);
	printf("[+] Index %d, TileX %d, TileY %d, dispatcherFunc %p -> 0x%x\n",  param1, param2, param3, dispatcherFunc, (UINT_PTR)dispatcherFunc - g_Module);

	return o_OnCursorDoAction(a1, actionPtr, position);
}

UINT_PTR h_CursorWorldContextMenu(UINT_PTR* GameContext, int a2, int a3, int a4, int a5)
{
	g_GameContext = GameContext;

	return o_CursorWorldConextMenu(GameContext, a2, a3, a4, a5);
}


bool h_wglSwapBuffers(HDC hDc)
{
	// Botting
	Manager::Manage();

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

static BOOL CALLBACK enumWindowCallback(HWND hWndv, LPARAM lparam) {

	DWORD lpdwProcessId;
	GetWindowThreadProcessId(hWndv, &lpdwProcessId);

	if(lpdwProcessId == lparam)
	{
		char* buffer = new char[MAX_PATH];


		GetClassNameA(hWndv, buffer, MAX_PATH);
		std::string windowTitle(buffer);

		// List visible windows with a non-empty title
		if (windowTitle.length() != 0 && windowTitle == "JagWindow") {
			hWnd = FindWindowExW(hWndv, 0, L"JagOpenGLView", 0);
			return TRUE;
		}

		delete[] buffer;
	}
	return TRUE;
}



LRESULT CALLBACK hWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (false)
	{
		SetCursor((HCURSOR)LoadImageA(NULL, MAKEINTRESOURCEA(32515), IMAGE_CURSOR, 0, 0, LR_SHARED));
	}

	if (uMsg == WM_KEYUP)
	{

		Manager::Keystates(wParam);

		if (wParam == VK_NUMPAD0)
		{
			printf("gcontext = %p\n", g_GameContext);
		}

		if (RS::IsInGame())
		{
			if (wParam == VK_NUMPAD1)
			{
				if (itemList.size() == 0)
				{
					std::ifstream inputs("C:\\ProgramData\\Jagex\\launcher\\ItemList.json");

					if (!inputs.fail() && inputs.peek() != std::ifstream::traits_type::eof()) {
						printf("Input is legit\n");
						inputs >> itemList;
					}
					else
					{
						printf("wtf?\n");
					}


				//	inputs.close();
				}


				printf("%d %d\n", itemList.count("1925"), itemList.count("99999999"));

				if (itemList.count("1925") > 0)
				{
					std::string iteName = itemList["1925"]["name"].get<std::string>();

					printf("%s\n", iteName.data());
				}
			}
			if (wParam == VK_NUMPAD2)
			{
			}
			if (wParam == VK_NUMPAD3)
			{
			}
			if (wParam == VK_NUMPAD4)
			{
			}
			if (wParam == VK_NUMPAD5)
			{
			}
			if (wParam == VK_NUMPAD6)
			{
				/*
				auto localplayer = RS::GetLocalPlayer();
				Player play = Player(localplayer);
				play.Test();
				*/
			}
			if (wParam == VK_OEM_3)
			{
#ifdef _DEBUG
				UINT_PTR* PlayerObj = *(UINT_PTR**)(g_GameContext[1] + 0x1780);
				auto player = o_GetLocalPlayer(PlayerObj);
				printf("Found local player players %p and %d entities %s.\n", RS::GetLocalPlayer(), RS::GetEntityCount(), RS::GetLocalPlayer()->Name);
#endif
			}
		}
		if (wParam == VK_CONTROL)
		{

		}
	}

	if (wParam == VK_HOME)
		Unload();

	return CallWindowProc(OriginalWndProcHandler, hWnd, uMsg, wParam, lParam);
}

bool hooks()
{

	do
	{
		printf("[+] Finding the correct HWND\n");
		EnumWindows(enumWindowCallback, GetCurrentProcessId());
		Sleep(1000);
	} while (!hWnd);

	printf("[+] Found HWND at %p\n", hWnd);

	g_Module = (UINT_PTR)HdnGetModuleBase("rs2client.exe");

	if (!findPatterns())
	{
		printf("[!] Failed to pattern scan!\n");
		return false;
	}

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

	//ofn_GetInterface = (fn_GetInterface)detours.Hook(ofn_GetInterface, h_GetInterface, 17);

	o_wglSwapBuffers = (fn_wglSwapBuffers)detour_iat_ptr("SwapBuffers", h_wglSwapBuffers, (HMODULE)HdnGetModuleBase("rs2client.exe"));

	OriginalWndProcHandler = (WNDPROC)SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG_PTR)hWndProc);




#ifdef NDEBUG
	Beep(523, 500);
#endif
	return true;
}