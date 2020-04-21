#pragma once




class Wisp
{
public:
	// Prioritize Current Combat -> Banking -> Looting -> New Combat
	void FSM();
	void Combat(EntityObj* Enemies);
	void Banking();
	void Looting(FakeItemEX loots);


	Player * player;
	uint32_t MyrtleId;
	Tile2D origin;
};


class Rabbit
{
public:
	// Prioritize Current Combat -> Banking -> Looting -> New Combat
	void FSM();
	void Banking();
	void Looting(FakeItemEX loots);

	Tile2D origin;
	bool runOnce = false;
};


class GenMining
{
public:
	void FSM();

	Player* player;
	int MiningExp;
	StaticObjEX Node;
	
	std::string TargetNode;
	std::string TargetOre;
};


class Penguins
{
public:
	void FSM();

	Player* player;

};


class BikeAgi
{
public:
	void FSM();

	Player* player;
};


class Woodcutting
{
public:
	void FSM();

	Tile2D origin;
	Player* player;
	int WoodchoppingExp;
	std::string TargetTree;
	std::string TargetLog;
};

