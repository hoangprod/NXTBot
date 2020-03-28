#pragma once

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

	bool justTeleported = false;
	std::string SelectedRune;
	std::string SelectedRift;
};