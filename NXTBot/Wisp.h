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
	void Combat(EntityObj* Enemies);
	void Banking();
	void Looting(FakeItemEX loots);


	Player* player;
	Tile2D origin;
};

class GeneralCombat
{
public:
	void FSM();
	void Combat(EntityObj* Enemy);
	void Looting(FakeItemEX loot);
	void ConsumeFood();
	bool NeedHeal();
	

	Player* player;
	std::string monsterTargetName;
	Tile2D origin;
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

struct AgilityCourse
{
	AgilityCourse() {};

	AgilityCourse(uint32_t _objId)
	{
		objId = _objId;
	}

	AgilityCourse(uint32_t _objId, Tile2D end) {
		objId = _objId;
		EndPos = end;
	}
	uint32_t objId;
	Tile2D EndPos;
};



class MoneyAgi
{
public:
	void FSM();
	AgilityCourse GetNextCourse();

	Player* player;
	uint32_t currentObstacle;
};




class AbyssCrafting
{
public:
	void FSM();
	void FillPouches();
	void RepairPouches();
	bool FinishedCrafting();
	bool IsInAbyss();
	bool IsInAltar();
	bool isOuterAbyss();
	bool isInnerAbyss();
	bool NeedToRepairPouches();

	Player* player;
	bool justTeleported = false;
	std::string SelectedRune;
	std::string SelectedRift;
};