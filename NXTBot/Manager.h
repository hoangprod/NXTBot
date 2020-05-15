#pragma once



class Manager{
public:
	static void Manage();

	static void Auto_Start();

	static _current_bot get_current_bot();

	static void Keystates(WPARAM wParam);

	const static uint64_t tick = 200;
};