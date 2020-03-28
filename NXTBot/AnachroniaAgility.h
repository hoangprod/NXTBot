#pragma once


class MoneyAgi
{
public:
	void FSM();
	AgilityCourse GetNextCourse();

	Player* player;
	uint32_t currentObstacle;
};

