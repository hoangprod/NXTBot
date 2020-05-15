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


struct ipc
{
	static HANDLE client_mutex;
	static HANDLE client_map_file;
	static uintptr_t client_shared_mem;

	static DWORD get_client_ppid();

	static bool ipc_init();
	static bool ipc_register();
	static bool ipc_heartbeat();
	static int get_current_client_index(DWORD pid);

	static _bot_status get_bot_status();

	static void ipc_loop();
	static void ipc_mutex_lock();
	static bool ipc_cmd_manager();

};

