#include "pch.h"
#include "GameClasses.h"
#include "Patterns.h"
#include "Inventory.h"
#include "Game.h"
#include "PlayableEntity.h"
#include "Tile.h"
#include "Widgets.h"
#include "Experience.h"
#include "Common.h"

extern UINT_PTR* g_GameContext;
extern UINT_PTR g_Module;
extern Addr Patterns;

std::vector<int> MemberWorlds = { 1, 2, 4, 5, 6, 9, 10, 12, 14, 15, 16, 21, 22, 23, 24, 25, 26, 27, 28, 31, 32, 35, 36, 37, 39, 40, 44, 45, 46, 49, 50, 51, 53, 54, 56, 58, 59, 60, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 76,
	77, 78, 79, 82, 83, 84, 85, 87, 88, 89, 91, 92, 97, 98, 99, 100, 103, 104, 105, 106, 115, 116, 117, 119, 123, 124, 134, 138, 139, 140 };

int WildernessY[] = { 3525, 3528, 3536, 3544, 3552, 3560 };

int Common::GetCurrentWorld()
{
	auto g_Context = RS::GetGameContext();

	if (!g_Context)
		return 0;

	return g_Context->WorldClass->CurrentWorld->CurrentWorld;
}

const char* Common::GetCurrentWorldString()
{
	auto g_Context = RS::GetGameContext();

	if (!g_Context)
		return 0;

	return g_Context->WorldClass->CurrentWorld->WorldUrl;
}

int Common::GetRandomWorld()
{
	int currentWorld = Common::GetCurrentWorld();

	int randomIndex = rand() % MemberWorlds.size();

	auto NewWorld = MemberWorlds[randomIndex];

	if (NewWorld == currentWorld)
		return Common::GetRandomWorld();
	else
		return NewWorld;
}

int Common::HopRandomWorld()
{
	int NewWorld = Common::GetRandomWorld();

	printf("Switching world to %d\n", NewWorld);

	return Common::SwitchWorld(NewWorld);
}

int Common::HopWorldGUI()
{
	uint8_t data[100] = { 0 };

	*reinterpret_cast<int*>(&data[0x58]) = 3;
	*reinterpret_cast<int*>(&data[0x5c]) = -1;
	*reinterpret_cast<int*>(&data[0x60]) = 0x5B90009;

	uint64_t** handler = (uint64_t**)Patterns.Addr_InventoryActionHandler;
	if (!handler)
		return false;

	uint64_t* handler_vtable = *handler;

	if (!handler_vtable)
		return false;

	uint64_t func_ptr = handler_vtable[2];

	if (!func_ptr)
		return false;


	dataStruct dt;
	dt.dataPtr = data;

	typedef void(__cdecl* _WidgetLootAll)(uint64_t* _this, void* dataPtr);
	reinterpret_cast<_WidgetLootAll>(func_ptr)(g_GameContext, &dt);

	return 1;
}

int Common::SwitchWorld(int World)
{
	uint8_t data[100] = { 0 };

	*reinterpret_cast<int*>(&data[0x58]) = 1;
	*reinterpret_cast<int*>(&data[0x5c]) = World;
	*reinterpret_cast<int*>(&data[0x60]) = 0x6330008;

	uint64_t** handler = (uint64_t**)Patterns.Addr_InventoryActionHandler;
	if (!handler)
		return false;

	uint64_t* handler_vtable = *handler;

	if (!handler_vtable)
		return false;

	uint64_t func_ptr = handler_vtable[2];

	if (!func_ptr)
		return false;


	dataStruct dt;
	dt.dataPtr = data;

	typedef void(__cdecl* _WidgetLootAll)(uint64_t* _this, void* dataPtr);
	reinterpret_cast<_WidgetLootAll>(func_ptr)(g_GameContext, &dt);

	return 1;
}

int Common::ConfirmGUI(int GUI_Id)
{
	uint8_t data[100] = { 0 };

	*reinterpret_cast<int*>(&data[0x58]) = 0;
	*reinterpret_cast<int*>(&data[0x5c]) = -1;
	*reinterpret_cast<int*>(&data[0x60]) = GUI_Id;

	uint64_t** handler = (uint64_t**)Patterns.Addr_InventoryActionHandler;
	if (!handler)
		return false;

	uint64_t* handler_vtable = *handler;

	if (!handler_vtable)
		return false;

	uint64_t func_ptr = handler_vtable[2] - 0x10;

	if (!func_ptr)
		return false;


	dataStruct dt;
	dt.dataPtr = data;

	typedef uintptr_t(__cdecl* _InteractionDepositNPC)(uint64_t* _this, void* dataPtr);
	reinterpret_cast<_InteractionDepositNPC>(func_ptr)(g_GameContext, &dt);
}

int Common::ConfirmSwitchWorld()
{
	return Common::ConfirmGUI(0x633005F);
}

DialogInfoEx* Common::IsWorldWidgetUp()
{
	return Widgets::GetWidgetUI(CHANGE_WORLD_WIDGET);
}

int Common::GetCurrentWildernessLevel()
{
	Player localPlayer = RS::GetLocalPlayer();

	int TileY = localPlayer.GetTilePosition().y;

	for (int i = 6; i > 0; i--)
	{
		if (TileY >= WildernessY[i - 1])
		{
			return i;
		}
	}
	return 0;
}

bool Common::DepositActionNPC(uint32_t Entity)
{
	uint8_t data[100] = { 0 };

	*reinterpret_cast<int*>(&data[0x58]) = Entity;

	uint64_t func_ptr = g_Module + 0x9c1a0;

	if (!func_ptr)
		return false;


	dataStruct dt;
	dt.dataPtr = data;

	typedef uintptr_t(__cdecl* _InteractionDepositNPC)(uint64_t* _this, void* dataPtr);
	auto test = reinterpret_cast<_InteractionDepositNPC>(func_ptr)(g_GameContext, &dt);

	return true;
}


bool Common::BankUsingNPC(uint32_t targetEntity)
{
	auto ent = RS::GetEntityObjectByEntityId(targetEntity);

	if (!ent)
		return false;

	uint8_t data[100] = { 0 };

	*reinterpret_cast<int*>(&data[0x58]) = ent->EntityId;
	*reinterpret_cast<int*>(&data[0x5c]) = 0;
	*reinterpret_cast<int*>(&data[0x60]) = 0;

	uint64_t func_ptr = g_Module + 0x9c160;

	if (!func_ptr)
		return false;


	dataStruct dt;
	dt.dataPtr = data;

	typedef void(__cdecl* _Loot)(uint64_t* _this, void* dataPtr);
	reinterpret_cast<_Loot>(func_ptr)(g_GameContext, &dt);

	return true;
}

bool Common::BankInteractItem(int slot, int option)
{
	uint8_t data[100] = { 0 };

	*reinterpret_cast<int*>(&data[0x58]) = option;
	*reinterpret_cast<int*>(&data[0x5c]) = slot;
	*reinterpret_cast<int*>(&data[0x60]) = 0x205000E; // Hardcoded

	uint64_t** handler = (uint64_t**)Patterns.Addr_InventoryActionHandler;
	if (!handler)
		return false;

	uint64_t* handler_vtable = *handler;

	if (!handler_vtable)
		return false;

	uint64_t func_ptr = handler_vtable[2];

	if (!func_ptr)
		return false;


	dataStruct dt;
	dt.dataPtr = data;

	typedef uintptr_t(__cdecl* _BankDepositAll)(uint64_t* _this, void* dataPtr);
	reinterpret_cast<_BankDepositAll>(func_ptr)(g_GameContext, &dt);
	return true;
}

bool Common::BankLoadPreset(int preset)
{
	uint8_t data[100] = { 0 };

	*reinterpret_cast<int*>(&data[0x58]) = 1;
	*reinterpret_cast<int*>(&data[0x5c]) = preset;
	*reinterpret_cast<int*>(&data[0x60]) = 0x2050070;// Hardcoded

	uint64_t** handler = (uint64_t**)Patterns.Addr_InventoryActionHandler;
	if (!handler)
		return false;

	uint64_t* handler_vtable = *handler;

	if (!handler_vtable)
		return false;

	uint64_t func_ptr = handler_vtable[2];

	if (!func_ptr)
		return false;


	dataStruct dt;
	dt.dataPtr = data;

	typedef uintptr_t(__cdecl* _BankDepositAll)(uint64_t* _this, void* dataPtr);
	reinterpret_cast<_BankDepositAll>(func_ptr)(g_GameContext, &dt);
	return true;
}

bool Common::TeleportToAbyssThroughMage()
{
	uint8_t data[100] = { 0 };

	*reinterpret_cast<int*>(&data[0x58]) = 18639; // UPDATE PLEASE MageOfZamorak
	*reinterpret_cast<int*>(&data[0x5c]) = 0;
	*reinterpret_cast<int*>(&data[0x60]) = 0;// Hardcoded

	uint64_t func_ptr = g_Module + 0x9c1c0;

	if (!func_ptr)
		return false;


	dataStruct dt;
	dt.dataPtr = data;

	typedef uintptr_t(__cdecl* _BankDepositAll)(uint64_t* _this, void* dataPtr);
	reinterpret_cast<_BankDepositAll>(func_ptr)(g_GameContext, &dt);
	return true;
}

bool Common::InteractWithEquipment(int slot, int option, int64_t param3)
{
	uint8_t data[100] = { 0 };

	*reinterpret_cast<int*>(&data[0x58]) = slot;
	*reinterpret_cast<int*>(&data[0x5c]) = option;
	*reinterpret_cast<int*>(&data[0x60]) = param3;

	uint64_t** handler = (uint64_t**)Patterns.Addr_InventoryActionHandler;
	if (!handler)
		return false;

	uint64_t* handler_vtable = *handler;

	if (!handler_vtable)
		return false;

	uint64_t func_ptr = handler_vtable[2];

	if (!func_ptr)
		return false;


	dataStruct dt;
	dt.dataPtr = data;

	typedef void(__cdecl* _WidgetLootAll)(uint64_t* _this, void* dataPtr);
	reinterpret_cast<_WidgetLootAll>(func_ptr)(g_GameContext, &dt);

	return true;
}

bool Common::StaticInteract2(StaticObjEX obj)
{
	uint8_t data[100] = { 0 };

	if (obj.SecondId)
		*reinterpret_cast<int*>(&data[0x58]) = obj.SecondId;
	else
		*reinterpret_cast<int*>(&data[0x58]) = obj.Definition->Id;

	*reinterpret_cast<int*>(&data[0x5c]) = obj.TileX;
	*reinterpret_cast<int*>(&data[0x60]) = obj.TileY;

	uint64_t func_ptr = g_Module + 0x9c0a0;

	if (!func_ptr)
		return false;


	dataStruct dt;
	dt.dataPtr = data;

	typedef void(__cdecl* _Loot)(uint64_t* _this, void* dataPtr);
	reinterpret_cast<_Loot>(func_ptr)(g_GameContext, &dt);

	return true;
}

bool Common::DepositAllThroughBank()
{
	uint8_t data[100] = { 0 };

	*reinterpret_cast<int*>(&data[0x58]) = 1;
	*reinterpret_cast<int*>(&data[0x5c]) = -1;
	*reinterpret_cast<int*>(&data[0x60]) = 0x2050025;

	uint64_t** handler = (uint64_t**)Patterns.Addr_InventoryActionHandler;
	if (!handler)
		return false;

	uint64_t* handler_vtable = *handler;

	if (!handler_vtable)
		return false;

	uint64_t func_ptr = handler_vtable[2];

	if (!func_ptr)
		return false;


	dataStruct dt;
	dt.dataPtr = data;

	typedef void(__cdecl* _WidgetLootAll)(uint64_t* _this, void* dataPtr);
	reinterpret_cast<_WidgetLootAll>(func_ptr)(g_GameContext, &dt);

	return true;
}

bool Common::StaticInteract(StaticObjEX obj)
{
	uint8_t data[100] = { 0 };

	if (obj.SecondId)
		*reinterpret_cast<int*>(&data[0x58]) = obj.SecondId;
	else
		*reinterpret_cast<int*>(&data[0x58]) = obj.Definition->Id;

	*reinterpret_cast<int*>(&data[0x5c]) = obj.TileX;
	*reinterpret_cast<int*>(&data[0x60]) = obj.TileY;

	uint64_t func_ptr = g_Module + 0x9c080;

	if (!func_ptr)
		return false;


	dataStruct dt;
	dt.dataPtr = data;

	typedef void(__cdecl* _Loot)(uint64_t* _this, void* dataPtr);
	reinterpret_cast<_Loot>(func_ptr)(g_GameContext, &dt);

	return true;
}


bool Common::DepositAll()
{
	uint8_t data[100] = { 0 };

	*reinterpret_cast<int*>(&data[0x58]) = 1;
	*reinterpret_cast<int*>(&data[0x5c]) = -1;
	*reinterpret_cast<int*>(&data[0x60]) = 0xb0005; // Hardcoded

	uint64_t** handler = (uint64_t**)Patterns.Addr_InventoryActionHandler;
	if (!handler)
		return false;

	uint64_t* handler_vtable = *handler;

	if (!handler_vtable)
		return false;

	uint64_t func_ptr = handler_vtable[2];

	if (!func_ptr)
		return false;


	dataStruct dt;
	dt.dataPtr = data;

	typedef uintptr_t(__cdecl* _BankDepositAll)(uint64_t* _this, void* dataPtr);
	reinterpret_cast<_BankDepositAll>(func_ptr)(g_GameContext, &dt);
	return true;
}
