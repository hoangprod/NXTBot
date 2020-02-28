#pragma once


struct Inventory {
	static ContainerObj* GetContainerObj(uint32_t containerId);
	static bool InteractItemOption(uint32_t slot, uint32_t Type);
};