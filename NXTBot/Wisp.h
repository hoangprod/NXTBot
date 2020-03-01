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
	Tile2D origin;
};