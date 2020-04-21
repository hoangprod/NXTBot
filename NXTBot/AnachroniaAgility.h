#pragma once


class MoneyAgi
{
public:
	void FSM();
	AgilityCourse GetNextCourse();

	uint32_t currentObstacle;
};

