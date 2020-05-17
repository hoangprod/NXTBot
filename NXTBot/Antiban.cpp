#include "pch.h"
#include "GameClasses.h"
#include "Antiban.h"

// 0 = not on break
// 1 = short break
// 2 = long break

bool break_type = false;

extern HWND hWnd;
extern bool bIsFocus;
extern bool to_suicide;

std::vector<mouse_biometric> mouse_data;

void antiban::Keyboard()
{

}

void antiban::mouse_record(UINT uMsg, UINT wParam, UINT lParam)
{
	static int64_t current_tick = GetTickCount64();

	int64_t elapsed_time = GetTickCount64() - current_tick;

	mouse_data.push_back(mouse_biometric(uMsg, wParam, lParam, (UINT)elapsed_time));

	current_tick = GetTickCount64();
}


void antiban::mouse_playback(mouse_replay* mouse_data, int screen_height, int screen_width)
{
	// replay between 10 mins and 60 mins of "replay" data
	float mins_replay = antiban::float_random_range(10, 60);

	int start_index = antiban::int_random_range(0, mouse_data->header.elements - 1);

	float mins_played_already = 0;

	float height_ratio = (float)screen_height / (float)mouse_data->header.height;

	float width_ratio = (float)screen_width / (float)mouse_data->header.width;

	if (start_index >= mouse_data->header.elements)
	{
		printf("[anitban] - should not be here\n");
		return;
	}

	for (int i = start_index; i < mouse_data->header.elements; i++)
	{
		if (to_suicide)
			return;

		Sleep(mouse_data->points[i].delay);

		if (bIsFocus)
			break;

		if (mouse_data->points[i].uMsg == WM_MOUSEMOVE)
		{
			UINT lParam = mouse_data->points[i].lParm;

			int new_x = floor(GET_X_LPARAM(lParam) * width_ratio);
			int new_y = floor(GET_Y_LPARAM(lParam) * height_ratio);

			UINT new_lParam = MAKELPARAM(new_x, new_y);

			//printf("%d %d ->  %d %d\n", GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), GET_X_LPARAM(new_lParam), GET_Y_LPARAM(new_lParam));

			SendMessage(hWnd, mouse_data->points[i].uMsg, mouse_data->points[i].wParm, new_lParam);

		}
		else
		{
			SendMessage(hWnd, mouse_data->points[i].uMsg, mouse_data->points[i].wParm, mouse_data->points[i].lParm);
		}

		// How many second did this delayed
		float delay_seconds = ((float)mouse_data->points[i].delay / 1000.0f);

		// How many minutes did this delayed
		float delay_minutes = delay_seconds / 60.0f;

		// How many minutes is this entire loop delayed already
		mins_played_already += delay_minutes;

		// if we have delayed more than the amount of minutes we were going to delay, break
		if (mins_played_already >= mins_replay)
			break;
	}
}


std::vector<fake_key> fakekeys = { fake_key(VK_UP, 0x1480001, 0xc1480001), fake_key(VK_DOWN, 0x1500001, 0xc1500001), fake_key(VK_RIGHT, 0x14d0001, 0xc14d0001), fake_key(VK_LEFT, 0x14b0001, 0xc14b0001), fake_key(VK_INSERT, 0x1520001, 0xc1520001) };

void antiban::anti_afk()
{
	int randomIndex = rand() % fakekeys.size();

	fake_key selected_key = fakekeys[randomIndex];

	int hold_time = antiban::int_random_range(100, 1500);

	SendMessageW(hWnd, WM_KEYDOWN, selected_key.uMsg, selected_key.up_key);

	// TODO: Make SendMessage not rely on sleep.
	Sleep(hold_time);

	SendMessageW(hWnd, WM_KEYUP, selected_key.uMsg, selected_key.down_key);
}

int antiban::int_random_range(int min, int max)
{
	std::random_device rd;
	std::mt19937 mt(rd());
	std::uniform_int_distribution<int> dist(min, max);

	return dist(mt);
}

float antiban::float_random_range(float min, float max)
{
	std::random_device rd;
	std::mt19937 mt(rd());
	std::uniform_real_distribution<float> dist(min, max);

	return dist(mt);
}

// return true if on break, false if not
bool antiban::break_manager(float min, float max, float min_break, float max_break, time_t *start, float* antiban_rest_timer, float* break_duration)
{
	double minute_since_start = difftime(time(0), *start) / 60.0f;

	// If we are not even at the lowest possible "break time" yet
	if (minute_since_start < 1)
	{
		//printf("[Antiban] minimum time have not elapsed yet. Only %f have elapsed vs %d.\n", minute_since_start, antiban::antiban_rest_timer);
		return false;
	}

	// If it is time to take a break
	if (minute_since_start >= (double)*antiban_rest_timer)
	{
		printf("[Antiban] - Taking a break for %f seconds. %f seconds left\n", *break_duration, (double)*antiban_rest_timer + (double)*break_duration - minute_since_start);

		// if we are done taking a break
		if (minute_since_start >= (double)*antiban_rest_timer + (double)*break_duration)
		{
			// reset "start" time
			*start = time(0);

			if (*break_duration > 4.0f)
				SendMessageW(hWnd, WM_SETFOCUS, NULL, NULL);

			// generate the next time we will be taking a break
			*antiban_rest_timer = float_random_range(min, max);

			// generate the next break timer
			*break_duration = antiban::float_random_range(min_break, max_break);

			printf("[Antiban] Finished taking a break. New start time is %lld and next rest is in %f minutes.\n", *start, *antiban_rest_timer);

			return false;
		}

		if (*break_duration > 4.0f)
			SendMessageW(hWnd, WM_KILLFOCUS, NULL, NULL);

		// otherwise, if we not done taking a break
		return true;
	}

	return false;
}

// return true if on break
bool antiban::long_break_manager(float min, float max, float min_break, float max_break)
{
	static time_t start;
	static float antiban_rest_timer;
	static float break_duration;

	if(!start)
		start = time(0);

	if(!antiban_rest_timer)
		antiban_rest_timer = float_random_range(min, max);

	if(!break_duration)
		break_duration = antiban::float_random_range(min_break, max_break);

	printf("long break - Start: %lld %f\n", start, antiban_rest_timer);

	bool bm = break_manager(min, max, min_break, max_break, &start, &antiban_rest_timer, &break_duration);

	if (bm)
		break_type = 2;

	return bm;
}

// return true if on break
bool antiban::short_break_manager(float min, float max, float min_break, float max_break)
{
	static time_t start;
	static float antiban_rest_timer;
	static float break_duration;

	if (!start)
		start = time(0);

	if (!antiban_rest_timer)
		antiban_rest_timer = float_random_range(min, max);

	if (!break_duration)
		break_duration = antiban::float_random_range(min_break, max_break);

	printf("short break - Start: %lld %f dura %f\n", start, antiban_rest_timer, break_duration);

	bool bm = break_manager(min, max, min_break, max_break, &start, &antiban_rest_timer, &break_duration);

	if (bm)
		break_type = 1;

	return bm;
}
