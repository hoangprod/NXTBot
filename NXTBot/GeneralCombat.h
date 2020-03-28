#pragma once

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
