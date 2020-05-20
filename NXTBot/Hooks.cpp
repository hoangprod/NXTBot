#include "pch.h"
#include "Hooks.h"
#include "Patterns.h"
#include "GameClasses.h"
#include "Game.h"
#include "Inventory.h"
#include "ItemDef.h"
#include "Render.h"
#include "Tile.h"
#include "PlayableEntity.h"
#include "Widgets.h"
#include "Wisp.h"
#include "Manager.h"
#include "Experience.h"
#include "Helper.h"
#include "Varpbit.h"
#include "Archeology.h"
#include "Common.h"
#include "Auth.h"
#include "tiger.h"
#include "client_ipc.h"
#include "Antiban.h"
//#include <detours.h>

#define _CRTDBG_MAP_ALLOC

Detour64 detours;
HWND hWnd;
extern bool to_suicide;
extern bool break_type;
extern Addr Patterns;
extern Archeology* archelogy;
extern std::vector<mouse_biometric> mouse_data;


bool g_HijackCtrl = false;
UINT_PTR* g_GameContext = 0;
UINT_PTR g_Module = 0;
HANDLE gMutex;
HANDLE hFileMap;
uintptr_t sharedMem;

static WNDPROC OriginalWndProcHandler = nullptr;

fn_wglSwapBuffers o_wglSwapBuffers;
fn_GetLocalPlayer o_GetLocalPlayer;
fn_OnCursorDoAction o_OnCursorDoAction;
fn_CursorWorldContextMenu o_CursorWorldConextMenu;
fn_ExecuteHookInner o_ExecuteHookInner;
fn_OnDispatchNetMessage o_OnDispatchNetMessage;
fn_GUIManagerRender o_Render;
fn_CopyString o_CopyString;
fn_StartLogin o_StartLogin;
fn_PrepareUUIDPacket o_PrepareUUIDPacket;

fn_GetVarpType o_GetVarpType = (fn_GetVarpType)0x7FF60F64F550;
fn_SetVarpValueFromServer o_SetVarpValueFromServer = (fn_SetVarpValueFromServer)0x7FF60F5F9020;

char* biometric_data = 0;
nlohmann::json itemList;

bool bMenu = false;
bool recording = false;
bool bMaster = false;
bool bSlave = false;
bool bIsFocus = false;

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

std::vector<const char *> botList = {"Spellwisp", "Rabbit", "General Combat", "Mining", "Clockwork Suit", "WoodCutting", "Anachronia Agility", "Abyss Crafting", "Watch Tower Agility", "Wilderness Agility", "Divination", "Fungal Mage", "Taverly Summoning", "Drop Monei", "Slayer Tower"};

std::vector<std::string> OreNode = { "Copper rock", "Tin rock", "Iron rock", "Coal", "Mithril rock", "Adamantite rock", "Runite rock", "Orichalcite rock" };
std::vector<std::string> OreName = { "Copper ore", "Tin ore", "Iron ore", "Coal", "Mithril ore", "Adamantite ore", "Runite ore", "Orichalcite ore" };

std::vector<std::string> TreeNames = { "Tree", "Oak", "Willow", "Teak", "Maple", "Acadia", "Mahogany", "Yew" };
std::vector<std::string> LogNames = { "Logs", "Oak logs", "Willow logs", "Teak logs", "Maple logs", "Acadia logs", "Mahogany logs", "Yew logs" };


void Mirroring(uint32_t param1 = 0, int32_t param2 = 0, int32_t param3 = 0, uint64_t function = 0)
{
	static uint32_t index = 0;

	DWORD  dwWaitResult = WaitForSingleObject(
		gMutex,    // handle to mutex
		200);  // no time-out interval

	switch (dwWaitResult)
	{
		// The thread got ownership of the mutex
	case WAIT_OBJECT_0:
		try {
			// MAP the Memory of the mirror action
			if (sharedMem)
			{
				if (bMaster)
				{
					index++;
					ActionMirror* action = new ActionMirror(index, param1, param2, param3, function);

					printf("Master sent %d %d %d %d %p\n", index, param1, param2, param3, function);

					memcpy((void*)sharedMem, action, sizeof(ActionMirror));

					delete action;
				}
				else if (bSlave)
				{
					static int slaveIndex = 0;

					ActionMirror* action = (ActionMirror*)sharedMem;

					//printf("Slave received %d %d %d %d %p\n", action->index, action->param1, action->param2, action->param3, action->function);

					// If we haven't execute this mirror action yet, then do it
					if (action->index > slaveIndex)
					{
						if (action->function > 0x10)
						{
							uint8_t data[100] = { 0 };

							*reinterpret_cast<int*>(&data[0x58]) = action->param1;
							*reinterpret_cast<int*>(&data[0x5c]) = action->param2;
							*reinterpret_cast<int*>(&data[0x60]) = action->param3;

							uint64_t func_ptr = action->function;

							if (func_ptr)
							{
								dataStruct dt;
								dt.dataPtr = data;

								printf("Slave is calling function %p\n", func_ptr);

								typedef void(__cdecl* _MirrorAction)(uint64_t* _this, void* dataPtr);
								reinterpret_cast<_MirrorAction>(func_ptr)(g_GameContext, &dt);

							}
							
						}
						else if (action->function == 0x1) // Key press
						{
							SendMessageW(hWnd, action->param1, action->param2, action->param3);
						}


						slaveIndex = action->index;

					}
					else if(slaveIndex != action->index)
					{
						slaveIndex = action->index;
					}

				}
			}
		}
		catch (...)
		{
			printf("Mirroring error occured %d\n", GetLastError());
		}


		if (!ReleaseMutex(gMutex))
		{
			// Handle error.
		}

		break;

		// The thread got ownership of an abandoned mutex
		// The database is in an indeterminate state
	case WAIT_ABANDONED:
	default:
		return;
		break;
	}
}

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

	if(bMaster)
		Mirroring(param1, param2, param3, (uint64_t)dispatcherFunc);
	
	return o_OnCursorDoAction(a1, actionPtr, position);
}

__int64* __fastcall h_GetVarpType(__int64 VarpWrap, __int64 VarpDomain, unsigned int varpid)
{
	if(bMenu)
		printf("Getvarp domain %p  varpid %d\n", VarpDomain, varpid);
	return o_GetVarpType(VarpWrap, VarpDomain, varpid);
}

__int64 __fastcall h_SetVarValueFromServer(__int64 Player, __int64 VarpType, VarpClass* a3)
{
	if (bMenu)
		printf("Varp value set %d\n", a3->Value);

	return o_SetVarpValueFromServer(Player, VarpType, a3);
}

int* h_ExecuteHookInner(UINT_PTR* GameContext, unsigned int* a2, int a3)
{
	g_GameContext = GameContext;


	return o_ExecuteHookInner(GameContext, a2, a3);
}

__int64* h_PrepareUUIDPacket(__int64* uuid_struct, __int64 packet)
{
	auto contextPtr = (GameContextPtr*)g_GameContext;

	if (uuid_struct)
	{
		BYTE* uuid = (BYTE*)((__int64)uuid_struct + 0xc8);

		if (contextPtr)
		{
			const char* email =contextPtr->gContext->LoginManager->Email;
			//printf("Email: %s\n", contextPtr->gContext->LoginManager->Email);
			//print_bytes("=== Original UUID ===", uuid, 24, true);


			word64* tiger_hash = new word64[3];
			tiger((word64*)email, strlen(email), tiger_hash);

			//print_bytes("=== Generated UUID ===", (byte*)tiger_hash, 24, true);

			memcpy(uuid, tiger_hash, 24);

			//print_bytes("=== New UUID ===", uuid, 24, true);

		}
	}

	return o_PrepareUUIDPacket(uuid_struct, packet);
}


UINT_PTR h_CursorWorldContextMenu(UINT_PTR* GameContext, int a2, int a3, int a4, int a5)
{
	g_GameContext = GameContext;

	// Botting
	//Manager::Manage();

	return o_CursorWorldConextMenu(GameContext, a2, a3, a4, a5);
}

std::map<int, HGLRC> contexts;

bool h_wglSwapBuffers(HDC hdc)
{
	// if all bot are disabled and suicide is set.
	if (to_suicide && Manager::get_current_bot() == _current_bot::None)
	{
		log("Terminating process now.");
		TerminateProcess(GetCurrentProcess(), 0);
		return 0;
	}

	if (bMenu)
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


		uint32_t width = 0, height = 0;

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
		glOrtho(0, width, height, 0, -1, 1);
		glMatrixMode(GL_MODELVIEW);

		CFont font(15, 0xff, 0, 0);


		if (SelectedBot == -1)
		{
			//r.PrintCStr(100.0f, 90.0f, 0x00, 0xFF, 0x00, std::string("Selected Bot: None").data());
			font.Print(100.f, 120.0f, "Selected Bot: None");
		}
		else
		{
			//std::string selectedBotName = std::string("Selected Bot: ") + std::string(botList[SelectedBot]);
			//r.PrintCStr(100.0f, 90.0f, 0x00, 0xFF, 0x00, selectedBotName.data());
			font.Print(100.f, 120.0f, botList[SelectedBot]);

			if (SelectedBot == (int)BotType::WoodCutting)
			{
				//std::string TreeName = std::string("Selected Tree: ") + std::string(TreeNames[SelectedWood]);
				//r.PrintCStr(100.0f, 110.0f, 44, 195, 212, TreeName.data());

				if (SelectedWood > -1)
					font.Print(100.f, 140.0f, TreeNames[SelectedWood].data());
				else
					font.Print(100.f, 140.0f, "Selected Log: None");

			}
			else if (SelectedBot == (int)BotType::Mining)
			{
				//std::string Ore = std::string("Selected Ore: ") + std::string(OreName[SelectedOre]);
				//r.PrintCStr(100.0f, 110.0f, 44, 195, 212, TreeName.data());

				if (SelectedOre > -1)
					font.Print(100.f, 140.0f, OreName[SelectedOre].data());
				else
					font.Print(100.f, 140.0f, "Selected Ore: None");

			}

		}

		//botStatus = botStatus + " break_type: " + std::to_string(break_type);

		font.Print(100.f, 200.0f, botStatus.data());

		if(bMaster)
			font.Print(100.f, 140.0f, "Master Mode Activated");
		if (bSlave)
			font.Print(100.f, 160.0f, "Slave Mode Activated");
		if (recording)
			font.Print(100.f, 180.0f, "Recording Mouse Data");

		glPopMatrix();
		glPopAttrib();

		wglMakeCurrent(oldhdc, oldctx);
	}


	if (bSlave)
		Mirroring();

	try {
		// Botting
		Manager::Manage();
	}
	catch (...)
	{

	}


	if (GetActiveWindow())
		bIsFocus = true;
	else
		bIsFocus = false;

	return o_wglSwapBuffers(hdc);
}


bool unloaded = false;

bool __stdcall Unload()
{
#ifdef _DEBUG
	HANDLE hLogFile;
	hLogFile = CreateFile(L"C:\\temp\\log.txt", GENERIC_WRITE,
		FILE_SHARE_WRITE, NULL, CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL, NULL);
	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
	_CrtSetReportFile(_CRT_WARN, hLogFile);
	_CrtDumpMemoryLeaks();
	CloseHandle(hLogFile);
#endif

	if (detours.Clearhook())
	{

		//unhook_function((PVOID&)o_GetVarpType, (PBYTE)h_GetVarpType, "GetVarpType");

		o_wglSwapBuffers = (fn_wglSwapBuffers)detour_iat_ptr("SwapBuffers", o_wglSwapBuffers, (HMODULE)HdnGetModuleBase("rs2client.exe"));

		SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG_PTR)OriginalWndProcHandler);

		delete[] biometric_data; biometric_data = 0;

		printf("[+]  Successfully unhook and unloaded\n");

		unloaded = true;

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


void UpdateTest()
{
	auto cplayer = RS::GetClosestPlayer();
	Tile2D localplayerPos = RS::GetLocalPlayerTilePos();
	printf("============================================\n");
	printf("PID: %d %x\n", GetCurrentProcessId(), GetCurrentProcessId());
	printf("GameContext: %p   --   Gamestate: %d\n", g_GameContext, RS::GetGameState());
	printf("LocalPlayer: %p\n",  RS::GetLocalPlayer());
	printf("EntityCount: %d\n", RS::GetEntityCount());
	printf("PlayerName: %s -- Current Target is: %d\n", RS::GetLocalPlayer()->Name, RS::GetLocalPlayer()->CurrentTarget);
	printf("LocalPlayer is currently on tile (%d, %d) with z being %f\n", localplayerPos.x, localplayerPos.y, RS::GetLocalPlayerPos()[1]);
	printf("LocalPlayer Animations: %d %d %d\n", cplayer->AnimationId, cplayer->CurrentAni, cplayer->UsefulAni);
	printf("Inventory Free Slot: %d  -- ", Inventory::GetFreeSlot());
	printf("Current first id is %d  -- ", Inventory::GetContainerObj(static_cast<uint32_t>(ContainerType::Backpack))->ContainerContent[0].ItemId);
	printf("Inventory Container: %p\n", Inventory::GetContainerObj(static_cast<uint32_t>(ContainerType::Backpack)));
	printf("Widget Conversation: %p\n", Widgets::GetWidgetUI(CONVERSATION_WIDGET));
	printf("Closest Player: %p\n", cplayer);

	if(cplayer)
		printf("Closest Player Info. Name: %s  X: %f  Y: %f Level: %d\n", cplayer->Name, cplayer->GetPos()[0], cplayer->GetPos()[2], cplayer->Level);

	auto cmonster = RS::GetClosestMonster();

	if (cmonster)
	{
		auto ent = Entity(cmonster);

		printf("Closest Monster Info. Name: %s  X: %f  Y: %f Level: %d (%p) with [%d / %d] health.\n", cmonster->Name, cmonster->GetPos()[0], cmonster->GetPos()[2], cmonster->Level, cmonster, ent.NPCCurHealth(), ent.NPCMaxHealth());
	}

	printf("Varp check: Health [%d] Prayer [%d]\n", Varpbit::GetVarpBit(1668), Varpbit::GetVarp(3274));

	printf("Exp for Health: [%d] HealthLvl [%d] Exp for Summoning: [%d]  Summoning level [%d]\n", Exp::GetCurrentExp(Stat::HITPOINTS), Exp::GetSkillLevel(Stat::HITPOINTS), Exp::GetCurrentExp(Stat::SUMMONING), Exp::GetSkillLevel(Stat::SUMMONING));

	printf("Current World [%d]  [%s]\n", Common::GetCurrentWorld(), Common::GetCurrentWorldString());
	printf("============================================\n");

}

static std::vector<std::pair<int, int>> varprange = { {5, 5},{5, 5} };
extern std::vector<std::pair<std::string, int>> ArcheologyCaches;

LRESULT CALLBACK hWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (false)
	{
		SetCursor((HCURSOR)LoadImageA(NULL, MAKEINTRESOURCEA(32515), IMAGE_CURSOR, 0, 0, LR_SHARED));
	}


	if (uMsg == WM_KEYUP)
	{

		Manager::Keystates(wParam);

		if (wParam == VK_F1)
		{

		}

		if (wParam == VK_END)
		{
			recording = !recording;

			// if we just disabled recording
			if (!recording)
			{
				time_t filename = std::time(0);

				std::string str_filename = "C:\\ProgramData\\Jagex\\launcher\\" + std::to_string(filename) + "_mouse" + ".bin";

				std::ofstream f(str_filename, std::ios::binary | std::ios::out);
			
				mouse_data[0].delay = 0;

				const char* pointer = reinterpret_cast<const char*>(&mouse_data[0]);

				size_t bytes = mouse_data.size() * sizeof(mouse_data[0]);

				uint32_t width = 0, height = 0;

				RECT r;
				if (GetClientRect(hWnd, &r))
				{
					width = r.right - r.left;
					height = r.bottom - r.top;
				}

				mouse_data_header header(height, width, mouse_data.size());

				printf("Writing to file %s %d\n", str_filename.data(), bytes);

				f.write((const char*)&header, sizeof mouse_data_header);

				f.write(pointer, bytes);

				f.close();
			}
		}

		if (wParam == VK_NUMPAD0)
		{
			printf("gcontext = %p\n", g_GameContext);

			printf("Gamestate: %d\n", RS::GetGameState());

		}

		if (wParam == VK_NUMPAD1)
		{
			try {
				//Common::Login("heph_yeet9@maildu.de", "poching29");
			}
			catch (...)
			{
				log("An exception happened.");
			}
		}

		if (wParam == VK_OEM_3)
		{
			bMenu = !bMenu;
		}

		if (wParam == VK_NUMPAD2)
		{
			UpdateTest();


			/*
			auto player = RS::GetLocalPlayerTilePos();
			agiList[agiList.size() - 1].EndPos = player;

			printf("Added (%d, %d) as player pos.\n", player.x, player.y);
			*/
		}

		if (RS::IsInGame())
		{

			if (wParam == VK_NUMPAD1)
			{
				/*
				int items = 0;

				auto storage = Inventory::GetContainerObj(95);
				if (storage)
				{
					auto content = storage->ContainerContent;
					for (int i = 0; i < Varpbit::GetVarp(MAX_BANK_SLOT); i++)
					{
						auto x = content[i];
						if (x.ItemId != -1 && x.ItemQuantity > 0)
							items++ ;
					}
				}
				*/

				//printf("found %d valid items\n", items);
				/*
				printf("std::vector<AgilityCourse> WildernessAgi = { ");
				for (auto i : agiList)
				{
					printf("AgilityCourse(%d, Tile2D(%d, %d)), ", i.objId, i.EndPos.x, i.EndPos.y);
				}
				printf(" };");
				*/
			}

			if (FALSE)
			{
				//Player player = RS::GetLocalPlayer();
				//player.Move(player.GetTilePosition());
				


				//Player player = RS::GetLocalPlayer();

				/*
				RecordAgility = !RecordAgility;

				printf("Record Agi: %d\n", RecordAgility);
				*/
			}
			if (wParam == VK_NUMPAD3)
			{
				std::string line;
				std::ifstream myfile("C:\\ProgramData\\Jagex\\launcher\\scan.txt");
				if (myfile.is_open())
				{
					getline(myfile, line);
					myfile.close();

					int value = std::stoi(line);


					printf("varp %d == %d %d\n", value, Varpbit::GetVarp(value), Varpbit::GetVarpBit(value));

				}
			}

			// New Scan
			if (wParam == VK_NUMPAD4)
			{
				std::string line;
				std::ifstream myfile("C:\\ProgramData\\Jagex\\launcher\\scan.txt");
				if (myfile.is_open())
				{
					getline(myfile, line);
					myfile.close();

					int value = std::stoi(line);


					varprange = Varpbit::ScanNewVarpValue(varprange, value);
				}
			}

			// Changed Varp

			if (wParam == VK_NUMPAD5)
			{
				std::string line;
				std::ifstream myfile("C:\\ProgramData\\Jagex\\launcher\\scan.txt");
				if (myfile.is_open())
				{
					getline(myfile, line);
					myfile.close();

					int value = std::stoi(line);


					if(value != 9999)
						varprange = Varpbit::ScanNextVarpValue(varprange, true, value);
					else
						varprange = Varpbit::ScanNextVarpValue(varprange, true);
				}
			}
			// VK6 is now used elsewhere!
			if (wParam == VK_NUMPAD6)
			{
				
				//varprange = Varpbit::ScanNextVarpValue(varprange, false);
			}

			if (wParam == VK_OEM_3)
			{
				//UINT_PTR* PlayerObj = *(UINT_PTR**)(g_GameContext[1] + 0x1780);
				//printf("Found local player players %p and %d entities %s.\n", RS::GetLocalPlayer(), RS::GetEntityCount(), RS::GetLocalPlayer()->Name);
			}
		}
		if (wParam == VK_CONTROL)
		{

		}

		if (wParam == VK_HOME)
			Unload();




		if (wParam == VK_PRIOR)
		{
			if (!bSlave)
				bMaster = !bMaster;
		}

		else if (wParam == VK_NEXT)
		{

			if (!bMaster)
				bSlave = !bSlave;
		}
	}

	if(bMaster && ((wParam >= 0x30 && wParam <= 0x5A ) || wParam == VK_RETURN))
		Mirroring(uMsg, wParam, lParam, 0x1);

	static bool isFocus = false;

	if (uMsg == WM_SETFOCUS)
	{
		// do not focus if we are already focused
		if (isFocus)
		{
			return 0;

		}

		isFocus = true;
	}


	if (uMsg == WM_KILLFOCUS)
	{
		// if we are not focused, and menu is not on -1 OR archeology is running, then kill all natural KILLFOCUS
		if (SelectedBot != -1 || archelogy || !isFocus)
		{
			return 0;
		}

		isFocus = false;
	}

	
	if (recording && (uMsg == WM_MOUSEMOVE || uMsg == WM_KILLFOCUS || uMsg == WM_SETFOCUS || wParam == VK_UP || wParam == VK_DOWN || wParam == VK_LEFT || wParam == VK_RIGHT))
	{
		antiban::mouse_record(uMsg, wParam, lParam);
	}
	

	return CallWindowProc(OriginalWndProcHandler, hWnd, uMsg, wParam, lParam);
}

/*

BOOL unhook_function(PVOID& t1, PBYTE t2, const char* s = NULL)
{
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourDetach(&t1, t2);
	if (DetourTransactionCommit() != NO_ERROR) {
		printf("[Hook] - Failed to unhook %s.\n", s);
		return false;
	}
	return true;
}

BOOL hook_function(PVOID& t1, PBYTE t2, const char* s = NULL)
{
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourAttach(&t1, t2);
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
}*/

extern bool to_suicide;

DWORD WINAPI biometric_thread(LPVOID lpParameter)
{
	while (!unloaded)
	{
		if (!bIsFocus && biometric_data && Manager::get_current_bot() != _current_bot::None)
		{
			uint32_t width = 0, height = 0;

			RECT r;
			if (GetClientRect(hWnd, &r))
			{
				width = r.right - r.left;
				height = r.bottom - r.top;
			}

			mouse_replay* bio = (mouse_replay*)biometric_data;

			antiban::mouse_playback(bio, height, width);
		}

		Sleep(5);
	}

	return 0;

}


DWORD WINAPI ipc_thread(LPVOID lpParam)
{
	while (!unloaded)
	{

		// 1/101 chance
		if (break_type == 0 && Manager::get_current_bot() != _current_bot::None)
		{
			int random = antiban::int_random_range(0, 90);

			log("random %d\n", random);

			if (random == 30)
			{
				log("hit.");

				antiban::anti_afk();
			}
		}

		if (ipc::client_shared_mem)
		{
			ipc::ipc_mutex_lock();
		}
		else
		{
			log("[ Critical ] client_share_mem invalid");
		}

		Sleep(500);
	}

	return 0;
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

	o_StartLogin = (fn_StartLogin)Patterns.Func_StartLogin;

	o_GetLocalPlayer = (fn_GetLocalPlayer)Patterns.Func_GetLocalPlayer;

	o_OnCursorDoAction = (fn_OnCursorDoAction)Patterns.Func_OnCursorDoAction;

	o_OnCursorDoAction = (fn_OnCursorDoAction)detours.Hook(o_OnCursorDoAction, h_OnCursorDoAction, 19);

	o_CursorWorldConextMenu = (fn_CursorWorldContextMenu)Patterns.Func_OnCursorWorldContextMenu;

	o_CursorWorldConextMenu = (fn_CursorWorldContextMenu)detours.Hook(o_CursorWorldConextMenu, h_CursorWorldContextMenu, 21);

	//o_SetVarpValueFromServer = (fn_SetVarpValueFromServer)detours.Hook(o_SetVarpValueFromServer, h_SetVarValueFromServer, 15);

	//hook_function((PVOID&)o_GetVarpType, (PBYTE)h_GetVarpType, "h_GetVarpType");

	o_ExecuteHookInner = (fn_ExecuteHookInner)Patterns.Func_ExecuteHookInner;

	o_ExecuteHookInner = (fn_ExecuteHookInner)detours.Hook(o_ExecuteHookInner, h_ExecuteHookInner, 14);

	o_PrepareUUIDPacket = (fn_PrepareUUIDPacket)Patterns.Func_PrepareUUIDPacket;

	o_PrepareUUIDPacket = (fn_PrepareUUIDPacket)detours.Hook(o_PrepareUUIDPacket, h_PrepareUUIDPacket, 18);

	//o_OnDispatchNetMessage = (fn_OnDispatchNetMessage)Patterns.Func_OnDispatchMessage;

	//o_OnDispatchNetMessage = (fn_OnDispatchNetMessage)detours.Hook(o_OnDispatchNetMessage, h_OnDispatchNetMessage, 18);

	o_wglSwapBuffers = (fn_wglSwapBuffers)detour_iat_ptr("SwapBuffers", h_wglSwapBuffers, (HMODULE)HdnGetModuleBase("rs2client.exe"));

	OriginalWndProcHandler = (WNDPROC)SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG_PTR)hWndProc);

	gMutex = CreateMutex(
		NULL,              // default security attributes
		FALSE,             // initially not owned
		L"Local\\svchost_srv");             // unnamed mutex

	if (gMutex == NULL)
	{
		printf("CreateMutex error: %d %d\n", GetLastError(), sizeof(ActionMirror));
		return 1;
	}

	hFileMap = CreateFileMapping(
		INVALID_HANDLE_VALUE,
		NULL,
		PAGE_READWRITE,
		0,
		sizeof(ActionMirror),
		L"Local\\svchost_srv_mem");

	if (!hFileMap)
	{
		printf("CreateFileMapping error: %d\n", GetLastError());
		return 0;
	}

	sharedMem = (uintptr_t)MapViewOfFile(hFileMap, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(ActionMirror));

	if (!sharedMem)
	{
		printf("MapViewOfFile failed with error: %d\n", GetLastError());
		return 0;
	}

	if (!ipc::ipc_init())
		printf("[!] Failed to initialize IPC.\n");

	std::ifstream mouse_data_file("C:\\ProgramData\\Jagex\\launcher\\mouse_data.bin", std::ios::binary | std::ios::ate);

	if (!mouse_data_file.fail())
	{
		size_t FileSize = static_cast<size_t>(mouse_data_file.tellg());

		biometric_data = new char[FileSize]();

		mouse_data_file.seekg(0, std::ios::beg);

		mouse_data_file.read(biometric_data, FileSize);

		printf("[+] Successfully Read the biometric file at %p with size %d.\n", biometric_data, FileSize);

		mouse_data_file.close();

		if(!CreateThread(0, 0, biometric_thread, 0, 0, 0))
		{
			log("[ Critical ] Biometric Create thread failed with error %d.", GetLastError());
		}
	}
	else
	{
		log("[ Medium ] Biometric file reading failed");
	}


	if (!CreateThread(0, 0, ipc_thread, 0, 0, 0))
	{
		log("[ Critical ] IPC Create thread failed with error %d.", GetLastError());
	}

#ifdef NDEBUG
	Beep(523, 200);
#endif

	return true;
}

