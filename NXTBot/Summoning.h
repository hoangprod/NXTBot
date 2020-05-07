#pragma once

class TaverlySummoning
{
public:
	void FSM();

	int GetShopTertiary();

	bool IsShopUIOn();
	bool HasSummoningPouches();


	StaticObjEX Obelisk;
	EntityObj* Magestrix = 0;

	int tertiary_product;
	bool runOnce = true;
};