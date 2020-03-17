#include "pch.h"
#include "GameClasses.h"
#include "Patterns.h"
#include "Inventory.h"
#include "Game.h"
#include "Helper.h"
#include "Widgets.h"

extern UINT_PTR* g_GameContext;
extern UINT_PTR g_Module;
extern Addr Patterns;

//0x5C502B2 4a7 = First // Player's turn talking with a NEXT
//0x5C502B2 4a0 = Second
//0x5C502B2 4a2 = NO option, not a conversation. ie: "You do not have any essence to craft" or "Your pouch does not need repair" 
//0x5C502B2 4a4 = Multiple selection chat options.

// 0x5C502A5 Deposit Box
// 0x5C502B2 Conversation Talk



UINT_PTR* Widgets::GetWidgetUI(uint32_t FullIndex)
{
	if (!g_GameContext)
		return 0;

	const uint64_t list = ReadPtrOffset(ReadPtrOffset(g_GameContext, 8), Patterns.Offset_Widget) + 0x128;

	const uint64_t list_start = ReadPtrOffset(list, 8);
	const uint64_t count = ReadPtrOffset(list, 16);
	const uint64_t list_end = list_start + count * 8;

	if (!list_start)
		return 0;

	const uint32_t index = FullIndex % count;

	uint64_t curr = ReadPtrOffset(list_start, 8 * index);

	while (curr != 0 && *reinterpret_cast<uint32_t*>(curr) != FullIndex) {
		curr = ReadPtrOffset(curr, 0x18);
	}
	if (curr == 0) {
		return 0;
	}

	auto WidgetPtr = ReadPtrOffset(curr, 0x10);

	return (UINT_PTR*)WidgetPtr;
}
