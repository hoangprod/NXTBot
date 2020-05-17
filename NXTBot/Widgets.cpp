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



DialogInfoEx* Widgets::GetWidgetUI(uint32_t FullIndex)
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

	return (DialogInfoEx*)WidgetPtr;
}

DialogInfoEx* Widgets::IsInDialogWidget()
{
	return Widgets::GetWidgetUI(CONVERSATION_WIDGET);
}

RSDialog Widgets::GetDialogType()
{
	auto Dialog = IsInDialogWidget();

	if (!Dialog)
		return RSDialog::Unknown;

	switch (Dialog->SecondaryId)
	{
	case 0x4a0:
		return RSDialog::NPCDialog;
		break;
	case 0x4a2:
		return RSDialog::GameDialog;
		break;
	case 0x4a4:
		return RSDialog::MultiSelectionDialog;
		break;
	case 0x4a7:
		return RSDialog::PlayerDialog;
		break;
	case 0x49f:
		return RSDialog::ModernDestroy;
		break;
	default:
		return RSDialog::Unknown;
		break;
	}
}

UIType Widgets::GetUIType()
{
	// Just using summoning for now
	auto UI = Widgets::GetWidgetUI(SUMMONING_WIDGET);

	if (!UI)
		return UIType::UNKNOWN;

	switch (UI->SecondaryId)
	{
	case 0x4F1:
		return UIType::SHOP_STORE;
		break;
	case 0x55a:
		return UIType::OBELISK_WIDGET;
		break;
	case 0x572:
		return UIType::SLAYER_CONTRACT;
		break;
	default:
		return UIType::UNKNOWN;
		break;
	}

	return UIType();
}
