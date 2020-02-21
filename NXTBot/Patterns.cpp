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

	Patterns.Addr_MouseIntersectWorld = (UINT_PTR)ptr_offset_Scanner(Rs2Client, 0x100000, "\x44\x89\x35\xCC\xCC\xCC\x00\x89\x35\xCC\xCC\xCC\x00", 0, 7, 3, "xxx???xxx???x");

	if (!Patterns.Addr_MouseIntersectWorld)
	{
		printf("[Error] Patterns.Addr_MouseIntersectWorld failed to pattern scan.\n");
		return FALSE;
	}
	else { printf("[Pattern Scan]  Patterns.Addr_MouseIntersectWorld is at %llx\n", Patterns.Addr_MouseIntersectWorld); };

	Patterns.Addr_InventoryActionHandler = (UINT_PTR)ptr_offset_Scanner2(Rs2Client, 0x100000, "\x48\x8d\x05\xCC\xCC\xCC\x00\x48\x89\x44\xCC\xCC\x4d\x8b\xc7", 0, 7, 3, "xxx???xxxx??xxx");

	if (!Patterns.Addr_InventoryActionHandler)
	{
		printf("[Error] Patterns.Addr_InventoryActionHandler failed to pattern scan.\n");
		return FALSE;
	}
	else { printf("[Pattern Scan]  Patterns.Addr_InventoryActionHandler is at %llx\n", Patterns.Addr_InventoryActionHandler); };

	Patterns.Addr_MoveActionHandler = (UINT_PTR)ptr_offset_Scanner(Rs2Client, 0x100000, "\x48\x8d\x05\xCC\xCC\xCC\x00\x48\x89\x44\x24\x40\x48\xc7", 0, 7, 3, "xxx???xxxxxxxx");

	if (!Patterns.Addr_MoveActionHandler)
	{
		printf("[Error] Patterns.Addr_MoveActionHandler failed to pattern scan.\n");
		return FALSE;
	}
	else { printf("[Pattern Scan]  Patterns.Addr_MoveActionHandler is at %llx\n", Patterns.Addr_MoveActionHandler); };


	printf("\n------------------ [ OFFSETS ] ------------------\n\n");


	printf("\n------------------ [ FUNCTIONS ] ------------------\n\n");

	Patterns.Func_OnCursorDoAction = (UINT_PTR)PatternScan(Rs2Client, 0x100000, "\x48\x89\x5c\x24\x10\x48\x89\x6c\x24\x18\x56\x48\x83\xec\x20\x48\x8b\x41\x08", "xxxxxxxxxxxxxxxxxxx");

	if (!Patterns.Func_OnCursorDoAction)
	{
		printf("[Error] Patterns.Func_OnCursorDoAction failed to pattern scan.\n");
		return FALSE;
	}
	else { printf("[Pattern Scan]  Patterns.Func_OnCursorDoAction is at %llx\n", Patterns.Func_OnCursorDoAction); };

	Patterns.Func_OnCursorWorldContextMenu = (UINT_PTR)PatternScan(Rs2Client, 0x100000, "\x48\x8b\xc4\x44\x89\x40\x18\x89\x50\x10\x55\x56\x57\x41\x54\x41\x55\x41\x56\x41\x57", "xxxxxxxxxxxxxxxxxxxxx");

	if (!Patterns.Func_OnCursorWorldContextMenu)
	{
		printf("[Error] Patterns.Func_OnCursorWorldContextMenu failed to pattern scan.\n");
		return FALSE;
	}
	else { printf("[Pattern Scan]  Patterns.Func_OnCursorWorldContextMenu is at %llx\n", Patterns.Func_OnCursorWorldContextMenu); };

	Patterns.Func_GetLocalPlayer = (UINT_PTR)PatternScan(Rs2Client, 0x100000, "\x48\x83\x79\xCC\x00\x74\x05\x48\x8d\x41\xCC\xc3", "xxx?xxxxxx?x");

	if (!Patterns.Func_GetLocalPlayer)
	{
		printf("[Error] Patterns.Func_GetLocalPlayer failed to pattern scan.\n");
		return FALSE;
	}
	else { printf("[Pattern Scan]  Patterns.Func_GetLocalPlayer is at %llx\n", Patterns.Func_GetLocalPlayer); };

	Patterns.Func_OnDispatchMessage = (UINT_PTR)PatternScan(Rs2Client, 0x100000, "\x40\x57\x48\x81\xec\x90\x00\x00\x00\x48\xc7\x44\x24\x30\xfe\xff\xff\xff", "xxxxxxxxxxxxxxxxxx");

	if (!Patterns.Func_OnDispatchMessage)
	{
		printf("[Error] Patterns.Func_OnDIspatchMessage failed to pattern scan.\n");
		return FALSE;
	}
	else { printf("[Pattern Scan]  Patterns.Func_OnDIspatchMessage is at %llx\n", Patterns.Func_OnDispatchMessage); };

	printf("\n------------------ [ END PATTERN SCAN ] ------------------\n\n");

	return true;
}