#include "pch.h"
#include "ipc.h"

std::vector<DWORD> HandleList;

bool isInList(DWORD pid)
{
	for (auto stored : HandleList)
	{
		if (stored == pid)
			return true;
	}

	return false;
}

bool isReady(HWND hwnd)
{
	RECT rect;
	if (GetWindowRect(hwnd, &rect))
	{
		int width = rect.right - rect.left;
		int height = rect.bottom - rect.top;
		printf("Height: %d  Width: %d\n", height, width);

		if (width && height)
			return true;
	}

	return false;
}

extern HANDLE manager_mutex;

BOOL WINAPI ConsoleHandlerRoutine(DWORD dwCtrlType)
{
	if (dwCtrlType == CTRL_CLOSE_EVENT)
	{
		if (!ReleaseMutex(manager_mutex))
		{
			log("Could not release mutex.");
		}

		return TRUE;
	}

	return FALSE;
}

int WINAPI main()
{

	BOOL ret = SetConsoleCtrlHandler(ConsoleHandlerRoutine, TRUE);

	while (TRUE)
	{
		ipc_loop();
	}
	return TRUE;
}



/*
while (TRUE)
{
	if (GetAsyncKeyState(VK_END) & 1)
		break;

	Sleep(50);
}

auto pids = GetPIDs(L"rs2client.exe");

for (auto pid : pids)
{

	if (isInList(pid))
		continue;

	HandleList.push_back(pid);

	inject(pid);
}

Sleep(1000);
*/