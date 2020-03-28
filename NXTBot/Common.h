#pragma once




class Common
{
public:
	static int GetCurrentWorld();
	static const char* GetCurrentWorldString();

	static int GetRandomWorld();
	static int HopRandomWorld();
	static int HopWorldGUI();
	static int SwitchWorld(int World);

	static int ConfirmGUI(int GUI_Id);
	static int ConfirmSwitchWorld();

	static DialogInfoEx* IsWorldWidgetUp();
	static int GetCurrentWildernessLevel();

	static bool DepositActionNPC(uint32_t Entity);
	static bool BankUsingNPC(uint32_t targetEntity);
	static bool BankInteractItem(int slot, int option);
	static bool BankLoadPreset(int preset);
	static bool TeleportToAbyssThroughMage();
};