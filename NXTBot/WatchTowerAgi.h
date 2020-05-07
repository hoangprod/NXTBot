#pragma once

class WatchTowerAgi
{
public:
	void FSM();

	StaticObjEX Trellis;
	StaticObjEX Stair;
};



class WildernessAgilityCourse
{
public:
	void FSM();
	AgilityCourse GetNextCourse();
	bool isUnderWorld();
	void ConsumeFood();
	EntityObj* enemy;
	uint32_t currentObstacle;
};
