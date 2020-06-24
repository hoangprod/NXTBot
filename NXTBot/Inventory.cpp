#include "pch.h"
#include "GameClasses.h"
#include "Patterns.h"
#include "Game.h"
#include "Inventory.h"

extern UINT_PTR* g_GameContext;
extern Addr Patterns;

uint32_t Inventory::GetFreeSlot()
{
	return 28 - GetInventoryItems().size();
}

std::vector<FakeItem> Inventory::GetInventoryItems()
{
	std::vector<FakeItem> Result;

	auto inventory = GetContainerObj(static_cast<uint32_t>(ContainerType::Backpack));

	if (!inventory || inventory->ContainerContent == 0)
	{
		return Result;
	}

	for (int i = 0; i < 28; i++)
	{
		FakeItem item = inventory->ContainerContent[i];

		if (item.ItemId != -1)
			Result.push_back(item);
	}

	return Result;
}

std::vector<FakeItem> Inventory::GetEquipmentItems()
{
	std::vector<FakeItem> Result;

	auto inventory = GetContainerObj(static_cast<uint32_t>(ContainerType::Equipment));

	if (!inventory || inventory->ContainerContent == 0)
	{
		return Result;
	}

	for (int i = 0; i < 13; i++)
	{
		FakeItem item = inventory->ContainerContent[i];

		if (item.ItemId != -1)
			Result.push_back(item);
	}

	return Result;
}

std::vector<FakeItem> Inventory::LootWindowItems()
{
	std::vector<FakeItem> Result;

	auto lootBag = GetContainerObj(static_cast<uint32_t>(ContainerType::AreaLoot));

	if (!lootBag || lootBag->ContainerContent == 0)
	{
		return Result;
	}

	for (int i = 0; i < 28; i++)
	{
		FakeItem item = lootBag->ContainerContent[i];

		if (item.ItemId != -1)
			Result.push_back(item);
		else
			Result.push_back(FakeItem());
	}

	return Result;
}

std::vector<FakeItem> Inventory::GetInventoryItemsEx()
{
	std::vector<FakeItem> Result;

	auto inventory = GetContainerObj(static_cast<uint32_t>(ContainerType::Backpack));

	if (!inventory || inventory->ContainerContent == 0)
	{
		return Result;
	}

	for (int i = 0; i < 28; i++)
	{
		FakeItem item = inventory->ContainerContent[i];

		if (item.ItemId != -1)
			Result.push_back(item);
		else
			Result.push_back(FakeItem());
	}

	return Result;
}

bool Inventory::isBankOpened()
{
	return GetContainerObj(static_cast<uint32_t>(ContainerType::Bank));
}

bool Inventory::isInventoryFull()
{
	return GetFreeSlot() == 0;
}

bool Inventory::HaveItemName(std::string name)
{
	std::vector<FakeItem> Result;

	auto inventory = GetContainerObj(static_cast<uint32_t>(ContainerType::Backpack));

	if (!inventory || inventory->ContainerContent == 0)
	{
		return false;
	}

	for (int i = 0; i < 28; i++)
	{
		FakeItem item = inventory->ContainerContent[i];

		if (item.ItemId != -1)
		{
			std::string ItemName = RS::ItemIdToString(item.ItemId);

			if(ItemName == name)
				return true;
		}
	}

	return false;
}

// Get item's slot by its itemId, return -1 if item is not found
int Inventory::GetItemById(uint32_t Id)
{
	std::vector<FakeItem> Result;

	auto inventory = GetContainerObj(static_cast<uint32_t>(ContainerType::Backpack));

	if (!inventory || inventory->ContainerContent == 0)
	{
		return -1;
	}

	for (int i = 0; i < 28; i++)
	{
		FakeItem item = inventory->ContainerContent[i];

		if (item.ItemId == Id)
			return i;
	}

	return -1;
}

FakeItem Inventory::GetItemByIdEx(uint32_t Id)
{
	std::vector<FakeItem> Result;

	auto inventory = GetContainerObj(static_cast<uint32_t>(ContainerType::Backpack));

	if (!inventory || inventory->ContainerContent == 0)
	{
		return FakeItem();
	}

	for (int i = 0; i < 28; i++)
	{
		FakeItem item = inventory->ContainerContent[i];

		if (item.ItemId == Id)
			return item;
	}

	return FakeItem();
}

int Inventory::GetItemNameSlot(std::string name)
{
	std::vector<FakeItem> Result;

	auto inventory = GetContainerObj(static_cast<uint32_t>(ContainerType::Backpack));

	if (!inventory || inventory->ContainerContent == 0)
	{
		return false;
	}

	for (int i = 0; i < 28; i++)
	{
		FakeItem item = inventory->ContainerContent[i];

		if (item.ItemId != -1)
		{
			std::string ItemName = RS::ItemIdToString(item.ItemId);

			if (ItemName == name)
				return i;
		}
	}

	return -1;
}

int Inventory::GetItemNameCount(std::string name)
{
	std::vector<FakeItem> Result;
	int count = 0;
	auto inventory = GetContainerObj(static_cast<uint32_t>(ContainerType::Backpack));

	if (!inventory || inventory->ContainerContent == 0)
	{
		return false;
	}

	for (int i = 0; i < 28; i++)
	{
		FakeItem item = inventory->ContainerContent[i];

		if (item.ItemId != -1)
		{
			std::string ItemName = RS::ItemIdToString(item.ItemId);

			if (ItemName == name)
				count++;
		}
	}

	return count;
}

ContainerObj* Inventory::GetContainerObj(uint32_t containerId)
{
	auto gContext = g_GameContext[1];

	auto ContainerManager = ReadPtrOffset(gContext, Patterns.Offset_Container);

	if (!ContainerManager)
		return 0;

	UINT_PTR* container = reinterpret_cast<fn_GetContainerPtr>(Patterns.Func_GetContainer)((UINT_PTR*)ContainerManager, containerId, 0);

	return (ContainerObj*)container;
}

bool Inventory::InteractItemOption(uint32_t slot, uint32_t Type)
{
	uint8_t data[100] = { 0 };

	*reinterpret_cast<int*>(&data[0x58]) = Type;
	*reinterpret_cast<int*>(&data[0x5c]) = slot; 
	*reinterpret_cast<int*>(&data[0x60]) = 0x5C10007;

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

	// changed cdecl to fastcall which should be correct
	typedef void(__cdecl* __ItemInteract)(uint64_t* _this, void* dataPtr);
	reinterpret_cast<__ItemInteract>(func_ptr)(g_GameContext, &dt);

	return true;
}
