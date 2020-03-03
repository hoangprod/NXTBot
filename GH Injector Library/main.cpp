#include "pch.h"
#include "Injection.h"
#include <iostream>

std::vector<DWORD> HandleList;

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


DWORD WINAPI inject(LPVOID pidz)
{
	int pid = *static_cast<int*>(pidz);

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
		delete pidz;
		return 0;
	}

	Ret = _ManualMap(szDll, hProc, LAUNCH_METHOD::LM_HijackThread, INJ_ERASE_HEADER, out, Err);


	if (Err || Ret)
		printf("[!] Error %d when injecting.\n", Err);
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

	delete pidz;

	return 1;
}

int WINAPI main()
{


	while (TRUE)
	{

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

			int* id = new int(pid);

			CreateThread(NULL, NULL, inject, id, NULL, NULL);

			Sleep(1000);
		}

		Sleep(1000);
	}
	return TRUE;
}