#include "pch.h"
#include "GameClasses.h"
#include "Patterns.h"
#include "Action.h"

extern UINT_PTR* g_GameContext;
extern Addr Patterns;

bool GAction::Walk(Tile2D tile)
{
	uint8_t data[100] = { 0 };

	*reinterpret_cast<int*>(&data[0x54]) = tile.x;
	*reinterpret_cast<int*>(&data[0x58]) = tile.y;
	
	uint64_t** handler = (uint64_t**)Patterns.Addr_MoveActionHandler;

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

	typedef void(__cdecl* _MoveTo)(uint64_t **_this, void* dataPtr);
	reinterpret_cast<_MoveTo>(func_ptr)(handler, &dt);

	return true;
}
