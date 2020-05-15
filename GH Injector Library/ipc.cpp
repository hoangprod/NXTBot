#include "pch.h"
#include "Injection.h"
#include "csv.h"
#include "ipc.h"

bool kill_all = false;
int max_clients = 0;
int last_cmd_pid = 0;
_server_cmd last_cmd;
HANDLE manager_mutex;
HANDLE manager_map_file;
uintptr_t manager_shared_mem;

std::vector<_internal_bot_list> account_list;

std::vector<DWORD> GetPIDs(std::wstring targetProcessName) {
	std::vector<DWORD> pids;
	if (targetProcessName == L"")
		return pids; // No process name given
	HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); // All processes
	PROCESSENTRY32W entry; // Current process
	entry.dwSize = sizeof entry;
	if (!Process32FirstW(snap, &entry)) // Start with the first in snapshot
		return pids;
	do {
		if (std::wstring(entry.szExeFile) == targetProcessName)
			pids.emplace_back(entry.th32ProcessID); // Names match, add to list
	} while (Process32NextW(snap, &entry)); // Keep going until end of snapshot
	CloseHandle(snap);
	return pids;
}

DWORD WINAPI inject(DWORD pid)
{
	DWORD Err = 0;
	DWORD Ret = 0;

	HINSTANCE out;
	const wchar_t* szDll = L"NXTBot.dll";

	printf("[+] Found new client with pid %d\n", pid);

	HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, false, pid);

	// Let it load first

	printf("[+] Waiting for runescape client with pid %d to load.\n", pid);

	Sleep(1000);

	if (!hProc)
	{
		std::cout << "[!] Error Opening Process => " << "0x" << std::hex << hProc << std::endl;
		return 0;
	}

	Ret = _ManualMap(szDll, hProc, LAUNCH_METHOD::LM_HijackThread, INJ_ERASE_HEADER, out, Err);


	if (Err || Ret)
		printf("[!] Error %d - %d when injecting.\n", Err, Ret);
	else
	{
		std::cout << "[+] Injected into runescape with handle => " << "0x" << std::hex << hProc << "\n";
		std::cout << "[+] HModule of DLL is => " << "0x" << std::hex << out << "\n";
	}

	if (Ret)
	{
		std::cout << "[!] Error Injecting into => " << "0x" << std::hex << pid << std::endl;
	}

	CloseHandle(hProc);

	printf("==========================================\n");

	return 1;
}


bool initialize_ipc()
{
	manager_mutex = CreateMutexW(
		NULL,              // default security attributes
		FALSE,             // initially not owned
		L"Local\\svchost_ipc");             // unnamed mutex

	if (manager_mutex == NULL)
	{
		printf("CreateMutex error: %d %lld\n", GetLastError(), sizeof(_bot_ipc));
		return false;
	}

	manager_map_file = CreateFileMappingW(
		INVALID_HANDLE_VALUE,
		NULL,
		PAGE_READWRITE,
		0,
		sizeof(_bot_ipc),
		L"Local\\svchost_ipc_mem");

	if (!manager_map_file)
	{
		printf("CreateFileMapping error: %d\n", GetLastError());
		return false;
	}

	manager_shared_mem = (uintptr_t)MapViewOfFile(manager_map_file, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(_bot_ipc));

	if (!manager_shared_mem)
	{
		printf("MapViewOfFile failed with error: %d\n", GetLastError());
		return false;
	}

	return true;
}


bool start_rs_client()
{
	STARTUPINFOW si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	// Start the child process. 
	if (!CreateProcessW(
		(wchar_t*)L"C:\\Program Files\\Jagex\\RuneScape Launcher\\RuneScape.exe",   // No module name (use command line)
		NULL,        // Command line
		NULL,           // Process handle not inheritable
		NULL,           // Thread handle not inheritable
		FALSE,          // Set handle inheritance to FALSE
		0,              // No creation flags
		NULL,           // Use parent's environment block
		NULL,           // Use parent's starting directory 
		&si,            // Pointer to STARTUPINFO structure
		&pi)           // Pointer to PROCESS_INFORMATION structure
		)
	{
		printf("CreateProcess failed (%d).\n", GetLastError());
		return false;
	}

	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	return true;
}

bool does_pid_exist(DWORD pid)
{
	HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); // All processes
	PROCESSENTRY32W entry; // Current process
	entry.dwSize = sizeof entry;
	if (!Process32FirstW(snap, &entry)) // Start with the first in snapshot
		return false;
	do {
		if (entry.th32ProcessID == pid)
			return true;
	} while (Process32NextW(snap, &entry)); // Keep going until end of snapshot
	CloseHandle(snap);

	return false;
}

void cleanup_client_list()
{
	if (manager_shared_mem)
	{
		_bot_ipc* botipc = (_bot_ipc*)manager_shared_mem;

		// if last command IS acknowledged, clear the command memory to send a new one.
		if (botipc->s2c.client_pid && botipc->s2c.is_acknowledged)
		{
			last_cmd_pid = botipc->s2c.client_pid;
			acknowledgement_handler(botipc->s2c);
			RtlZeroMemory(&botipc->s2c, sizeof _server_msg);
		}

		for (int i = 0; i < 16; i++)
		{
			_Client *client = &botipc->clients[i];

			if (client->pid)
			{
				if (!does_pid_exist(client->pid))
				{
					HANDLE handle = OpenProcess(PROCESS_TERMINATE, false, client->ppid);

					if (handle)
						TerminateProcess(handle, 0);

					RtlZeroMemory(client, sizeof _Client);
				}
				
				// if client exist but we have not filled in their creation time, do it now
				else if(!client->process_create_time)
				{
					client->process_create_time = get_process_creation_time(client->pid);
				}
			}
		}
	}
}

bool start_clients()
{
	auto rs_launchers = GetPIDs(L"RuneScape.exe");

	if (rs_launchers.size() >= max_clients)
		return true;

	int clients_to_start = max_clients - rs_launchers.size();

	for (int i = 0; i < clients_to_start; i++)
	{
		if (!start_rs_client())
			return false;
	}

	return true;
}

bool existed_in_sharedmemory(DWORD pid)
{
	if (manager_shared_mem)
	{
		_bot_ipc* botipc = (_bot_ipc*)manager_shared_mem;

		for (int i = 0; i < 16; i++)
		{
			_Client *client = &botipc->clients[i];

			//log("client pid = %d %x\n", client->pid, client->pid);

			if (client->pid && client->pid == pid)
			{
				return true;
			}
		}

		return false;
	} 

	log("[ CRITICAL ] - existed_in_sharedmemory cannot access manager_shared_mem.");

	return false;
}


ULONGLONG get_process_creation_time(DWORD pid)
{
	FILETIME createTime;
	FILETIME exitTime;
	FILETIME kernelTime;
	FILETIME userTime;

	HANDLE pHande = OpenProcess(PROCESS_QUERY_INFORMATION, NULL, pid);

	if (pHande && GetProcessTimes(pHande,
		&createTime, &exitTime, &kernelTime, &userTime) != -1)
	{
		CloseHandle(pHande);

		return (((ULONGLONG)createTime.dwHighDateTime) << 32) + createTime.dwLowDateTime;
	}

	if (pHande)
		CloseHandle(pHande);

	return 0;
}

// Get total runtime of a process in _double_ SECOND
double get_process_runtime(DWORD pid, uint64_t creation_time = 0)
{
	if(!creation_time)
		creation_time = get_process_creation_time(pid);

	if (!creation_time)
	{
		log("[ High ] Failed to get process creation time for runtime calculation.");
		return 0.0f;
	}

	FILETIME cur_file_time;
	SYSTEMTIME cur_system_time;

	GetSystemTime(&cur_system_time);

	if (SystemTimeToFileTime(&cur_system_time, &cur_file_time))
	{
		ULONGLONG cur_time = (((ULONGLONG)cur_file_time.dwHighDateTime) << 32) + cur_file_time.dwLowDateTime;

		ULONGLONG dif_time = (cur_time - creation_time);

		auto res = static_cast<double>(dif_time) / _SECOND;

		return res;
	}

	log("[ Critical ] Failed to get process_run_time.");

	return 0.0f;
}


void inject_to_rs2clients()
{
	auto rs2clients = GetPIDs(L"rs2client.exe");

	for (auto rs2client : rs2clients)
	{
		// Client is already botting / injected
		if (existed_in_sharedmemory(rs2client))
			continue;

		// if client have not ran for at least 30 seconds
		if (get_process_runtime(rs2client) < 5.0f)
			continue;

		int result = inject(rs2client);

		if (result)
			log("[ Info ] Successfully injected into pid %d.", rs2client);
		else
			log("[ High ] Failed to inject into pid %d", rs2client);
	}

	return;
}


void send_server_cmd(_bot_ipc* botipc, int reciepient_pid, _server_cmd cmd_type, _account_info *info = 0)
{
	if (!botipc)
	{
		log("botipc is null.");
		return;
	}

	if (!reciepient_pid)
	{
		log("Recipient PID is null.");
		return;
	}

	last_cmd = cmd_type;

	// make sure to reset is_acknowledged
	botipc->s2c.is_acknowledged = false;

	switch (cmd_type)
	{
	case _server_cmd::AUTO_START:
		if (!info || !strlen(info->email) || !strlen(info->password))
		{
			log("Cannot auto_start w/o filling in _account_info.");
			break;
		}

		botipc->s2c.client_pid = reciepient_pid;
		botipc->s2c.cmd = cmd_type;
		strcpy_s(botipc->s2c.acc_info.email, info->email);
		strcpy_s(botipc->s2c.acc_info.password, info->password);
		break;
	case _server_cmd::SUICIDE:
		botipc->s2c.client_pid = reciepient_pid;
		botipc->s2c.cmd = cmd_type;
		break;
	case _server_cmd::HEARTBEAT:
		botipc->s2c.client_pid = reciepient_pid;
		botipc->s2c.cmd = cmd_type;
		break;
	}
}


// Get us some random, potentally big numbers, used for getting the seconds before switching account
uint64_t int_random_range(uint64_t min, uint64_t max, ULONGLONG seed)
{
	std::mt19937 mt(seed);
	std::uniform_int_distribution<int> dist(min, max);

	return dist(mt);
}

bool is_client_exhausted(DWORD pid)
{
	auto creation_time = get_process_creation_time(pid);

	double runtime = get_process_runtime(pid);

	// TODO: Change this back to 5-8
	uint64_t max_run_time = int_random_range(7 * HOUR_, 12 * HOUR_, creation_time);

	if (static_cast <uint64_t>(std::floor(runtime)) >= max_run_time)
		return true;
	
	return false;
}


_Client * get_running_client_by_pid(DWORD pid)
{
	if (manager_shared_mem)
	{
		_bot_ipc* botipc = (_bot_ipc*)manager_shared_mem;

		for (int i = 0; i < 16; i++)
		{
			_Client *client = &botipc->clients[i];

			if (pid == client->pid)
				return client;
		}
	}

	log("[ CRITICAL ] - get_running_client_by_pid cannot access manager_shared_mem.");

	return 0;
}

// Wrap these in its own class later
void heartbeat_ack_handler(_server_msg srv_msg)
{
	log("Client %d is acknowledging heartbeat request.", srv_msg.client_pid);
}

void auto_start_ack_handler(_server_msg srv_msg)
{
	log("Client %d is logging in with account %s.", srv_msg.client_pid, srv_msg.acc_info.email);
}

void suicide_ack_handler(_server_msg srv_msg)
{
	_Client* client = get_running_client_by_pid(srv_msg.client_pid);

	if (client)
	{
		uint64_t play_time = static_cast <uint64_t>(std::floor(get_process_runtime(srv_msg.client_pid, client->process_create_time)));

		for (auto acc : account_list)
		{
			// if the acknowledging account is already part of our internal list
			if (strcmp(client->client_msg.acc_info.email, acc.acc_info.email) == 0)
			{
				acc.total_play_time += play_time;

				log("[ Old ] Client %d with account %s is commiting sucide after %.2f hours of playtime, resulting in total %.2f hours.", srv_msg.client_pid, srv_msg.acc_info.email, (double)play_time / (int64_t)HOUR_, (float)acc.total_play_time / (int64_t)HOUR_);

				return;
			}
		}

		// if the acknowledging account is NOT in our internal list, add it to the list
		account_list.emplace_back(_internal_bot_list(client->client_msg.acc_info.email, client->client_msg.acc_info.password, play_time));

		// TODO: Bug here, srv_msg.acc_info.email for some reason by this time is already wiped out.
		log("[ New ] Client %d with account %s is commiting sucide after %.2f hours of playtime.", srv_msg.client_pid, (*srv_msg.acc_info.email) ? srv_msg.acc_info.email : "None", (double)play_time / (int64_t)HOUR_);
		return;
	}

	log("[ CRITICAL ] Acknowledged client is not part of the ipc hive.");

}

void acknowledgement_handler(_server_msg srv_msg)
{
	log("Receiving acknowledgement.");

	switch (srv_msg.cmd)
	{
	case _server_cmd::AUTO_START:
		auto_start_ack_handler(srv_msg);
		break;
	case _server_cmd::SUICIDE:
		suicide_ack_handler(srv_msg);
		break;
	case _server_cmd::HEARTBEAT:
		heartbeat_ack_handler(srv_msg);
		break;
	default:
		break;
	}
}

bool is_account_playing(const char * email)
{
	if (manager_shared_mem)
	{
		_bot_ipc* botipc = (_bot_ipc*)manager_shared_mem;

		for (int i = 0; i < 16; i++)
		{
			_Client *client = &botipc->clients[i];

			if (strcmp(email, client->client_msg.acc_info.email) == 0)
				return true;
		}

		return false;
	}

	log("[ CRITICAL ] - is_account_playing cannot access manager_shared_mem.");
}

bool does_account_exist_in_internal_list(const char* email)
{
	for (auto acc : account_list)
	{
		// if the acknowledging account is already part of our internal list
		if (strcmp(email, acc.acc_info.email) == 0)
		{
			return true;
		}
	}

	return false;
}

_internal_bot_list get_account_from_internal_list(const char* email)
{
	for (auto acc : account_list)
	{
		if (strcmp(acc.acc_info.email, email) == 0)
			return acc;
	}

	log("[ Critical ] get_account_from_internal_list cannot find email.");
}

bool is_lowest_playtime(const char* email)
{
	// make sure the acc we are checking IS in our internal list
	if (!does_account_exist_in_internal_list(email))
	{
		log("[ Critical ] is_lowest_playtime received an account that is not in our internal list.");
		return true; // SHOULD NEVER HAPPEN
	}

	_internal_bot_list cur_acc = get_account_from_internal_list(email);

	for (auto acc : account_list)
	{
		// Ignore accs that are still playing
		if (is_account_playing(acc.acc_info.email))
		{
			log("%s is playing so we will ignore it in our check.", acc.acc_info.email);
			continue;
		}


		// if current selected acc playtime is higher than ANY acc, return false
		if (cur_acc.total_play_time > acc.total_play_time)
		{
			log("cur_acc.total_play_time %s  %lld > acc.total_play_time %s  %lld", cur_acc.acc_info.email, cur_acc.total_play_time, acc.acc_info.email, acc.total_play_time);
			return false;
		}
	}

	// if we did not hit that previous if_statement at all, then this acc IS the lowest playtime
	return true;
}

_account_info* select_next_account()
{
	if (manager_shared_mem)
	{
		_bot_ipc* botipc = (_bot_ipc*)manager_shared_mem;

		io::CSVReader<2> in("accounts.csv");
		in.read_header(io::ignore_extra_column, "username", "password");
		std::string username; std::string password;
		while (in.read_row(username, password))
		{
			if (is_account_playing(username.data()))
				continue;

			// the if that will save us from all the troubles later
			else if (!does_account_exist_in_internal_list(username.data()))
			{
				log("[ Info ] Selecting an account that have not been played during this session.");
				return new _account_info((char*)username.data(), (char*)password.data());
			}
		}

		log("[ Info ] Switching to using account with lowest play-time.");

		io::CSVReader<2> in_2("accounts.csv");
		in_2.read_header(io::ignore_extra_column, "username", "password");

		while (in_2.read_row(username, password))
		{
			log("[ Info ] Checking on account %s", username.data());

			if (is_account_playing(username.data()))
				continue;

			// if account is the lowest play time, select that account
			else if (is_lowest_playtime(username.data()))
			{
				log("[ Info ] Selecting an %s as the account that have the least known play time.", username.data());
				return new _account_info((char*)username.data(), (char*)password.data());
			}
		}

		log("[ High ] select_next_account cannot find a valid acc.");
		return 0;
	}

	log("[ CRITICAL ] select_next_account cannot access manager_shared_mem.");
	return 0;
}

// TODO: make botting class/struct have an support_resume flag
bool is_resume_supported_bot(_current_bot current_bot)
{
	switch (current_bot)
	{
	case _current_bot::None:
		return false;
		break;
	case _current_bot::Penguin_Agility:
		return false;
		break;
	case _current_bot::Anachronia_Agility:
		return true;
		break;
	case _current_bot::Abyss_Crafting:
		return true;
		break;
	case _current_bot::Archeology:
		return true;
		break;
	case _current_bot::Watchtower_Agility:
		return true;
		break;
	case _current_bot::Wilderness_Agility:
		return false;
		break;
	case _current_bot::Divination:
		return true;
		break;
	case _current_bot::Fungal_Mage:
		return true;
		break;
	case _current_bot::Taverly_Summoning:
		return false;
		break;
	case _current_bot::Money_Drop:
		return false;
		break;
	case _current_bot::Wisp_Farming:
		return true;
		break;
	case _current_bot::General_Combat:
		return false;
		break;
	case _current_bot::Rabbit_Farming:
		return false;
		break;
	case _current_bot::Woodcutting:
		return false;
		break;
	case _current_bot::General_Mining:
		return false;
		break;
	default:
		return false;
		break;
	}

}


// Bread and butter
void command_clients()
{
	if (manager_shared_mem)
	{
		_bot_ipc* botipc = (_bot_ipc*)manager_shared_mem;

		// if last command is not acknowledged, wait for it to be acknowledged.
		if (botipc->s2c.client_pid && !botipc->s2c.is_acknowledged && does_pid_exist(botipc->s2c.client_pid))
		{
			log("[ Info ] Previous command not acknowledged.");
			return;
		}

		// if it didn't acknowledge but the process is also dead, then just zero the command out
		if (!botipc->s2c.is_acknowledged && !does_pid_exist(botipc->s2c.client_pid))
		{
			log("[ Info ] Previous client is orphaned.");
			RtlZeroMemory(&botipc->s2c, sizeof _server_msg);
		}

		for (int i = 0; i < 16; i++)
		{
			_Client *client = &botipc->clients[i];

			// if this array object is valid and process is running
			if (client->pid && does_pid_exist(client->pid))
			{
				// I can't see a situation where we have to command a client twice in a row.
				if (!kill_all && client->pid == last_cmd_pid)
				{
					//log("0 - Already commanded %d to do something recently, skipping.", client->pid);
					continue;
				}

				// If you are ingame, botting, and have not done anything for 5+ minutes, kill yourself
				if (client->client_msg.gamestate == _game_state::Ingame && client->client_msg.status == _bot_status::ON && client->last_action_time > 5 * MINUTE_ && is_resume_supported_bot(client->current_bot))
				{
					log("1 - Command PID %d to commit suicide.", client->pid);
					return send_server_cmd(botipc, client->pid, _server_cmd::SUICIDE);
				}

				// If you are in game and botting for too long already, it is time to rest brother.
				else if (client->client_msg.gamestate == _game_state::Ingame && client->client_msg.status == _bot_status::ON && is_client_exhausted(client->pid) && is_resume_supported_bot(client->current_bot))
				{
					log("2 - Command PID %d to commit suicide.", client->pid);
					return send_server_cmd(botipc, client->pid, _server_cmd::SUICIDE);
				}

				// If you are in game for more than 22 hours, it is time to take a break, ur bot is probably not in a good state
				else if (static_cast<uint64_t>(floor(get_process_runtime(client->pid))) > static_cast<uint64_t>(22) * HOUR_)
				{
					log("3 - Command PID %d to commit suicide.", client->pid);
					return send_server_cmd(botipc, client->pid, _server_cmd::SUICIDE);
				}
				// If you are in lobby + not botting + last heartbeat was over a minute ago
				else if (client->client_msg.gamestate == _game_state::Lobby && client->client_msg.status == _bot_status::OFF && (GetTickCount64() - client->last_heart_beat) > 60000)
				{
					if (TerminateProcess(OpenProcess(PROCESS_TERMINATE, NULL, client->pid), 0))
						log("4 - Forced PID %d to commit suicide.", client->pid);
					else
						log("4 - Failed to force close pid %d.", client->pid);
					return;
				}
				// If you are injected but is not botting and at login screen
				else if (!kill_all && client->client_msg.gamestate == _game_state::LoginScreen && client->client_msg.status == _bot_status::OFF)
				{
					auto next_account = select_next_account();

					if (next_account)
					{
						log("5 - Command PID %d to auto start with account %s.", client->pid, next_account->email);
						send_server_cmd(botipc, client->pid, _server_cmd::AUTO_START, next_account);
						delete next_account; next_account = 0; // select_next_account uses "new"
						return;
					}

				}
				else if (kill_all)
				{
					log("6 - Command PID %d to commit suicide.", client->pid);
					return send_server_cmd(botipc, client->pid, _server_cmd::SUICIDE);
				}

			}
		}

		return;
	}

	log("[ High ] command_clients could not get access to manager_shared_mem.");

}

const char* getTextForEnum(_game_state enumVal)
{
	return EnumStrings[(int)enumVal];
}

void clear_console() {
	COORD topLeft = { 0, 0 };
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO screen;
	DWORD written;

	GetConsoleScreenBufferInfo(console, &screen);
	FillConsoleOutputCharacterA(
		console, ' ', screen.dwSize.X * screen.dwSize.Y, topLeft, &written
	);
	FillConsoleOutputAttribute(
		console, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE,
		screen.dwSize.X * screen.dwSize.Y, topLeft, &written
	);
	SetConsoleCursorPosition(console, topLeft);
}

void print_list()
{
	if (manager_shared_mem)
	{
		_bot_ipc* botipc = (_bot_ipc*)manager_shared_mem;

		clear_console();

		printf("--------------------- [ BOT STATS ] ---------------------\n");

		printf("%-30s %10s %10s %10s %10s %13s\n", "EMAIL", "PID", "PPID", "GAMESTATE", "BOTSTATUS", "LAST_HEART_BEAT");
		printf("----------------------------------------------------------------------------\n");

		for (int i = 0; i < 16; i++)
		{
			_Client *client = &botipc->clients[i];

			printf("%-30s %10d %10d %10d %10d %10.2fs ago\n", (*client->client_msg.acc_info.email) ? client->client_msg.acc_info.email : "None", 
				client->pid, client->ppid, client->client_msg.gamestate, client->client_msg.status, (client->last_heart_beat) ? ((GetTickCount64() - client->last_heart_beat) / (float)1000) : 0);
		}

		printf("--------------------- [ PLAY TIME ] ---------------------\n");

		printf("%-30s %10s %10s %10s\n", "EMAIL", "PLAY_TIME (sec)", "PLAY_TIME (mins)", "PLAY_TIME (hrs)");
		printf("----------------------------------------------------------------------------\n");

		for (auto account : account_list)
		{
			printf("%-30s  %15lld %15.2f %15.2f\n", account.acc_info.email, account.total_play_time, (float)account.total_play_time / (float)MINUTE_, (float)account.total_play_time / (float)HOUR_);
		}



		printf("------------------------ [ LOGS ] ------------------------\n");

	}

}


void clean_up_crashed_processes()
{
	auto rs_launchers = GetPIDs(L"RuneScape.exe");

	for (auto launcher : rs_launchers)
	{
		HANDLE handle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_TERMINATE, false, launcher);

		if (handle)
		{
			DWORD exit_code = 0;
			if (GetExitCodeProcess(handle, &exit_code))
			{
				if (exit_code != STILL_ACTIVE)
				{
					log("[ High ] Process %d exited with code %d. Terminating...", launcher, exit_code);
					TerminateProcess(handle, 0);
				}
			}

		}
	}


}

void ipc_loop()
{
	while (max_clients <= 0 || max_clients > 16)
	{
		std::cout << "How many clients to run? = ";
		std::cin >> max_clients;
	}

	if (!manager_shared_mem && !initialize_ipc())
	{
		log("[ Critical ] Failed to initialize IPC.");
		return;
	}


	if (GetAsyncKeyState(VK_END))
	{
		kill_all = !kill_all;
		log("Status of kill all toggle: %d", kill_all);
	}

	if (!manager_mutex)
	{
		log("[ Critical ] Failed to get mutex handle.");
		return;
	}

	DWORD  dwWaitResult = WaitForSingleObject(manager_mutex, INFINITE);

	switch (dwWaitResult)
	{
	case WAIT_OBJECT_0:
		try{
			print_list();

			cleanup_client_list();

			if(!kill_all)
			{
				// Requires hardcoded path :(
				if (!start_clients())
				{
					log("Failed to start RS clients, what is going on?");
					return;
				}

				// TODO: Remove dependency of shared_memory if possible.
				inject_to_rs2clients();
			}

			command_clients();
		}
		catch (...)
		{
			log("Failed into try/catch.");
		}

		if (!ReleaseMutex(manager_mutex))
		{
			log("Could not release mutex.");
		}
		break;
	case WAIT_ABANDONED:
		log("Wait abandoned.");
		break;
	case WAIT_TIMEOUT:
		log("Wait timeout.");
		break;
	default:
		log("Wait result: %d", dwWaitResult);
		break;
	}

	switch (last_cmd)
	{
	case _server_cmd::SUICIDE:
		Sleep(3000);
		break;
	case _server_cmd::AUTO_START:
		Sleep(8000);
		break;
	case _server_cmd::HEARTBEAT:
		Sleep(3000);
		break;
	default:
		if (kill_all)
			Sleep(100);
		else
			Sleep(1000);
		break;
	}
}

