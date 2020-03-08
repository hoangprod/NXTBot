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

	Patterns.Addr_MouseIntersectWorld = (UINT_PTR)ptr_offset_Scanner2(Rs2Client, 0x300000, "\xCC\xCC\xCC\xCC\xCC\xCC\x00\xCC\xCC\xCC\xCC\xCC\xCC\x00\xCC\xCC\xCC\x08\xCC\xCC\xCC\xCC\xCC\x00\x00\xCC\xCC\xCC\xCC\xCC\x00\x00", 0, 7, 3, "??????x??????x???x?????xx?????xx");

	if (!Patterns.Addr_MouseIntersectWorld)
	{
		printf("[Error] Patterns.Addr_MouseIntersectWorld failed to pattern scan.\n");

		Patterns.Addr_MouseIntersectWorld = (UINT_PTR)ptr_offset_Scanner2(Rs2Client, 0x300000, "\x44\x89\x35\xCC\xCC\xCC\x00\x89\x35\xCC\xCC\xCC\x00", 0, 7, 3, "xxx???xxx???x");

		if (!Patterns.Addr_MouseIntersectWorld)
		{
			printf("[Error] Patterns.Addr_MouseIntersectWorld failed to pattern scan.\n");

			return FALSE;
		}
		else { printf("[Pattern Scan]  Patterns.Addr_MouseIntersectWorld is at %llx\n", Patterns.Addr_MouseIntersectWorld); };

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
	// f58
	Patterns.Offset_OptionText = (UINT_PTR)Scan_Offsets(Rs2Client, 0x300000, "\x48\x83\xCC\xCC\xCC\x00\x00\x00\x0f\xCC\xCC\x00\x00\x00\x45", "xx???xxxx??xxxx", 3, 4);

	if (!Patterns.Offset_OptionText)
	{
		Patterns.Offset_OptionText = (UINT_PTR)Scan_Offsets(Rs2Client, 0x300000, "\x48\x83\xb9\xCC\xCC\x00\x00\x00\x75\xCC\x32\xc0\xc3", "xxx??xxxx?xxx", 3, 4);

		if (!Patterns.Offset_OptionText)
		{
			printf("[Error] Patterns.Offset_OptionText failed to pattern scan.\n");
			return FALSE;
		}
		else { printf("[Pattern Scan]  Patterns.Offset_OptionText is at %llx\n", Patterns.Offset_OptionText); };
	}
	else { printf("[Pattern Scan]  Patterns.Offset_OptionText is at %llx\n", Patterns.Offset_OptionText); };


	// 1238
	Patterns.Offset_Container = (UINT_PTR)Scan_Offsets(Rs2Client, 0x300000, "\x48\x8b\xCC\x38\x12\x00\x00\x44", "xx?xxxxx", 3, 4);

	if (!Patterns.Offset_Container)
	{
		Patterns.Offset_Container = (UINT_PTR)Scan_Offsets(Rs2Client, 0x300000, "\x48\x8b\xb0\xCC\xCC\x00\x00\x44\x0f\xb6\xc7", "xxx??xxxxxx", 3, 4);

		if (!Patterns.Offset_Container)
		{
			printf("[Error] Patterns.Offset_Container failed to pattern scan.\n");
			return FALSE;
		}
		else { printf("[Pattern Scan]  Patterns.Offset_Container is at %llx\n", Patterns.Offset_Container); };
	}
	else { printf("[Pattern Scan]  Patterns.Offset_Container is at %llx\n", Patterns.Offset_Container); };


	// 1780
	Patterns.Offset_PlayerContext = (UINT_PTR)Scan_Offsets(Rs2Client, 0x300000, "\x48\x8b\x88\xCC\xCC\x00\x00\xe8\xCC\xCC\xCC\xff\x48\x8b\x48\x08", "xxx??xxx???xxxxx", 3, 4);

	if (!Patterns.Offset_PlayerContext)
	{
		printf("[Error] Patterns.Offset_PlayerContext failed to pattern scan.\n");
		return FALSE;
	}
	else { printf("[Pattern Scan]  Patterns.Offset_PlayerContext is at %llx\n", Patterns.Offset_PlayerContext); };

	// 4038
	Patterns.Offset_EntityArrayList = (UINT_PTR)Scan_Offsets(Rs2Client, 0x300000, "\x48\x8b\xCC\xCC\x40\x00\x00\x48\x8b\xCC\xCC\x40\x00\x00\x48", "xx??xxxxx??xxxx", 3, 4);

	if (!Patterns.Offset_EntityArrayList)
	{
		Patterns.Offset_EntityArrayList = (UINT_PTR)Scan_Offsets(Rs2Client, 0x300000, "\x48\x8b\x83\xCC\xCC\x00\x00\x48\x63\x0c\x88", "xxx??xxxxxx", 3, 4);

		if (!Patterns.Offset_EntityArrayList)
		{
			printf("[Error] Patterns.Offset_EntityArrayList failed to pattern scan.\n");
			return FALSE;
		}
		else { printf("[Pattern Scan]  Patterns.Offset_EntityArrayList is at %llx\n", Patterns.Offset_EntityArrayList); };
	}
	else { printf("[Pattern Scan]  Patterns.Offset_EntityArrayList is at %llx\n", Patterns.Offset_EntityArrayList); };


	//a0b0
	Patterns.Offset_EntityOffsetList = (UINT_PTR)Scan_Offsets(Rs2Client, 0x300000, "\x4c\x63\x84\xbb\xCC\xCC\x00\x00\x4c\x8b\x4b", "xxxx??xxxxx", 4, 4);

	if (!Patterns.Offset_EntityOffsetList)
	{
		printf("[Error] Patterns.Offset_EntityOffsetList failed to pattern scan.\n");
		return FALSE;
	}
	else { printf("[Pattern Scan]  Patterns.Offset_EntityOffsetList is at %llx\n", Patterns.Offset_EntityOffsetList); };

	//fd0
	Patterns.Offset_Health = (UINT_PTR)Scan_Offsets(Rs2Client, 0x300000, "\x41\x8b\xCC\xCC\xCC\x0f\x00\x00\x45\x8b\xCC\xCC\xCC\xCC\x00\x00", "xx???xxxxx????xx", 4, 4);

	if (!Patterns.Offset_Health)
	{
		Patterns.Offset_Health = (UINT_PTR)Scan_Offsets(Rs2Client, 0x300000, "\x41\x8b\x94\x88\xCC\xCC\x00\x00\x45\x8b\x84\x88\xCC\xCC\x00\x00", "xxxx??xxxxxx??xx", 4, 4);

		if (!Patterns.Offset_Health)
		{
			printf("[Error] Patterns.Offset_Health failed to pattern scan.\n");
			return FALSE;
		}
		else { printf("[Pattern Scan]  Patterns.Offset_Health is at %llx\n", Patterns.Offset_Health); };
	}
	else { printf("[Pattern Scan]  Patterns.Offset_Health is at %llx\n", Patterns.Offset_Health); };


	// a00
	Patterns.Offset_AnimationId = (UINT_PTR)Scan_Offsets(Rs2Client, 0x300000, "\x44\x8b\xCC\x00\xCC\x00\x00\x48\x8d\xCC\xCC\x48", "xx?x?xxxx??x", 3, 4);

	if (!Patterns.Offset_AnimationId)
	{
		Patterns.Offset_AnimationId = (UINT_PTR)Scan_Offsets(Rs2Client, 0x300000, "\x44\x8b\x89\xCC\xCC\x00\x00\x48\x8d\x0c\x82", "xxx??xxxxxx", 3, 4);

		if (!Patterns.Offset_AnimationId)
		{
			printf("[Error] Patterns.Offset_AnimationId failed to pattern scan.\n");
			return FALSE;
		}
		else { printf("[Pattern Scan]  Patterns.Offset_AnimationId is at %llx\n", Patterns.Offset_AnimationId); };
	}
	else { printf("[Pattern Scan]  Patterns.Offset_AnimationId is at %llx\n", Patterns.Offset_AnimationId); };



	// 380
	Patterns.Offset_AnimationClass = (UINT_PTR)Scan_Offsets(Rs2Client, 0x300000, "\x48\xCC\xCC\xCC\x03\x00\x00\xc6\xCC\xCC\xCC\x00\x45", "x???xxxx???xx", 3, 4);

	if (!Patterns.Offset_AnimationClass)
	{
		Patterns.Offset_AnimationClass = (UINT_PTR)Scan_Offsets(Rs2Client, 0x300000, "\x48\x81\xc1\xCC\xCC\x00\x00\x8b\x93\xCC\xCC\x00\x00", "xxx??xxxx??xx", 3, 4);

		if (!Patterns.Offset_AnimationClass)
		{
			printf("[Error] Patterns.Offset_AnimationClass failed to pattern scan.\n");
			return FALSE;
		}
		else { printf("[Pattern Scan]  Patterns.Offset_AnimationClass is at %llx\n", Patterns.Offset_AnimationClass); };
	}
	else { printf("[Pattern Scan]  Patterns.Offset_AnimationClass is at %llx\n", Patterns.Offset_AnimationClass); };


	// 768
	Patterns.Offset_AnimationSpotId = (UINT_PTR)Scan_Offsets(Rs2Client, 0x300000, "\x83\xCC\xCC\x07\x00\x00\xff\x74\xCC\x48\xCC\xCC\xCC\x07\x00\x00\xCC\xCC\xCC\x8b", "x??xxxxx?x???xxx???x", 2, 4);

	if (!Patterns.Offset_AnimationSpotId)
	{
		Patterns.Offset_AnimationSpotId = (UINT_PTR)Scan_Offsets(Rs2Client, 0x300000, "\x83\xb9\xCC\xCC\x00\x00\xff\x0f\xCC\xCC\x00\x00\x00\x48", "xx??xxxx??xxxx", 2, 4);

		if (!Patterns.Offset_AnimationSpotId)
		{
			printf("[Error] Patterns.Offset_AnimationSpotId failed to pattern scan.\n");
			return FALSE;
		}
		else { printf("[Pattern Scan]  Patterns.Offset_AnimationSpotId is at %llx\n", Patterns.Offset_AnimationSpotId); };
	}
	else { printf("[Pattern Scan]  Patterns.Offset_AnimationSpotId is at %llx\n", Patterns.Offset_AnimationSpotId); };




	// 177c
	Patterns.Offset_GameState = (UINT_PTR)Scan_Offsets(Rs2Client, 0x300000, "\x48\x8b\xf9\x3b\x91\xCC\xCC\x00\x00\x74", "xxxxx??xxx", 5, 4);

	if (!Patterns.Offset_GameState)
	{
		printf("[Error] Patterns.Offset_GameState failed to pattern scan.\n");
		return FALSE;
	}
	else { printf("[Pattern Scan]  Patterns.Offset_GameState is at %llx\n", Patterns.Offset_GameState); };



	// 2c8
	Patterns.Offset_IsCurrentlyTargeting = (UINT_PTR)Scan_Offsets(Rs2Client, 0x500000, "\x41\xCC\xCC\xCC\x02\x00\x00\x01\x00\x00\x00\xeb", "x???xxxxxxxx", 3, 4);

	if (!Patterns.Offset_IsCurrentlyTargeting)
	{
		Patterns.Offset_IsCurrentlyTargeting = (UINT_PTR)Scan_Offsets(Rs2Client, 0x500000, "\x41\xc7\x85\xCC\xCC\x00\x00\x01\x00\x00\x00\xeb", "xxx??xxxxxxx", 3, 4);

		if (!Patterns.Offset_IsCurrentlyTargeting)
		{
			printf("[Error] Patterns.Offset_IsCurrentlyTargeting failed to pattern scan.\n");
			return FALSE;
		}
		else { printf("[Pattern Scan]  Patterns.Offset_IsCurrentlyTargeting is at %llx\n", Patterns.Offset_IsCurrentlyTargeting); };
	}
	else { printf("[Pattern Scan]  Patterns.Offset_IsCurrentlyTargeting is at %llx\n", Patterns.Offset_IsCurrentlyTargeting); };



	// 254
	Patterns.Offset_TargetedEntity = (UINT_PTR)Scan_Offsets(Rs2Client, 0x300000, "\x48\xCC\xCC\xCC\x02\x00\x00\x8b\xCC\xCC\x02\x00\x00\x83\xCC\x02", "x???xxxx??xxxx?x", 3, 4);

	if (!Patterns.Offset_TargetedEntity)
	{
		Patterns.Offset_TargetedEntity = (UINT_PTR)Scan_Offsets(Rs2Client, 0x300000, "\x41\x39\x9d\xCC\xCC\x00\x00\x75\xCC\x41\x83\xbd", "xxx??xxx?xxx", 3, 4);

		if (!Patterns.Offset_TargetedEntity)
		{
			printf("[Error] Patterns.Offset_TargetedEntity failed to pattern scan.\n");
			return FALSE;
		}
		else { printf("[Pattern Scan]  Patterns.Offset_TargetedEntity is at %llx\n", Patterns.Offset_TargetedEntity); };
	}
	else { printf("[Pattern Scan]  Patterns.Offset_TargetedEntity is at %llx\n", Patterns.Offset_TargetedEntity); };

	// 11a8
	Patterns.Offset_CharacterInfo = (UINT_PTR)Scan_Offsets(Rs2Client, 0x300000, "\x48\xCC\xCC\xCC\x11\x00\x00\x48\xCC\xCC\xCC\xCC\xCC\x00\x4b", "x???xxxx?????xx", 3, 4);

	if (!Patterns.Offset_CharacterInfo)
	{
		printf("[Error] Patterns.Offset_CharacterInfo failed to pattern scan.\n");
		return FALSE;
	}
	else { printf("[Pattern Scan]  Patterns.Offset_CharacterInfo is at %llx\n", Patterns.Offset_CharacterInfo); };

	// 6670
	Patterns.Offset_SkillExperience = (UINT_PTR)Scan_Offsets(Rs2Client, 0x300000, "\x4c\xCC\xCC\xCC\x66\x00\x00\x49\xCC\xCC\x10", "x???xxxx??x", 3, 4);

	if (!Patterns.Offset_SkillExperience)
	{
		printf("[Error] Patterns.Offset_SkillExperience failed to pattern scan.\n");
		return FALSE;
	}
	else { printf("[Pattern Scan]  Patterns.Offset_SkillExperience is at %llx\n", Patterns.Offset_SkillExperience); };

	// 11c0
	Patterns.Offset_TileList = (UINT_PTR)Scan_Offsets(Rs2Client, 0x300000, "\xCC\xCC\xCC\xCC\x11\x00\x00\xCC\xCC\xCC\x38\xCC\xCC\xCC\x30\xCC\xCC\xCC\x0f", "????xxx???x???x???x", 3, 4);

	if (!Patterns.Offset_TileList)
	{
		printf("[Error] Patterns.Offset_TileList failed to pattern scan.\n");
		return FALSE;
	}
	else { printf("[Pattern Scan]  Patterns.Offset_TileList is at %llx\n", Patterns.Offset_TileList); };



	// 108
	Patterns.Offset_LootObject = (UINT_PTR)Scan_Offsets(Rs2Client, 0x300000, "\x48\x8b\xCC\xCC\x01\x00\x00\x48\x8b\xCC\xCC\x01\x00\x00\x48\xCC\xCC\x0f", "xx??xxxxx??xxxx??x", 3, 4);

	if (!Patterns.Offset_LootObject)
	{
		printf("[Error] Patterns.Offset_LootObject failed to pattern scan.\n");
		return FALSE;
	}
	else { printf("[Pattern Scan]  Patterns.Offset_LootObject is at %llx\n", Patterns.Offset_LootObject); };

	// 1180
	Patterns.Offset_Widget = (UINT_PTR)Scan_Offsets(Rs2Client, 0x300000, "\x48\xCC\xCC\xCC\x11\x00\x00\xe8\xCC\xCC\xCC\x00\x48\xCC\xCC\x48\xCC\xCC\x08", "x???xxxx???xx??x??x", 3, 4);

	if (!Patterns.Offset_Widget)
	{
		printf("[Error] Patterns.Offset_Widget failed to pattern scan.\n");
		return FALSE;
	}
	else { printf("[Pattern Scan]  Patterns.Offset_Widget is at %llx\n", Patterns.Offset_Widget); };



	Patterns.Offset_Ptr1 = (UINT_PTR)Scan_Offsets(Rs2Client, 0x300000, "\xCC\xCC\xCC\xCC\x12\x00\x00\xCC\xCC\xCC\x90\x00\x00\x00", "????xxx???xxxx", 3, 4);

	if (!Patterns.Offset_Ptr1)
	{
		printf("[Error] Patterns.Offset_Ptr1 failed to pattern scan.\n");
		return FALSE;
	}
	else { printf("[Pattern Scan]  Patterns.Offset_Ptr1 is at %llx\n", Patterns.Offset_Ptr1); };


	Patterns.Offset_Camera = (UINT_PTR)Scan_Offsets(Rs2Client, 0x300000, "\xCC\xCC\xCC\xCC\x00\x01\x00\xe8", "????xxxx", 3, 4);

	if (!Patterns.Offset_Camera)
	{
		printf("[Error] Patterns.Offset_Camera failed to pattern scan.\n");
		return FALSE;
	}
	else { printf("[Pattern Scan]  Patterns.Offset_Camera is at %llx\n", Patterns.Offset_Camera); };

	Patterns.Offset_StaticEntityPtr = (UINT_PTR)Scan_Offsets2(Rs2Client, 0x300000, "\x48\x8b\xCC\xCC\x01\x00\x00\x4c\xCC\xCC\x48\xCC\xCC\x48", "xx??xxxx??x??x", 3, 4);

	if (!Patterns.Offset_StaticEntityPtr)
	{
		printf("[Error] Patterns.Offset_StaticEntityPtr failed to pattern scan.\n");
		return FALSE;
	}
	else { printf("[Pattern Scan]  Patterns.Offset_StaticEntityPtr is at %llx\n", Patterns.Offset_StaticEntityPtr); };


	Patterns.Offset_StaticAnd20 = (UINT_PTR)Scan_Offsets2(Rs2Client, 0x300000, "\xCC\xCC\xCC\xCC\x00\x00\x00\xf6\xCC\x20", "????xxxx?x", 3, 4);

	if (!Patterns.Offset_StaticAnd20)
	{
		printf("[Error] Patterns.Offset_StaticAnd20 failed to pattern scan.\n");
		return FALSE;
	}
	else { printf("[Pattern Scan]  Patterns.Offset_StaticAnd20 is at %llx\n", Patterns.Offset_StaticAnd20); };



	printf("\n------------------ [ FUNCTIONS ] ------------------\n\n");

	Patterns.Func_OnCursorDoAction = (UINT_PTR)PatternScan(Rs2Client, 0x300000, "\x48\x89\x5c\x24\x10\x48\x89\x6c\x24\x18\x56\x48\x83\xec\x20\x48\x8b\x41\x08", "xxxxxxxxxxxxxxxxxxx");

	if (!Patterns.Func_OnCursorDoAction)
	{
		printf("[Error] Patterns.Func_OnCursorDoAction failed to pattern scan.\n");
		return FALSE;
	}
	else { printf("[Pattern Scan]  Patterns.Func_OnCursorDoAction is at %llx\n", Patterns.Func_OnCursorDoAction); };



	Patterns.Func_OnCursorWorldContextMenu = (UINT_PTR)PatternScan(Rs2Client, 0x300000, "\x48\x8b\xc4\x89\x50\x10\x55\x56\x57\x41\x54\x41\x55\x41\x56\x41\x57", "xxxxxxxxxxxxxxxxx");

	if (!Patterns.Func_OnCursorWorldContextMenu)
	{
		Patterns.Func_OnCursorWorldContextMenu = (UINT_PTR)PatternScan(Rs2Client, 0x300000, "\x48\x8b\xc4\x44\x89\x40\x18\x89\x50\x10\x55\x56\x57\x41\x54\x41\x55\x41\x56\x41\x57", "xxxxxxxxxxxxxxxxxxxxx");

		if (!Patterns.Func_OnCursorWorldContextMenu)
		{
			printf("[Error] Patterns.Func_OnCursorWorldContextMenu failed to pattern scan.\n");
			return FALSE;
		}
		else { printf("[Pattern Scan]  Patterns.Func_OnCursorWorldContextMenu is at %llx\n", Patterns.Func_OnCursorWorldContextMenu); };
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

	Patterns.Func_GetContainer = (UINT_PTR)PatternScan(Rs2Client, 0x300000, "\x4c\x8b\xCC\x10\x44\x8d\x0c\x12\x41\x0f\xb6\xc0", "xx?xxxxxxxxx");

	if (!Patterns.Func_GetContainer)
	{
		printf("[Error] Patterns.Func_GetContainer failed to pattern scan.\n");
		return FALSE;
	}
	else { printf("[Pattern Scan]  Patterns.Func_GetContainer is at %llx\n", Patterns.Func_GetContainer); };

	printf("\n------------------ [ END PATTERN SCAN ] ------------------\n\n");

	return true;
}