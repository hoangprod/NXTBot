#pragma once


class MoneyAgi
{
public:
	void FSM();
	AgilityCourse GetNextCourse();

	static bool is_on_endpos();

	uint32_t currentObstacle;
};

