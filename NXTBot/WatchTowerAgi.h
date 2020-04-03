#pragma once

class WatchTowerAgi
{
public:
	void FSM();

	Player* player;

	StaticObjEX Trellis;
	StaticObjEX Stair;
};



class WildernessAgilityCourse
{
public:
	void FSM();
	AgilityCourse GetNextCourse();
	void ConsumeFood();

	Player* player;
	uint32_t currentObstacle;
};
