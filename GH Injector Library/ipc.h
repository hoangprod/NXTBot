#pragma once

enum class _server_cmd
{
	SUICIDE,
	AUTO_START,
	HEARTBEAT
};

enum class _bot_status
{
	OFF,
	ON,
	SHORT_BREAK,
	LONG_BREAK
};


enum class _game_state {
	LoginScreen = 10,
	Lobby = 20,
	Ingame = 30,
	Disconnected = 40,
	Reconnecting = 35,
	PleaseWait = 37,
	What = 666
};

static const char* EnumStrings[] = { "LOGIN", "LOBBY", "INGAME", "DISCONNECTED", "RECONNECTING", "PLEASE_WAIT", "NANIDAFUCK" };

struct _account_info
{
	_account_info()
	{

	}
	_account_info(char* e, char* p)
	{
		strcpy_s(email, e);
		strcpy_s(password, p);
	}
	char email[40];
	char password[30];
};

struct _server_msg
{
	int client_pid;
	bool is_acknowledged;
	_server_cmd cmd;
	_account_info acc_info;
};

struct _client_status
{
	_bot_status status;
	_account_info acc_info;
	_game_state gamestate;
};

struct _internal_bot_list
{
	_internal_bot_list(char* u, char* p, uint64_t t)
	{
		strcpy_s(acc_info.email, u);
		strcpy_s(acc_info.password, p);

		total_play_time = t;
	}
	uint64_t total_play_time;
	_account_info acc_info;
};

enum class _current_bot
{
	None,
	Penguin_Agility,
	Anachronia_Agility,
	Abyss_Crafting,
	Archeology,
	Watchtower_Agility,
	Wilderness_Agility,
	Divination,
	Fungal_Mage,
	Taverly_Summoning,
	Money_Drop,
	Wisp_Farming,
	General_Combat,
	Rabbit_Farming,
	Woodcutting,
	General_Mining
};

struct _Client
{
	int pid;
	int ppid;
	int last_action_time;
	uint64_t last_heart_beat;
	uint64_t process_create_time;
	_current_bot current_bot;
	_client_status client_msg;
};

struct _bot_ipc
{
	_server_msg s2c;
	_Client clients[16];
};

void ipc_loop();
DWORD WINAPI inject(DWORD pidz);
void clean_up_crashed_processes();
ULONGLONG get_process_creation_time(DWORD pid);
void acknowledgement_handler(_server_msg srv_msg);
std::vector<DWORD> GetPIDs(std::wstring targetProcessName);

#define log( format, ... ) printf("[ s2c ] " format "\n", ##__VA_ARGS__ )


