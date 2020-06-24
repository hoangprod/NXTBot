#pragma once


struct antiban
{
	static void Keyboard();

	static void mouse_record(UINT uMsg, UINT wParam, UINT lParam);
	static void mouse_playback(mouse_replay* mouse_data, int screen_height, int screen_width);

	static void anti_afk();

	static int int_random_range(int min, int max);
	static float float_random_range(float min, float max);

	static bool break_manager(float min, float max, float min_break, float max_break, time_t * start, float* antiban_rest_timer, float* break_duration);
	static bool long_break_manager(float min = 60.0f, float max = 400.0f, float min_break = 2.0f, float max_break = 6.0f);
	static bool short_break_manager(float min = 15.0f, float max = 60.0f, float min_break = 0.08f, float max_break = 1.5f);
};

struct fake_key
{
	fake_key(UINT msg, UINT param1, UINT param2)
	{
		uMsg = msg;
		down_key = param1;
		up_key = param2;
	};
	UINT uMsg;
	UINT down_key;
	UINT up_key;
};