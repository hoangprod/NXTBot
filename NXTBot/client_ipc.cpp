#include "pch.h"
#include "GameClasses.h"
#include "Game.h"
#include "PlayableEntity.h"
#include "Manager.h"
#include "client_ipc.h"

bool to_suicide = false;

extern bool break_type;
extern UINT_PTR* g_GameContext;

HANDLE ipc::client_mutex = NULL;
HANDLE ipc::client_map_file = NULL;
uintptr_t ipc::client_shared_mem = NULL;

bool auto_start = false;
std::string auto_username;
std::string auto_password;


DWORD ipc::get_client_ppid()
{
	HANDLE hSnapshot;
	PROCESSENTRY32 pe32;
	DWORD ppid = 0, pid = GetCurrentProcessId();

	hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	__try {
		if (hSnapshot == INVALID_HANDLE_VALUE) __leave;

		ZeroMemory(&pe32, sizeof(pe32));
		pe32.dwSize = sizeof(pe32);
		if (!Process32First(hSnapshot, &pe32)) __leave;

		do {
			if (pe32.th32ProcessID == pid) {
				ppid = pe32.th32ParentProcessID;
				break;
			}
		} while (Process32Next(hSnapshot, &pe32));

	}
	__finally {
		if (hSnapshot != INVALID_HANDLE_VALUE) CloseHandle(hSnapshot);
	}
	return ppid;
}

bool ipc::ipc_init()
{
	client_mutex = CreateMutexW(
		NULL,              // default security attributes
		FALSE,             // initially not owned
		L"Local\\svchost_ipc");             // unnamed mutex

	if (client_mutex == NULL)
	{
		printf("CreateMutex error: %d %lld\n", GetLastError(), sizeof(_bot_ipc));
		return false;
	}

	client_map_file = CreateFileMappingW(
		INVALID_HANDLE_VALUE,
		NULL,
		PAGE_READWRITE,
		0,
		sizeof(_bot_ipc),
		L"Local\\svchost_ipc_mem");

	if (!client_map_file)
	{
		printf("CreateFileMapping error: %d\n", GetLastError());
		return false;
	}

	client_shared_mem = (uintptr_t)MapViewOfFile(client_map_file, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(_bot_ipc));

	if (!client_shared_mem)
	{
		printf("MapViewOfFile failed with error: %d\n", GetLastError());
		return false;
	}

	log("Successfully initialized ipc.");

	return true;
}

// Return our index in the shared memory, returns -1 if we are not in memory
int ipc::get_current_client_index(DWORD pid = 0)
{
	if (client_shared_mem)
	{
		_bot_ipc* botipc = (_bot_ipc*)client_shared_mem;

		for (int i = 0; i < 16; i++)
		{
			_Client client = botipc->clients[i];

			if (client.pid && (client.pid == pid || client.pid == GetCurrentProcessId()))
			{
				return i;
			}
		}

		return -1;
	}

	log("[ CRITICAL ] - get_current_client_index cannot access manager_shared_mem.");

	return -1;
}

_bot_status ipc::get_bot_status()
{
	switch (break_type)
	{
	case 0:
		if (Manager::get_current_bot() != _current_bot::None)
			return _bot_status::ON;

		return _bot_status::OFF;
		break;
	case 1:
		return _bot_status::SHORT_BREAK;
		break;
	case 2:
		return _bot_status::LONG_BREAK;
		break;
	default:
		break;
	}
	
	log("get_bot_status received an invalid break_type.");
	return _bot_status::OFF;
}

bool ipc::ipc_register()
{
	if (client_shared_mem)
	{
		_bot_ipc* botipc = (_bot_ipc*)client_shared_mem;

		for (int i = 0; i < 16; i++)
		{
			_Client * client = &botipc->clients[i];

			if (!client->pid)
			{
				client->pid = GetCurrentProcessId();
				client->ppid = ipc::get_client_ppid();
				client->client_msg.gamestate = RS::GetGameState();
				log("Registered at index %d with pid %d\n", i, GetCurrentProcessId());
				return true;
			}
		}

		log("[ High ] - ran out of space to register this client.");
		return false;
	}

	log("[ CRITICAL ] - existed_in_sharedmemory cannot access manager_shared_mem.");

	return false;
}


bool ipc::ipc_heartbeat()
{
	int idx = get_current_client_index();

	if (idx > -1)
	{
		if (client_shared_mem)
		{
			_bot_ipc* botipc = (_bot_ipc*)client_shared_mem;

			_Client *client = &botipc->clients[idx];
			
			if (RS::GetGameState() == _game_state::Lobby || RS::GetGameState() == _game_state::Ingame)
			{
				auto contextPtr = (GameContextPtr*)g_GameContext;

				if (contextPtr)
				{
					client->pid = GetCurrentProcessId();
					client->current_bot = Manager::get_current_bot();
					client->last_heart_beat = GetTickCount64();
					client->client_msg.gamestate = RS::GetGameState();
					client->client_msg.status = ipc::get_bot_status();

					strcpy_s(client->client_msg.acc_info.email, contextPtr->gContext->LoginManager->Email);
					strcpy_s(client->client_msg.acc_info.password, contextPtr->gContext->LoginManager->Password);

					if (RS::GetGameState() == _game_state::Ingame)
					{
						Player player = RS::GetLocalPlayer();

						if (player._base)
						{
							client->last_action_time = player.GetElapsedSecondSinceLastAction();
							return true;
						}

						log("[ Medium ] - Player is not valid even though we are ingame.");
						return false;

					}

					return true;
				}

				log("[ High ] - ipc_heartbeat cannot access g_GameContext.");
				return false;
			}

			//log("[ Info ] - Client is not in lobby / ingame.");
			return true;
		}

		log("[ CRITICAL ] - ipc_heartbeat cannot access manager_shared_mem.");

		return false;
	}

	log("[ High ] - ipc_heartbeat cannot find our client in shared memory.");

	return false;
}


// Bread and butter
bool ipc::ipc_cmd_manager()
{
	if (client_shared_mem)
	{
		_bot_ipc* botipc = (_bot_ipc*)client_shared_mem;

		_server_msg *s2c_cmd = &botipc->s2c;

		// if message is meant for us
		if (s2c_cmd->client_pid && s2c_cmd->client_pid == GetCurrentProcessId() && !s2c_cmd->is_acknowledged)
		{
			switch (s2c_cmd->cmd)
			{
			case _server_cmd::SUICIDE:
				log("[c] Received command to commit seppuku.");
				s2c_cmd->is_acknowledged = true;
				if (!ReleaseMutex(client_mutex))
				{
					log("Could not release mutex.");
				}
				to_suicide = true;
				return true;
				break;
			case _server_cmd::AUTO_START:
				auto_username = s2c_cmd->acc_info.email;
				auto_password = s2c_cmd->acc_info.password;
				auto_start = true;
				log("[c] Received command to auto start with username %s.", s2c_cmd->acc_info.email);
				break;
			case _server_cmd::HEARTBEAT:
				ipc::ipc_heartbeat();
				log("[c] Received command to send a heartbeat.");
				break;
			default:
				break;
			}

			s2c_cmd->is_acknowledged = true;
			return true;
		}
		
		// server msg not meant for us
		return false;
	}

	log("[ CRITICAL ] - ipc_cmd_manager cannot access manager_shared_mem.");

	return false;
}


void ipc::ipc_loop()
{
	if (!RS::GetGameContext())
	{
		log("GameContext is not ready.");
		return;
	}

	int idx = get_current_client_index();

	if (get_current_client_index() == -1)
	{
		ipc::ipc_register();
	}

	if (!ipc_heartbeat())
		log("Failed to send heartbeat message.");

	ipc_cmd_manager();
}



void ipc::ipc_mutex_lock()
{
	if (to_suicide)
	{
		if (!ReleaseMutex(client_mutex))
		{
			log("Could not release mutex.");
		}

		return;
	}

	DWORD  dwWaitResult = WaitForSingleObject(client_mutex, 100);

	switch (dwWaitResult)
	{
	case WAIT_ABANDONED:
		log("[ High ] Wait abandoned. Memory might be in a corrupted state.");
	case WAIT_OBJECT_0:
		try {
			ipc::ipc_loop();
		}
		catch (...)
		{
			log("Failed into try/catch.");
		}

		if (!ReleaseMutex(client_mutex))
		{
			log("Could not release mutex.");
		}

		break;
	case WAIT_TIMEOUT:
		//log("Wait timeout.");
		break;
	default:
		log("Wait result: %d", dwWaitResult);
		break;
	}
}
