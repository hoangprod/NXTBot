#pragma once

class Divination
{
public:
	void FSM();
	bool HaveMemoryInInventory();

	std::string TargetName;


	bool runOnce = false;
};


