#pragma once

class Archeology
{
public:
	// Prioritize Current Combat -> Banking -> Looting -> New Combat
	void FSM();
	void Banking();
	void Banking20();

	bool isSoilBoxFull();
	int getSoilBoxAmount();

	int ArcheologyExp;
	Player* player;
	StaticObjEX Node;
	std::string NodeName;


	StaticObjEX DepositBox;
	StaticObjEX MaterialCart;
};
