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
#include "Experience.h"
#include "Helper.h"

#define NK_IMPLEMENTATION
#include "nuklear.h"

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


using nlohmann::json;
json itemList;

std::string botStatus = "Not started.";

int SelectedBot = -1;
int SelectedWood = 0;
int SelectedOre = 0;


enum class BotType {
	SpellWisp,
	Rabbit,
	GeneralCombat,
	Mining,
	ClockworkSuit,
	WoodCutting,
	AnachroniaAgility
};

std::vector<const char *> botList = {"Spellwisp", "Rabbit", "General Combat", "Mining", "Clockwork Suit", "WoodCutting", "Anachronia Agility"};
std::vector<std::string> TreeNames = { "Tree", "Oak", "Willow", "Teak", "Maple", "Acadia", "Mahogany", "Yew" };
std::vector<std::string> LogNames = { "Logs", "Oak logs", "Willow logs", "Teak logs", "Maple logs", "Acadia logs", "Mahogany logs", "Yew logs" };

bool h_OnDispatchNetMessage(UINT_PTR* a1, UINT_PTR* a2)
{
	return o_OnDispatchNetMessage(a1, a2);
}

bool RecordAgility = false;
std::vector<AgilityCourse> agiList;
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

	if (RecordAgility && param1 > 1)
	{
		printf("Adding %d to agi list\n", param1);

		agiList.push_back(AgilityCourse(param1));
	}

	return o_OnCursorDoAction(a1, actionPtr, position);
}

UINT_PTR h_CursorWorldContextMenu(UINT_PTR* GameContext, int a2, int a3, int a4, int a5)
{
	g_GameContext = GameContext;

	return o_CursorWorldConextMenu(GameContext, a2, a3, a4, a5);
}

std::map<int, HGLRC> contexts;

bool h_wglSwapBuffers(HDC hdc)
{
	

	int pixelformat = GetPixelFormat(hdc);
	if (!contexts.count(pixelformat))
	{
		HGLRC ctx = wglCreateContext(hdc);
		HGLRC old_ctx = wglGetCurrentContext();
		HDC old_dc = wglGetCurrentDC();
		wglMakeCurrent(hdc, ctx);

		glDisable(GL_ALPHA_TEST);
		glDisable(GL_AUTO_NORMAL);
		glEnable(GL_BLEND);
		glDisable(GL_COLOR_LOGIC_OP);
		glDisable(GL_COLOR_MATERIAL);
		glDisable(GL_COLOR_TABLE);
		glDisable(GL_CONVOLUTION_1D);
		glDisable(GL_CONVOLUTION_2D);
		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_DITHER);
		glDisable(GL_FOG);
		glDisable(GL_HISTOGRAM);
		glDisable(GL_INDEX_LOGIC_OP);
		glDisable(GL_LIGHTING);
		glDisable(GL_MINMAX);
		glDisable(GL_SEPARABLE_2D);
		glDisable(GL_SCISSOR_TEST);
		glDisable(GL_STENCIL_TEST);
		glEnable(GL_TEXTURE_2D);
		glDisable(GL_TEXTURE_GEN_Q);
		glDisable(GL_TEXTURE_GEN_R);
		glDisable(GL_TEXTURE_GEN_S);
		glDisable(GL_TEXTURE_GEN_T);
		glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

		wglMakeCurrent(old_dc, old_ctx);
		contexts[pixelformat] = ctx;
	}

	//Overwrite context
	HGLRC oldctx = wglGetCurrentContext();
	HDC oldhdc = wglGetCurrentDC();
	wglMakeCurrent(hdc, contexts[pixelformat]);

	uint32_t width = GetDeviceCaps(hdc, HORZRES);
	uint32_t height = GetDeviceCaps(hdc, VERTRES);
	
	HWND hwnd = WindowFromDC(hdc);
	if (hwnd)
	{
		RECT r;
		if (GetClientRect(hwnd, &r))
		{
			width = r.right - r.left;
			height = r.bottom - r.top;
		}
	}
	

	
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glPushMatrix();
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, width, height, 0, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glDisable(GL_DEPTH_TEST);
	
	Render r{ hdc, 11 };
	glColor3ub(0xFF, 0x0, 0x0);
	//r.DrawRect(100, 100, 200, 200, false);

	auto status = "NXTBot Heph Status =  " + botStatus;
	r.PrintCStr(100.0f, 70.0f, 0xFF, 0x00, 0x00, status.data());

	
	if (SelectedBot == -1)
	{
		r.PrintCStr(100.0f, 90.0f, 0x00, 0xFF, 0x00, std::string("Selected Bot: None").data());
	}
	else
	{
		std::string selectedBotName = std::string("Selected Bot: ") + std::string(botList[SelectedBot]);
		r.PrintCStr(100.0f, 90.0f, 0x00, 0xFF, 0x00, selectedBotName.data());

		if (SelectedBot == (int)BotType::WoodCutting)
		{
			std::string TreeName = std::string("Selected Tree: ") + std::string(TreeNames[SelectedWood]);
			r.PrintCStr(100.0f, 110.0f, 44, 195, 212, TreeName.data());
		}

	}
	
	// Botting
	Manager::Manage();

	glPopMatrix();
	glPopAttrib();

	wglMakeCurrent(oldhdc, oldctx);


	return o_wglSwapBuffers(hdc);
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
			if (wParam == VK_NUMPAD0)
			{
				/*
				printf("std::vector<AgilityCourse> agiList = { ");

				for (auto i : agiList)
				{
					printf("AgilityCourse(%d, Tile2D(%d, %d)), ", i.objId, i.EndPos.x, i.EndPos.y);
				}


				printf(" };");
				*/
			}

			if (wParam == VK_NUMPAD1)
			{

				auto leech = RS::GetEntityNPCByName("Dark mage");

				if (leech)
				{
					printf("Dark mage with distance: %f\n", RS::GetDistance(RS::GetLocalPlayerTilePos(), RS::GetEntityTilePos(leech)));
				}


			}
			if (wParam == VK_NUMPAD2)
			{
				auto invent = Inventory::GetInventoryItems();

				/*
				auto player = RS::GetLocalPlayerTilePos();
				agiList[agiList.size() - 1].EndPos = player;

				printf("Added (%d, %d) as player pos.\n", player.x, player.y);
				*/
			}
			if (wParam == VK_NUMPAD3)
			{
				Player player = RS::GetLocalPlayer();

				auto tree = Static::GetClosestStaticTreeObjectByName("Tree");
				if (tree.Definition)
				{
					printf("Choppign down %d [%d] at (%d, %d)\n", tree.Definition->Id, tree.Type, tree.TileX, tree.TileY);
					player.StaticInteract(tree);
				}
				else {
					printf("Cant find tree\n");
				}

				/*
				RecordAgility = !RecordAgility;

				printf("Record Agi: %d\n", RecordAgility);
				*/
			}
			if (wParam == VK_NUMPAD4)
			{
			}
			if (wParam == VK_NUMPAD5)
			{
			}

			if (wParam == VK_OEM_3)
			{
				UINT_PTR* PlayerObj = *(UINT_PTR**)(g_GameContext[1] + 0x1780);
				auto player = o_GetLocalPlayer(PlayerObj);
				printf("Found local player players %p and %d entities %s.\n", RS::GetLocalPlayer(), RS::GetEntityCount(), RS::GetLocalPlayer()->Name);
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

#ifdef NDEBUG
	std::ifstream inputs("C:\\ProgramData\\Jagex\\launcher\\ItemList.json", std::ios::binary);

	if (!inputs.fail() && inputs.peek() != std::ifstream::traits_type::eof()) {
		printf("[+] Successfully Read the JSON Item List file.\n\n\n");

		inputs >> itemList;

	}
	else
	{
		printf("[!] Item List JSON File is not in the Launcher folder!\n");
	}
#endif

	o_GetLocalPlayer = (fn_GetLocalPlayer)Patterns.Func_GetLocalPlayer;

	o_OnCursorDoAction = (fn_OnCursorDoAction)Patterns.Func_OnCursorDoAction;

	o_OnCursorDoAction = (fn_OnCursorDoAction)detours.Hook(o_OnCursorDoAction, h_OnCursorDoAction, 15);

	o_CursorWorldConextMenu = (fn_CursorWorldContextMenu)Patterns.Func_OnCursorWorldContextMenu;

	o_CursorWorldConextMenu = (fn_CursorWorldContextMenu)detours.Hook(o_CursorWorldConextMenu, h_CursorWorldContextMenu, 15);

	o_OnDispatchNetMessage = (fn_OnDispatchNetMessage)Patterns.Func_OnDispatchMessage;

	o_OnDispatchNetMessage = (fn_OnDispatchNetMessage)detours.Hook(o_OnDispatchNetMessage, h_OnDispatchNetMessage, 18);

	o_wglSwapBuffers = (fn_wglSwapBuffers)detour_iat_ptr("SwapBuffers", h_wglSwapBuffers, (HMODULE)HdnGetModuleBase("rs2client.exe"));

	OriginalWndProcHandler = (WNDPROC)SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG_PTR)hWndProc);

#ifdef NDEBUG
	Beep(523, 500);
#endif
	return true;
}