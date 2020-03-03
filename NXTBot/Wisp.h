#pragma once




class Wisp
{
public:
	// Prioritize Current Combat -> Banking -> Looting -> New Combat
	void FSM();
	void Combat(EntityObj* Enemies);
	void Banking();
	void Looting(std::vector<FakeItemEX> loots);


	Player * player;
	uint32_t MyrtleId;
	Tile2D origin;
};


class Rabbit
{
public:
	// Prioritize Current Combat -> Banking -> Looting -> New Combat
	void FSM();
	void Combat(EntityObj* Enemies);
	void Banking();
	void Looting(std::vector<FakeItemEX> loots);


	Player* player;
	Tile2D origin;
};

class GeneralCombat
{
public:
	void FSM();
	void Combat(EntityObj* Enemy);

	Player* player;
	std::string monsterTargetName;
	Tile2D origin;
};