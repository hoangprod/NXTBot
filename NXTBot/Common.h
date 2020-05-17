#pragma once




class Common
{
public:
	static int GetCurrentWorld();
	static const char* GetCurrentWorldString();


	static void Login(const char * username, const char* password);
	static int GetRandomWorld();
	static int HopRandomWorld();
	static int HopWorldGUI();
	static int SwitchWorld(int World);
	static int LoginFromLobby();

	static int ConfirmGUI(int GUI_Id);
	static int ConfirmSwitchWorld();

	static DialogInfoEx* IsWorldWidgetUp();
	static int GetCurrentWildernessLevel();

	static bool SelectFavoriteWorld();

	static bool DepositActionNPC(uint32_t Entity);
	static bool BankUsingNPC(uint32_t targetEntity);
	static bool BankInteractItem(int slot, int option);
	static bool BankLoadPreset(int preset);
	static bool TeleportToAbyssThroughMage();
	static bool StaticInteract2(StaticObjEX obj);
	static bool StaticInteract3(StaticObjEX obj);
	static bool DepositAllThroughBank();
	static bool StaticInteract(StaticObjEX obj);
	static bool DepositAll();
	static bool InteractWithEquipment(int slot, int option, int64_t param3 = 0x5B8000F);
};