#include "pch.h"
#include "Helper.h"
#include "Scan.h"
#include "Patterns.h"

Addr Patterns;

bool __stdcall findPatterns()
{
	char* Rs2Client = (char*)HdnGetModuleBase("rs2client.exe");

	printf("------------------ [ START PATTERN SCAN ] ------------------\n\n");





	printf("------------------ [ STATIC ADDRESSES ] ------------------\n\n");

	Patterns.Addr_MouseIntersectWorld = (UINT_PTR)ptr_offset_Scanner(Rs2Client, 0x300000, "\x44\x89\x35\xCC\xCC\xCC\x00\x89\x35\xCC\xCC\xCC\x00", 0, 7, 3, "xxx???xxx???x");

	if (!Patterns.Addr_MouseIntersectWorld)
	{
		printf("[Error] Patterns.Addr_MouseIntersectWorld failed to pattern scan.\n");
		return FALSE;
	}
	else { printf("[Pattern Scan]  Patterns.Addr_MouseIntersectWorld is at %llx\n", Patterns.Addr_MouseIntersectWorld); };

	Patterns.Addr_InventoryActionHandler = (UINT_PTR)ptr_offset_Scanner2(Rs2Client, 0x300000, "\x48\x8d\x05\xCC\xCC\xCC\x00\x48\x89\x44\xCC\xCC\x4d\x8b\xc7", 0, 7, 3, "xxx???xxxx??xxx");

	if (!Patterns.Addr_InventoryActionHandler)
	{
		printf("[Error] Patterns.Addr_InventoryActionHandler failed to pattern scan.\n");
		return FALSE;
	}
	else { printf("[Pattern Scan]  Patterns.Addr_InventoryActionHandler is at %llx\n", Patterns.Addr_InventoryActionHandler); };

	Patterns.Addr_MoveActionHandler = (UINT_PTR)ptr_offset_Scanner(Rs2Client, 0x300000, "\x48\x8d\x05\xCC\xCC\xCC\x00\x48\x89\x44\x24\x40\x48\xc7", 0, 7, 3, "xxx???xxxxxxxx");

	if (!Patterns.Addr_MoveActionHandler)
	{
		printf("[Error] Patterns.Addr_MoveActionHandler failed to pattern scan.\n");
		return FALSE;
	}
	else { printf("[Pattern Scan]  Patterns.Addr_MoveActionHandler is at %llx\n", Patterns.Addr_MoveActionHandler); };








	printf("\n------------------ [ OFFSETS ] ------------------\n\n");

	Patterns.Offset_OptionText = (UINT_PTR)Scan_Offsets(Rs2Client, 0x300000, "\x48\x83\xb9\xCC\xCC\x00\x00\x00\x75\xCC\x32\xc0\xc3", "xxx??xxxx?xxx", 3, 4);

	if (!Patterns.Offset_OptionText)
	{
		printf("[Error] Patterns.Offset_OptionText failed to pattern scan.\n");
		return FALSE;
	}
	else { printf("[Pattern Scan]  Patterns.Offset_OptionText is at %llx\n", Patterns.Offset_OptionText); };

	Patterns.Offset_Container = (UINT_PTR)Scan_Offsets(Rs2Client, 0x300000, "\x48\x8b\xb0\xCC\xCC\x00\x00\x44\x0f\xb6\xc7", "xxx??xxxxxx", 3, 4);

	if (!Patterns.Offset_Container)
	{
		printf("[Error] Patterns.Offset_Container failed to pattern scan.\n");
		return FALSE;
	}
	else { printf("[Pattern Scan]  Patterns.Offset_Container is at %llx\n", Patterns.Offset_Container); };

	Patterns.Offset_PlayerContext = (UINT_PTR)Scan_Offsets(Rs2Client, 0x300000, "\x48\x8b\x88\xCC\xCC\x00\x00\xe8\xCC\xCC\xCC\xff\x48\x8b\x48\x08", "xxx??xxx???xxxxx", 3, 4);

	if (!Patterns.Offset_PlayerContext)
	{
		printf("[Error] Patterns.Offset_PlayerContext failed to pattern scan.\n");
		return FALSE;
	}
	else { printf("[Pattern Scan]  Patterns.Offset_PlayerContext is at %llx\n", Patterns.Offset_PlayerContext); };

	Patterns.Offset_EntityArrayList = (UINT_PTR)Scan_Offsets(Rs2Client, 0x300000, "\x48\x8b\x83\xCC\xCC\x00\x00\x48\x63\x0c\x88", "xxx??xxxxxx", 3, 4);

	if (!Patterns.Offset_EntityArrayList)
	{
		printf("[Error] Patterns.Offset_EntityArrayList failed to pattern scan.\n");
		return FALSE;
	}
	else { printf("[Pattern Scan]  Patterns.Offset_EntityArrayList is at %llx\n", Patterns.Offset_EntityArrayList); };

	Patterns.Offset_EntityOffsetList = (UINT_PTR)Scan_Offsets(Rs2Client, 0x300000, "\x4c\x63\x84\xbb\xCC\xCC\x00\x00\x4c\x8b\x4b", "xxxx??xxxxx", 4, 4);

	if (!Patterns.Offset_EntityOffsetList)
	{
		printf("[Error] Patterns.Offset_EntityOffsetList failed to pattern scan.\n");
		return FALSE;
	}
	else { printf("[Pattern Scan]  Patterns.Offset_EntityOffsetList is at %llx\n", Patterns.Offset_EntityOffsetList); };




	printf("\n------------------ [ FUNCTIONS ] ------------------\n\n");

	Patterns.Func_OnCursorDoAction = (UINT_PTR)PatternScan(Rs2Client, 0x300000, "\x48\x89\x5c\x24\x10\x48\x89\x6c\x24\x18\x56\x48\x83\xec\x20\x48\x8b\x41\x08", "xxxxxxxxxxxxxxxxxxx");

	if (!Patterns.Func_OnCursorDoAction)
	{
		printf("[Error] Patterns.Func_OnCursorDoAction failed to pattern scan.\n");
		return FALSE;
	}
	else { printf("[Pattern Scan]  Patterns.Func_OnCursorDoAction is at %llx\n", Patterns.Func_OnCursorDoAction); };

	Patterns.Func_OnCursorWorldContextMenu = (UINT_PTR)PatternScan(Rs2Client, 0x300000, "\x48\x8b\xc4\x44\x89\x40\x18\x89\x50\x10\x55\x56\x57\x41\x54\x41\x55\x41\x56\x41\x57", "xxxxxxxxxxxxxxxxxxxxx");

	if (!Patterns.Func_OnCursorWorldContextMenu)
	{
		printf("[Error] Patterns.Func_OnCursorWorldContextMenu failed to pattern scan.\n");
		return FALSE;
	}
	else { printf("[Pattern Scan]  Patterns.Func_OnCursorWorldContextMenu is at %llx\n", Patterns.Func_OnCursorWorldContextMenu); };

	Patterns.Func_GetLocalPlayer = (UINT_PTR)PatternScan(Rs2Client, 0x300000, "\x48\x83\x79\xCC\x00\x74\x05\x48\x8d\x41\xCC\xc3", "xxx?xxxxxx?x");

	if (!Patterns.Func_GetLocalPlayer)
	{
		printf("[Error] Patterns.Func_GetLocalPlayer failed to pattern scan.\n");
		return FALSE;
	}
	else { printf("[Pattern Scan]  Patterns.Func_GetLocalPlayer is at %llx\n", Patterns.Func_GetLocalPlayer); };

	Patterns.Func_OnDispatchMessage = (UINT_PTR)PatternScan(Rs2Client, 0x300000, "\x40\x57\x48\x81\xec\x90\x00\x00\x00\x48\xc7\x44\x24\x30\xfe\xff\xff\xff", "xxxxxxxxxxxxxxxxxx");

	if (!Patterns.Func_OnDispatchMessage)
	{
		printf("[Error] Patterns.Func_OnDIspatchMessage failed to pattern scan.\n");
		return FALSE;
	}
	else { printf("[Pattern Scan]  Patterns.Func_OnDIspatchMessage is at %llx\n", Patterns.Func_OnDispatchMessage); };

	Patterns.Func_GetContainer = (UINT_PTR)PatternScan(Rs2Client, 0x300000, "\x4c\x8b\x51\x10\x44\x8d\x0c\x12\x41\x0f\xb6\xc0", "xxxxxxxxxxxx");

	if (!Patterns.Func_GetContainer)
	{
		printf("[Error] Patterns.Func_GetContainer failed to pattern scan.\n");
		return FALSE;
	}
	else { printf("[Pattern Scan]  Patterns.Func_GetContainer is at %llx\n", Patterns.Func_GetContainer); };

	printf("\n------------------ [ END PATTERN SCAN ] ------------------\n\n");

	return true;
}