#pragma once



class Manager{
public:
	static void Manage();
	static bool is_botting();
	static void Keystates(WPARAM wParam);

	const static uint64_t tick = 200;
};