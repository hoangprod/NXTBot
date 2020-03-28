#pragma once

void print_bytes(const char* title, const unsigned char* data, size_t dataLen, bool format);
int AIOAuth(std::string Username, std::string Password, std::string HWID);
char* DownloadCore(DWORD& Size);

struct Authentication {
	Authentication(DWORD instance, const char* User, const char* Pass, const char* Hardware)
	{
		Instance = instance;
		strcpy_s(Username, User);
		strcpy_s(Password, Pass);
		strcpy_s(HWID, Hardware);
	}

	DWORD Instance;
	char Username[100];
	char Password[100];
	char HWID[100];
};