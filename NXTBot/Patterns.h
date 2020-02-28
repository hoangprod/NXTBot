#pragma once

struct Addr {
	UINT_PTR Addr_MouseIntersectWorld;

	UINT_PTR Addr_MoveActionHandler;
	UINT_PTR Addr_InventoryActionHandler;

	UINT_PTR Offset_OptionText;
	UINT_PTR Offset_Container;
	UINT_PTR Offset_PlayerContext;
	UINT_PTR Offset_EntityOffsetList;
	UINT_PTR Offset_EntityArrayList;
	UINT_PTR Offset_Health;

	UINT_PTR Offset_AnimationId;  //aa0
	UINT_PTR Offset_AnimationClass;  //380
	UINT_PTR Offset_AnimationSpotId; // 768
	UINT_PTR Offset_GameState;
	UINT_PTR Offset_TargetedEntity;
	UINT_PTR Offset_IsCurrentlyTargeting;

	UINT_PTR Func_OnCursorDoAction;
	UINT_PTR Func_OnCursorWorldContextMenu;
	UINT_PTR Func_GetLocalPlayer;
	UINT_PTR Func_OnDispatchMessage;
	UINT_PTR Func_GetContainer;

};

bool __stdcall findPatterns();