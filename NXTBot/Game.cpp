#include "pch.h"
#include "GameClasses.h"
#include "Patterns.h"
#include "ItemDef.h"
#include "Game.h"

extern UINT_PTR* g_GameContext;
extern Addr Patterns;
extern fn_GetLocalPlayer o_GetLocalPlayer;

GameContext* RS::GetGameContext()
{
	if(!g_GameContext)
		return nullptr;
	return (GameContext*)g_GameContext[1];
}

EntityPtr* RS::GetEntityPtr()
{
	auto GameContext = GetGameContext();

	if(!GameContext)
		return nullptr;
	
	return GameContext->EntityPtr;
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

	//UINT_PTR result = PlayerListWrapper[2056] - PlayerListWrapper[2055] >> 2;
	UINT_PTR result = ReadPtrOffset(PlayerListWrapper, Patterns.Offset_EntityArrayList + 8) - ReadPtrOffset(PlayerListWrapper, Patterns.Offset_EntityArrayList) >> 2;

	return result;
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

void RS::LoopEntityList()
{
	auto Count = GetEntityCount();

	if (!Count)
		return;

	for (int i = 0; i < Count; i++)
	{
		auto entity = GetEntityObjByIndex(i);

		if (!entity || *(UINT_PTR*)entity == 0)
			continue;

		if (strlen(entity->Name) > 0)
		{
			printf("[+] %s with ID %d - %p\n", entity->Name, entity->EntityId, entity);
			auto npcdef = NpcDef(entity);
			npcdef.printOptions();
		}
	}
}

void RS::LoopPlayerEntityList()
{
	auto Count = GetPlayerEntityCount();

	if (!Count)
		return;

	for (int i = 0; i < Count + 10; i++)
	{
		auto entity = GetPlayerObjByIndex(i);

		if (!entity || *(UINT_PTR*)entity == 0)
			continue;

		if (strlen(entity->Name) > 0)
		{
			printf("[%d] %s with ID %d\n", i, entity->Name, entity->EntityId);

		}
	}
}

EntityObj* RS::GetClosestPlayer()
{
	auto Count = GetPlayerEntityCount();
	auto localPlayer = GetLocalPlayer();
	EntityObj* ret = 0;
	float closest = 99999.0f;
	if (!Count)
		return 0;
	auto localPlayerTile = GetLocalPlayerTilePos();
	for (int i = 0; i < Count + 10; i++)
	{
		auto entity = GetPlayerObjByIndex(i);

		if (!entity || *(UINT_PTR*)entity == 0)
			continue;

		if (strlen(entity->Name) > 0 && entity->EntityId != localPlayer->EntityId)
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
		printf("%s with ID %d and distance %f\n", ret->Name, ret->EntityId, closest);

	return ret;
}

Tile2D RS::WorldToTilePos(const int32_t wX, const int32_t wY) {

	Tile2D tile;

	tile.x = (wX / 512) & 0xFFF;
	tile.y = (wY / 512) & 0xFFF;

	return tile;
}
