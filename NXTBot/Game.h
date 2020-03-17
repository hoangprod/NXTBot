#pragma once

struct RS {
	static GameContext* GetGameContext();
	static NpcEntityPtr* GetEntityPtr();
	static uint32_t GetEntityCount();
	static uint32_t GetPlayerEntityCount();
	static EntityObj* GetLocalPlayer();
	static PlayerListWrapper* GetPlayerListWrapper();
	static PlayerObjWrapper* GetPlayerObjWrapperByIndex(uint32_t Index);
	static EntityObj* GetPlayerObjByIndex(uint32_t Index);
;	static EntityObjWrap* GetEntityWrapByIndex(uint32_t Index);
	static EntityObj* GetEntityObjByIndex(uint32_t Index);

	static GameState GetGameState();
	static BOOL IsInGame();
	static std::string GetGameStateStr();

	static Tile2D GetMouseIntersectWorldPos();
	static Tile2D GetEntityTilePos(EntityObj* entity);
	static float* GetEntityPos(EntityObj* entity);
	static Tile2D GetLocalPlayerTilePos();
	static float* GetLocalPlayerPos();


	static float GetDistance(Tile2D from, Tile2D to);
	static float GetDistanceFromEntity(uint32_t EntityId);
	static std::vector<EntityObj*> GetInCombatNPCwithMe();
	static std::vector<EntityObj*> GetNPCEntityList();
	static std::vector<EntityObj*> GetPlayerEntityList();
	static EntityObj* GetClosestPlayer();
	static EntityObj* GetClosestMonster();
	static EntityObj* GetEntityNPCByName(const char* name);
	static EntityObj* GetEntityObjectByEntityId(uint32_t EntityId);
	static EntityObj* GetMonsterWithinRadius(Tile2D from, float MaxDistance);
	static EntityObj* GetMonsterWithinRadiusWithName(const char* monsterName, Tile2D from, float MaxDistance);
	static EntityObj* GetClosestMonsterWithinRadius(Tile2D from, float MaxDistance);

	static Tile2D WorldToTilePos(const int32_t wX, const int32_t wY);

	static std::string ItemIdToString(uint32_t itemId);
	static int ItemNameToId(std::string itemName);
};


struct Static
{
	static UINT_PTR GetFullEntityList();

	static StaticObjEX GetClosestStaticObjectByName(const char* name);
	static StaticObjEX GetClosestStaticTreeObjectByName(const char* name);
	static StaticObjEX GetClosestStaticTreeObjectByNameWithOrigin(const char* name, Tile2D origin);

	static EntityObj* GetEntityNpcByName(const char* name);

	static StaticObjEX GetCStaticObjectById(uint32_t id);
	static void GetStaticEntities(std::set<uint64_t>* out);
	static void GetStaticEntities_(uint64_t curr, std::set<uint64_t>* out);
};
