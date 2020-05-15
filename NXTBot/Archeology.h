#pragma once

class Archeology
{
public:
	// Prioritize Current Combat -> Banking -> Looting -> New Combat
	void FSM();
	void Initializer();
	void Banking();
	void Banking20();
	void CacheHandler();

	bool isSoilBoxFull();
	int getSoilBoxAmount();

	static bool is_next_to_archeology_node();

	bool isCache;
	int ArcheLevel;
	int ArcheologyExp;
	StaticObjEX Node;
	std::string NodeName;

	bool runOnce = true;

	StaticObjEX DepositBox;
	StaticObjEX MaterialCart;
};
