#include "pch.h"
#include "GameClasses.h"
#include "Patterns.h"
#include "ItemDef.h"
#include "PlayableEntity.h"
#include "Common.h"
#include "Game.h"

extern UINT_PTR* g_GameContext;
extern Addr Patterns;
extern fn_GetLocalPlayer o_GetLocalPlayer;

using nlohmann::json;
extern json itemList;

GameContext* RS::GetGameContext()
{
	if(!g_GameContext)
		return nullptr;
	return (GameContext*)g_GameContext[1];
}

NpcEntityPtr* RS::GetEntityPtr()
{
	auto GameContext = GetGameContext();

	if(!GameContext)
		return nullptr;
	
	return (NpcEntityPtr*)GameContext->EntityPtr;
}

uint32_t RS::GetEntityCount()
{
	auto EntityPtr = GetEntityPtr();

	if (!EntityPtr)
		return 0;

	return EntityPtr->EntityCnt;
}

uint32_t RS::GetPlayerEntityCount()
{
	auto PlayerListWrapper = (UINT_PTR*)GetPlayerListWrapper();

	if (!PlayerListWrapper)
		return 0;

	UINT_PTR result = PlayerListWrapper[(Patterns.Offset_EntityArrayList/8)+1] - PlayerListWrapper[(Patterns.Offset_EntityArrayList / 8)] >> 2;
	//UINT_PTR result = PlayerListWrapper[2056] - PlayerListWrapper[2055] >> 2;
	//UINT_PTR result = ReadPtrOffset(PlayerListWrapper, Patterns.Offset_EntityArrayList + 8) - ReadPtrOffset(PlayerListWrapper, Patterns.Offset_EntityArrayList) >> 2;

	return result;
}

DestinationFlag* RS::GetDestinationFlag()
{
	auto g_gameContextDef = g_GameContext[1];

	if (!g_gameContextDef)
		return 0;

	DestinationFlag* destFlag = *(DestinationFlag**)(g_gameContextDef + Patterns.Offset_DestinationFlag);

	return destFlag;
}

EntityObj* RS::GetLocalPlayer()
{
	if (!g_GameContext)
		return 0;

	auto g_gameContextDef = g_GameContext[1];

	if (!g_gameContextDef)
		return 0;

	UINT_PTR* PlayerObj = *(UINT_PTR**)(g_gameContextDef + Patterns.Offset_PlayerContext);

	if (!PlayerObj)
		return 0;

	auto player = (EntityObj*)(o_GetLocalPlayer(PlayerObj)[1]);

	return player;
}

PlayerListWrapper* RS::GetPlayerListWrapper()
{
	auto GameContext = GetGameContext();

	if (!GameContext)
		return nullptr;

	return GameContext->PlayerListWrapper;
}

PlayerObjWrapper* RS::GetPlayerObjWrapperByIndex(uint32_t Index)
{
	UINT_PTR* PlayerListWrapper = (UINT_PTR*)GetPlayerListWrapper();

	if (!PlayerListWrapper)
		return 0;

	int EntityIdx = *(int*)(ReadPtrOffset(PlayerListWrapper, Patterns.Offset_EntityArrayList) + 4 * (UINT_PTR)Index);

	if (!PlayerListWrapper)
		return 0;

	PlayerObjWrapper* PlayerObjWrap = *(PlayerObjWrapper**)(PlayerListWrapper[2] + 8 * (UINT_PTR)EntityIdx);

	if (!PlayerObjWrap || !PlayerObjWrap->EntityObj)
		return 0;

	return PlayerObjWrap;
}

EntityObj* RS::GetPlayerObjByIndex(uint32_t Index)
{
	auto PlayerObjWrapper = GetPlayerObjWrapperByIndex(Index);

	if (!PlayerObjWrapper)
		return 0;

	return PlayerObjWrapper->EntityObj;
}

EntityObjWrap* RS::GetEntityWrapByIndex(uint32_t Index)
{
	auto EntityPtr = (UINT_PTR)GetEntityPtr();

	if (!EntityPtr)
		return nullptr;

	UINT_PTR entityList = *(UINT_PTR*)(EntityPtr + 0x20);

	int EntityOffset = *(int*)(EntityPtr + 4 * (UINT_PTR)Index + Patterns.Offset_EntityOffsetList) % (unsigned __int64) * (unsigned int*)(EntityPtr + 0x28);

	EntityObjWrap* entitywrap = *(EntityObjWrap**)(entityList + 8 * (UINT_PTR)EntityOffset);

	if (!entitywrap)
		return 0;

	return entitywrap;
}

EntityObj* RS::GetEntityObjByIndex(uint32_t Index)
{
	auto EntityPtrWrap = GetEntityWrapByIndex(Index);

	if (!EntityPtrWrap)
		return 0;

	return EntityPtrWrap->EntityObj;
}

game_state RS::GetGameState()
{
	GameContext* context = RS::GetGameContext();

	if (!context)
		return game_state::Unknown;

	return *(game_state*)(((UINT_PTR)context) + Patterns.Offset_GameState);
}

BOOL RS::IsInGame()
{
	return GetGameState() == game_state::Ingame;
}

std::string RS::GetGameStateStr()
{
	game_state state = RS::GetGameState();

	switch (state)
	{
	case game_state::LoginScreen:
		return "Login Screen";
		break;
	case game_state::Lobby:
		return "Lobby";
		break;
	case game_state::Ingame:
		return "Ingame";
		break;
	case game_state::Disconnected:
		return "Disconnected";
		break;
	case game_state::Reconnecting:
		return "Reconnecting";
		break;
	case game_state::PleaseWait:
		return "Please Wait";
		break;
	case game_state::Unknown:
		return "Invalid Context";
		break;
	default:
		return "Unknown";
		break;
	}

}


Tile2D RS::GetMouseIntersectWorldPos()
{
	return *(Tile2D*)Patterns.Addr_MouseIntersectWorld;
}

Tile2D RS::GetEntityTilePos(EntityObj* entity)
{
	if (!entity)
		return Tile2D();

	float * pos = (float*)entity->GetPos();

	return WorldToTilePos(pos[0], pos[2]);
}

float* RS::GetEntityPos(EntityObj* entity)
{
	if (!entity)
		return 0;

	float* pos = (float*)entity->GetPos();

	return pos;
}

Tile2D RS::GetLocalPlayerTilePos()
{
	auto PlayerWorldPos = GetLocalPlayerPos();

	if(!PlayerWorldPos)
		return Tile2D();

	return WorldToTilePos(PlayerWorldPos[0], PlayerWorldPos[2]);
}

float* RS::GetLocalPlayerPos()
{
	auto lp = GetLocalPlayer();

	if (!lp)
		return 0;


	return (float*)lp->GetPos();
}

float RS::GetDistance(Tile2D from, Tile2D to)
{
	return sqrtf(pow(((int64_t)from.x - (int64_t)to.x), 2) + pow(((int64_t)from.y - (int64_t)to.y), 2));
}

float RS::GetDistanceFromEntity(uint32_t EntityId)
{
	auto Entity = GetEntityObjectByEntityId(EntityId);

	if(!Entity)
		return -1.0f;
}

std::vector<EntityObj*> RS::GetInCombatNPCwithMe()
{
	EntityObj* player = GetLocalPlayer();

	std::vector<EntityObj*> EntityList = RS::GetNPCEntityList();

	std::vector<EntityObj*> AttackingNPCs;

	if (!player || EntityList.size() <= 0)
		return AttackingNPCs;

	for (auto entity : EntityList)
	{
		if (entity)
		{
			Entity ent = Entity(entity);

			if (ent.CurrentTarget() == player->EntityId)
			{
				auto npcdef = NpcDef(entity);
				
				if(npcdef.HaveOption("Attack"))
					AttackingNPCs.push_back(entity);
			}

		}
	}

	return AttackingNPCs;
}

std::vector<EntityObj*> RS::GetNPCEntityList()
{
	auto Count = GetEntityCount();

	std::vector<EntityObj*> EntityList;

	if (!Count)
		return EntityList;

	EntityList.reserve(Count + 10);

	for (uint32_t i = 0; i < Count + 10; i++)
	{
		auto entity = GetEntityObjByIndex(i);

		if (!entity || *(UINT_PTR*)entity == 0)
			continue;

		if (strlen(entity->Name) > 0)
		{
			EntityList.push_back(entity);
		}
	}

	return EntityList;
}

std::vector<EntityObj*> RS::GetPlayerEntityList()
{
	auto Count = GetPlayerEntityCount();

	std::vector<EntityObj*> EntityList;

	if (!Count)
		return EntityList;

	EntityList.reserve(Count + 10);

	for (int i = 0; i < Count + 10; i++)
	{
		auto entity = GetPlayerObjByIndex(i);

		if (!entity || *(UINT_PTR*)entity == 0)
			continue;

		if (strlen(entity->Name) > 0)
		{
			EntityList.push_back(entity);
		}
	}

	return EntityList;
}

EntityObj* RS::GetClosestPlayer()
{
	auto Count = GetPlayerEntityCount();

	EntityObj* ret = 0;
	float closest = 99999.0f;

	if (!Count)
		return 0;

	auto player = GetLocalPlayer();

	auto localPlayerTile = GetLocalPlayerTilePos();
	for (uint32_t i = 0; i < Count + 20; i++)
	{

		auto entity = GetPlayerObjByIndex(i);

		if (!entity || *(UINT_PTR*)entity == 0)
			continue;


		if (strlen(entity->Name) > 0 && entity->EntityId != player->EntityId)
		{

			float distance = GetDistance(localPlayerTile, GetEntityTilePos(entity));

			if (distance < closest)
			{
				closest = distance;
				ret = entity;
			}
		}
	}

	if (ret && strlen(ret->Name) > 0)
		return ret;

	return 0;
}

std::vector<std::string> friendlyList = { "B0RAVlR", "MOCHlBUN", "3_HEPH_YEETY", "BlackPEARLZ", "22ricecakess", "wotyoproblem", "redenvlope"};

EntityObj* RS::GetAnyPlayer()
{
	auto Count = GetPlayerEntityCount();

	EntityObj* ret = 0;

	if (!Count)
		return 0;

	auto player = GetLocalPlayer();

	for (uint32_t i = 0; i < Count + 20; i++)
	{
		auto entity = GetPlayerObjByIndex(i);

		if (!entity || *(UINT_PTR*)entity == 0)
			continue;


		if (strlen(entity->Name) > 0 && entity->EntityId != player->EntityId)
		{
			if (std::find(friendlyList.begin(), friendlyList.end(), std::string(entity->Name)) != friendlyList.end())
			{
				printf("Found Friendly %s\n", entity->Name);
				continue;
			}

			return entity;
		}
	}

	return 0;
}

EntityObj* RS::GetClosestMonster()
{
	auto Count = GetEntityCount();

	EntityObj* ret = 0;
	float closest = 99999.0f;

	if (!Count)
		return 0;

	auto localPlayerTile = GetLocalPlayerTilePos();

	for (uint32_t i = 0; i < Count + 20; i++)
	{
		auto entity = GetEntityObjByIndex(i);


		if (!entity || *(UINT_PTR*)entity == 0 || entity->EntityType != 1)
			continue;

		float distance = GetDistance(localPlayerTile, GetEntityTilePos(entity));

		if (distance < closest)
		{
			closest = distance;
			ret = entity;
		}
	}

	if (ret && strlen(ret->Name) > 0)
		return ret;

	return 0;
}

EntityObj* RS::GetEntityNPCByName(const char* name)
{
	auto Entities = GetNPCEntityList();

	for (auto ent : Entities)
	{
		if (strcmp(name, ent->Name) == 0)
		{
			return ent;
		}
	}

	return nullptr;
}

EntityObj* RS::GetClosestMonsterNPCByName(const char* name)
{
	Player player = RS::GetLocalPlayer();

	auto Count = GetEntityCount();

	EntityObj* ret = 0;

	float closest = 99999.0f;

	if (!Count)
		return 0;

	for (uint32_t i = 0; i < Count + 20; i++)
	{
		auto entity = GetEntityObjByIndex(i);


		if (!entity || *(UINT_PTR*)entity == 0 || entity->EntityType != 1)
			continue;

		float distance = GetDistance(player.GetTilePosition(), GetEntityTilePos(entity));

		if (distance < closest && strcmp(entity->Name, name) == 0)
		{
			auto npcdef = NpcDef(entity);

			if (npcdef.HaveOption("Attack"))
			{
				closest = distance;
				ret = entity;
			}

		}
	}

	if (ret && strlen(ret->Name) > 0)
		return ret;

	return 0;
}


EntityObj* RS::GetClosestMonsterNPCByNameFromOrigin(const char* name, Tile2D from, float maxDistance)
{
	auto Count = GetEntityCount();

	EntityObj* ret = 0;

	float closest = 99999.0f;

	if (!Count)
		return 0;

	for (uint32_t i = 0; i < Count + 20; i++)
	{
		auto entity = GetEntityObjByIndex(i);


		if (!entity || *(UINT_PTR*)entity == 0 || entity->EntityType != 1)
			continue;

		float distance = GetDistance(from, GetEntityTilePos(entity));

		if (distance <= maxDistance && distance < closest && strcmp(entity->Name, name) == 0)
		{
			auto npcdef = NpcDef(entity);

			if (npcdef.HaveOption("Attack"))
			{
				closest = distance;
				ret = entity;
			}

		}
	}

	if (ret && strlen(ret->Name) > 0)
		return ret;

	return 0;
}

EntityObj* RS::GetValidWildernessPlayerEnemy()
{
	auto Count = GetPlayerEntityCount();

	EntityObj* ret = 0;

	if (!Count)
		return 0;

	auto player = GetLocalPlayer();

	int WildernessLevel = Common::GetCurrentWildernessLevel();

	auto localPlayerTile = GetLocalPlayerTilePos();

	for (uint32_t i = 0; i < Count + 20; i++)
	{

		auto entity = GetPlayerObjByIndex(i);

		if (!entity || *(UINT_PTR*)entity == 0 || RS::GetEntityTilePos(entity).y < 3525)
			continue;


		if (strlen(entity->Name) > 0 && entity->EntityId != player->EntityId)
		{
			int levelDif = std::abs((int)entity->Level - (int)player->Level);

			if (levelDif <= WildernessLevel + 2)
			{
				float distance = GetDistance(localPlayerTile, GetEntityTilePos(entity));

				if (distance < 105.0f)
				{
					return entity;
				}
			}


		}
	}

	return 0;
}

EntityObj* RS::GetClosestEntityNPCByName(const char* name, bool bClosest)
{
	Player player = RS::GetLocalPlayer();

	auto Count = GetEntityCount();

	EntityObj* ret = 0;

	float closest = 99999.0f;

	if (!Count)
		return 0;

	for (uint32_t i = 0; i < Count + 20; i++)
	{
		auto entity = GetEntityObjByIndex(i);


		if (!entity || *(UINT_PTR*)entity == 0 || entity->EntityType != 1)
			continue;

		float distance = GetDistance(player.GetTilePosition(), GetEntityTilePos(entity));

		if (distance < closest && strcmp(entity->Name, name) == 0)
		{
			closest = distance;
			ret = entity;

			if (!bClosest)
				break;
		}
	}

	if (ret && strlen(ret->Name) > 0)
		return ret;

	return 0;
}

EntityObj* RS::GetClosestEntityNPCByNameStrStr(const char* name, bool bClosest)
{
	Player player = RS::GetLocalPlayer();

	auto Count = GetEntityCount();

	EntityObj* ret = 0;

	float closest = 99999.0f;

	if (!Count)
		return 0;

	for (uint32_t i = 0; i < Count + 20; i++)
	{
		auto entity = GetEntityObjByIndex(i);


		if (!entity || *(UINT_PTR*)entity == 0 || entity->EntityType != 1)
			continue;

		float distance = GetDistance(player.GetTilePosition(), GetEntityTilePos(entity));

		if (distance < closest && strstr(entity->Name, name) != 0)
		{
			closest = distance;
			ret = entity;

			if (!bClosest)
				break;
		}
	}

	if (ret && strlen(ret->Name) > 0)
		return ret;

	return 0;
}

EntityObj* RS::GetEntityObjectByEntityId(uint32_t EntityId)
{
	auto Entities = GetNPCEntityList();

	for (auto ent : Entities)
	{
		if (ent->EntityId == EntityId)
		{
			return ent;
		}
	}

	return nullptr;
}

EntityObj* RS::GetMonsterWithinRadius(Tile2D from, float MaxDistance)
{
	auto Count = GetEntityCount();

	if (!Count)
		return 0;

	for (uint32_t i = 0; i < Count + 20; i++)
	{
		auto entity = GetEntityObjByIndex(i);

		auto ent = Entity(entity);

		if (!entity || *(UINT_PTR*)entity == 0 || entity->EntityType != 1 || ent.NPCCurHealth() == 0)
			continue;

		float distance = GetDistance(from, GetEntityTilePos(entity));

		if (distance < MaxDistance)
		{
			if (entity && strlen(entity->Name) > 0)
			{
				auto npcdef = NpcDef(entity);

				if (npcdef.HaveOption("Attack"))
					return entity;
			}
		}
	}

	return 0;
}


EntityObj* RS::GetMonsterWithinRadiusWithName(const char* monsterName, Tile2D from, float MaxDistance)
{
	auto Count = GetEntityCount();

	if (!Count)
		return 0;

	for (uint32_t i = 0; i < Count + 20; i++)
	{
		auto entity = GetEntityObjByIndex(i);

		auto ent = Entity(entity);

		if (!entity || *(UINT_PTR*)entity == 0 || entity->EntityType != 1 || ent.NPCCurHealth() == 0)
			continue;

		float distance = GetDistance(from, GetEntityTilePos(entity));

		if (distance < MaxDistance)
		{
			if (entity && strlen(entity->Name) > 0 && strcmp(entity	->Name, monsterName) == 0)
				return entity;
		}
	}

	return 0;
}

EntityObj* RS::GetClosestMonsterWithinRadius(Tile2D from, float MaxDistance)
{
	auto Count = GetEntityCount();

	EntityObj* ret = 0;

	float closest = 99999.0f;

	if (!Count)
		return 0;

	for (uint32_t i = 0; i < Count + 20; i++)
	{
		auto entity = GetEntityObjByIndex(i);


		if (!entity || *(UINT_PTR*)entity == 0 || entity->EntityType != 1)
			continue;

		float distance = GetDistance(from, GetEntityTilePos(entity));

		if (distance < MaxDistance && distance < closest)
		{
			closest = distance;
			ret = entity;
		}
	}

	if (ret && strlen(ret->Name) > 0)
		return ret;

	return 0;
}

Tile2D RS::WorldToTilePos(const int32_t wX, const int32_t wY) {
	Tile2D tile;

	tile.x = (wX / 512);
	tile.y = (wY / 512);

	return tile;
}

Tile2D RS::GetEntityObjTile2d(EntityObj* entity)
{
	if(!entity)
	{
		return Tile2D(0, 0);
	}

	float* pos = entity->GetPos();

	return WorldToTilePos(pos[0], pos[2]);
}


std::string RS::ItemIdToString(uint32_t itemId)
{
	std::string itemIdS = std::to_string(itemId);

	if (itemList.count(itemIdS) > 0)
	{
		return itemList[itemIdS]["name"].get<std::string>();
	}

	return "Unknown";
}

int RS::ItemNameToId(std::string itemName)
{
	return 0;
}

UINT_PTR Static::GetFullEntityList()
{
	UINT_PTR Ptr2 = *(UINT_PTR*)((UINT_PTR)RS::GetGameContext() + Patterns.Offset_Ptr1);

	if (!Ptr2)
		return 0;

	uint64_t ptr3 = *(uint64_t*)(Ptr2 + 0x70);

	if (!ptr3)
		return 0;

	uint64_t camera = *(uint64_t*)(ptr3 + 0x8);

	if (!camera)
		return 0;

	uint64_t g_entityListFull = *(uint64_t*)(camera + Patterns.Offset_Camera);

	if (!g_entityListFull)
		return 0;

	return g_entityListFull;
}

StaticObjEX Static::GetClosestStaticObjectByName(const char* name, bool useSecondary, bool closest)
{
	std::set<uint64_t> static_entities;
	GetStaticEntities(&static_entities);

	Player player = RS::GetLocalPlayer();

	uint64_t closestEnt = 0;
	float closestDistance = 99999.0f;

	for (const uint64_t& entity_ptr : static_entities) {
		EntityType type = *reinterpret_cast<EntityType*>(entity_ptr + 0x40);



		if (type == EntityType::Object)
		{
			auto staticObj = (StaticObj1Wrapper*)entity_ptr;

			if (staticObj->Definition == 0) {
				continue;
			}

			Tile2D Pos = Tile2D(staticObj->TileX, staticObj->TileY);

			float curDistance = RS::GetDistance(player.GetTilePosition(), Pos);

			if (strcmp(name, staticObj->Definition->Name) == 0)
			{
				if (curDistance < closestDistance)
				{
					closestDistance = curDistance;
					closestEnt = entity_ptr;

					//printf("%p\n", entity_ptr);

					if (!closest)
						break;

				}
			}

		}

		else if (type == EntityType::Object2)
		{
			auto staticObj = (StaticObj2Wrapper*)entity_ptr;

			if (staticObj->Definition == 0) {
				continue;
			}

			Tile2D Pos = Tile2D(staticObj->TileX, staticObj->TileY);

			float curDistance = RS::GetDistance(player.GetTilePosition(), Pos);

			if (strcmp(name, staticObj->Definition->Name) == 0)
			{
				if (curDistance < closestDistance)
				{
					closestDistance = curDistance;
					closestEnt = entity_ptr;

					//printf("%p\n", entity_ptr);

					if (!closest)
						break;
				}
			}
		}
	}

	StaticObjEX returnObj = StaticObjEX();

	if (closestEnt)
	{
		returnObj.Type = *reinterpret_cast<EntityType*>(closestEnt + 0x40);

		if (returnObj.Type == EntityType::Object)
		{
			auto staticObj = (StaticObj1Wrapper*)closestEnt;
			returnObj.TileX = staticObj->TileX;
			returnObj.TileY = staticObj->TileY;
			returnObj.Definition = staticObj->Definition;

			if (useSecondary)
				returnObj.SecondId = staticObj->SecondaryId;
		}
		else if (returnObj.Type == EntityType::Object2)
		{
			auto staticObj = (StaticObj2Wrapper*)closestEnt;

			returnObj.TileX = staticObj->TileX;
			returnObj.TileY = staticObj->TileY;
			returnObj.Definition = staticObj->Definition;
		}
	}


	return returnObj;
}

StaticObjEX Static::GetClosestStaticObjectByNameWithOption(const char* name, const char * option, bool closest)
{
	std::set<uint64_t> static_entities;
	GetStaticEntities(&static_entities);

	Player player = RS::GetLocalPlayer();

	uint64_t closestEnt = 0;
	float closestDistance = 99999.0f;

	for (const uint64_t& entity_ptr : static_entities) {
		EntityType type = *reinterpret_cast<EntityType*>(entity_ptr + 0x40);

		if (type == EntityType::Object2)
		{
			auto staticObj = (StaticObj2Wrapper*)entity_ptr;

			if (staticObj->Definition == 0 || staticObj->IsHarvested) {
				continue;
			}

			Tile2D Pos = Tile2D(staticObj->TileX, staticObj->TileY);

			float curDistance = RS::GetDistance(player.GetTilePosition(), Pos);
			if (strcmp(name, staticObj->Definition->Name) == 0 && strcmp(option, staticObj->Definition->Op0) == 0)
			{
				if (curDistance < closestDistance)
				{
					closestDistance = curDistance;
					closestEnt = entity_ptr;

					if (!closest)
						break;
				}
			}
		}
	}

	StaticObjEX returnObj = StaticObjEX();

	if (closestEnt)
	{
		returnObj.Type = *reinterpret_cast<EntityType*>(closestEnt + 0x40);

		if (returnObj.Type == EntityType::Object)
		{
			auto staticObj = (StaticObj1Wrapper*)closestEnt;
			returnObj.TileX = staticObj->TileX;
			returnObj.TileY = staticObj->TileY;
			returnObj.Definition = staticObj->Definition;

		}
		else if (returnObj.Type == EntityType::Object2)
		{
			auto staticObj = (StaticObj2Wrapper*)closestEnt;
			returnObj.TileX = staticObj->TileX;
			returnObj.TileY = staticObj->TileY;
			returnObj.Definition = staticObj->Definition;
		}
	}


	return returnObj;
}

StaticObjEX Static::GetClosestStaticHarvestableObjectByNameWithOrigin(const char* name, const char * option, Tile2D origin)
{
	std::set<uint64_t> static_entities;
	GetStaticEntities(&static_entities);

	uint64_t closestEnt = 0;
	float closestDistance = 99999.0f;

	for (const uint64_t& entity_ptr : static_entities) {
		EntityType type = *reinterpret_cast<EntityType*>(entity_ptr + 0x40);

		if (type == EntityType::Object2)
		{
			auto staticObj = (StaticObj2Wrapper*)entity_ptr;

			if (staticObj->Definition == 0 || staticObj->IsHarvested) {
				continue;
			}

			Tile2D Pos = Tile2D(staticObj->TileX, staticObj->TileY);

			float curDistance = RS::GetDistance(origin, Pos);
			if (strcmp(name, staticObj->Definition->Name) == 0 && strcmp(option, staticObj->Definition->Op0) == 0)
			{
				if (curDistance < closestDistance)
				{
					closestDistance = curDistance;
					closestEnt = entity_ptr;
				}
			}
		}
	}

	StaticObjEX returnObj = StaticObjEX();

	if (closestEnt)
	{
		returnObj.Type = *reinterpret_cast<EntityType*>(closestEnt + 0x40);

		if (returnObj.Type == EntityType::Object)
		{
			auto staticObj = (StaticObj1Wrapper*)closestEnt;
			returnObj.TileX = staticObj->TileX;
			returnObj.TileY = staticObj->TileY;
			returnObj.Definition = staticObj->Definition;

		}
		else if (returnObj.Type == EntityType::Object2)
		{
			auto staticObj = (StaticObj2Wrapper*)closestEnt;
			returnObj.TileX = staticObj->TileX;
			returnObj.TileY = staticObj->TileY;
			returnObj.Definition = staticObj->Definition;
		}
	}


	return returnObj;
}

StaticObjEX Static::GetClosestAbyssEntrance()
{
	std::set<uint64_t> static_entities;
	GetStaticEntities(&static_entities);

	Player player = RS::GetLocalPlayer();

	uint64_t closestEnt = 0;
	float closestDistance = 99999.0f;

	for (const uint64_t& entity_ptr : static_entities) {
		EntityType type = *reinterpret_cast<EntityType*>(entity_ptr + 0x40);

		if (type == EntityType::Object)
		{
			auto staticObj = (StaticObj1Wrapper*)entity_ptr;

			if (staticObj->Definition == 0) {
				continue;
			}

			Tile2D Pos = Tile2D(staticObj->TileX, staticObj->TileY);

			float curDistance = RS::GetDistance(player.GetTilePosition(), Pos);

			if ((strcmp("Passage", staticObj->Definition->Name) == 0 && strcmp("Go-through", staticObj->Definition->Op0) == 0) ||
				(strcmp("Rock", staticObj->Definition->Name) == 0 && strcmp("Mine", staticObj->Definition->Op0) == 0) ||
				(strcmp("Gap", staticObj->Definition->Name) == 0 && strcmp("Squeeze-through", staticObj->Definition->Op0) == 0))
			{
				if (curDistance < closestDistance)
				{
					closestDistance = curDistance;
					closestEnt = entity_ptr;
				}
			}

		}
	}

	StaticObjEX returnObj = StaticObjEX();

	if (closestEnt)
	{
		returnObj.Type = *reinterpret_cast<EntityType*>(closestEnt + 0x40);

		if (returnObj.Type == EntityType::Object)
		{
			auto staticObj = (StaticObj1Wrapper*)closestEnt;
			returnObj.TileX = staticObj->TileX;
			returnObj.TileY = staticObj->TileY;
			returnObj.SecondId = staticObj->SecondaryId;
			returnObj.Definition = staticObj->Definition;

		}
	}


	return returnObj;
}

EntityObj* Static::GetEntityNpcByName(const char* name)
{
	std::set<uint64_t> static_entities;
	GetStaticEntities(&static_entities);

	float closestDistance = 99999.0f;

	for (const uint64_t& entity_ptr : static_entities) {
		EntityType type = *reinterpret_cast<EntityType*>(entity_ptr + 0x40);
		if (type == EntityType::NPC)
		{
			EntityObj* ent = (EntityObj*)entity_ptr;

			if (ent && strlen(ent->Name) > 0)
			{
				printf("ent name: %s\n", ent->Name);

				if (strcmp(ent->Name, name) == 0)
					return ent;
			}
		}
	}

	return 0;
}

StaticObjEX Static::GetCStaticObjectById(uint32_t id)
{
	std::set<uint64_t> static_entities;
	GetStaticEntities(&static_entities);

	uint64_t findingEnt = 0;

	for (const uint64_t& entity_ptr : static_entities) {
		EntityType type = *reinterpret_cast<EntityType*>(entity_ptr + 0x40);

		if (type == EntityType::Object)
		{
			auto staticObj = (StaticObj1Wrapper*)entity_ptr;

			if (staticObj->Definition == 0) {
				continue;
			}

			if (staticObj->Definition->Id == id)
			{
				findingEnt = entity_ptr;
				break;
			}

		}
		else if (type == EntityType::Object2)
		{
			auto staticObj = (StaticObj2Wrapper*)entity_ptr;

			if (staticObj->Definition == 0) {
				continue;
			}

			if (staticObj->Definition->Id == id)
			{
				findingEnt = entity_ptr;
				break;
			}

		}
		/*
		else if(type != EntityType::GroundItem && type != EntityType::Players)
		{
			printf("Type: %d  %p\n", type, entity_ptr);
		}
		*/
	}

	StaticObjEX returnObj = StaticObjEX();

	if (findingEnt)
	{
		returnObj.Type = *reinterpret_cast<EntityType*>(findingEnt + 0x40);

		if (returnObj.Type == EntityType::Object)
		{
			auto staticObj = (StaticObj1Wrapper*)findingEnt;
			returnObj.TileX = staticObj->TileX;
			returnObj.TileY = staticObj->TileY;
			returnObj.Definition = staticObj->Definition;

		}
		else if (returnObj.Type == EntityType::Object2)
		{
			auto staticObj = (StaticObj2Wrapper*)findingEnt;
			returnObj.TileX = staticObj->TileX;
			returnObj.TileY = staticObj->TileY;
			returnObj.Definition = staticObj->Definition;
		}
	}

	return returnObj;
}

void Static::GetStaticEntities(std::set<uint64_t>* out)
{
	GetStaticEntities_(GetFullEntityList(), out);
}

void Static::GetStaticEntities_(uint64_t curr, std::set<uint64_t>* out)
{
	uint64_t list = curr;


	uint64_t entity_ptr = ReadPtrOffset(list, Patterns.Offset_StaticEntityPtr);
	if (entity_ptr != 0) {
		out->insert(entity_ptr);
	}
	if (**reinterpret_cast<uint8_t**>(list + Patterns.Offset_StaticAnd20) & 0x20) {

		uint64_t start_ptr = ReadPtrOffset(list, Patterns.Offset_StaticEntityPtr - 0x68);
		uint64_t end_ptr = ReadPtrOffset(list, Patterns.Offset_StaticEntityPtr - 0x60);

		if (start_ptr != end_ptr) {
			size_t count = (end_ptr - start_ptr + 7) >> 3;


			for (size_t i = 0; i < count; ++i) {
				GetStaticEntities_(ReadPtrOffset(start_ptr, i * 8), out);
			}
		}
	}
}
