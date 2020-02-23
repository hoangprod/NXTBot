#include "pch.h"
#include "GameClasses.h"
#include "Patterns.h"
#include "Inventory.h"

extern UINT_PTR* g_GameContext;
extern Addr Patterns;

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

	*reinterpret_cast<int*>(&data[0x50]) = Type;
	*reinterpret_cast<int*>(&data[0x54]) = slot; 
	*reinterpret_cast<int*>(&data[0x58]) = 0x5C10007;
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
	typedef void(__cdecl* __ItemInteract)(uint64_t** _this, void* dataPtr);
	reinterpret_cast<__ItemInteract>(func_ptr)(handler, &dt);

	return true;
}
