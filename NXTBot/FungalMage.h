#pragma once

class FungalMage
{
public:
	void FSM();
	bool ConsumeFood();

	bool NeedHeal();
	bool HaveFoodInInvent();
	bool IsInCave();
	bool IsFarFromOrigin(Tile2D currentPos);
	bool runOnce = false;

	Tile2D origin;
};


