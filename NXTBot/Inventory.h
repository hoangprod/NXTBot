#pragma once


struct Inventory {
	static uint32_t GetFreeSlot();
	static std::vector<FakeItem> GetInventoryItems();
	
	static bool isBankOpened();
	static bool isInventoryFull();
	static bool HaveItemName(std::string name);

	static ContainerObj* GetContainerObj(uint32_t containerId);
	static bool InteractItemOption(uint32_t slot, uint32_t Type);
};