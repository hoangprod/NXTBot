#pragma once

struct Addr {
	UINT_PTR Addr_MouseIntersectWorld;

	UINT_PTR Addr_MoveActionHandler;
	UINT_PTR Addr_InventoryActionHandler;

	UINT_PTR Func_OnCursorDoAction;
	UINT_PTR Func_OnCursorWorldContextMenu;
	UINT_PTR Func_GetLocalPlayer;
	UINT_PTR Func_OnDispatchMessage;
	UINT_PTR Func_GetClientUnit;

};

bool __stdcall findPatterns();