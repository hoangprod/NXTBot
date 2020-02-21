#pragma once

struct RS {
	static GameContext* GetGameContext();
	static EntityPtr* GetEntityPtr();
	static uint32_t GetEntityCount();
	static uint32_t GetPlayerEntityCount();
	static EntityObj* GetLocalPlayer();
	static PlayerListWrapper* GetPlayerListWrapper();
	static PlayerObjWrapper* GetPlayerObjWrapperByIndex(uint32_t Index);
	static EntityObj* GetPlayerObjByIndex(uint32_t Index);
;	static EntityObjWrap* GetEntityWrapByIndex(uint32_t Index);
	static EntityObj* GetEntityObjByIndex(uint32_t Index);

	static Tile2D GetMouseIntersectWorldPos();
	static Tile2D GetLocalPlayerTilePos();
	static float* GetLocalPlayerPos();


	static void LoopEntityList();
	static void LoopPlayerEntityList();

	static Tile2D WorldToTilePos(const int32_t wX, const int32_t wY);
};

