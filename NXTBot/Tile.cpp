#include "pch.h"
#include "GameClasses.h"
#include "Patterns.h"
#include "Inventory.h"
#include "Game.h"
#include "PlayableEntity.h"
#include "Tile.h"

extern Addr Patterns;

TileList* Tile::GetTileList()
{
	GameContext* context = RS::GetGameContext();

	if (!context)
		return nullptr;

	return *(TileList**)((UINT_PTR)context + Patterns.Offset_TileList);
}

FakeItemEX Tile::GetClosestItem(float Maxdistance)
{
	auto playerPos = RS::GetLocalPlayerTilePos();

	auto lootlist = Tile::GetAllLootsNearby();

	int index = -1;
	float closest = 99999.0f;

	for (int i = 0; i < lootlist.size(); i++)
	{
		auto loot = lootlist[i];

		if (loot.Pos.plane == 0)
		{
			float distance = RS::GetDistance(Tile2D(loot.Pos.x, loot.Pos.y), playerPos);

			if (distance < Maxdistance && distance < closest)
			{
				closest = distance;
				index = i;
			}
		}
	}

	if (index == -1)
		return FakeItemEX();

	else return lootlist[index];
}

FakeItemEX Tile::GetClosestItem(uint32_t ItemId, float Maxdistance)
{
	auto playerPos = RS::GetLocalPlayerTilePos();

	auto lootlist = Tile::GetAllLootsNearby();

	int index = -1;
	float closest = 99999.0f;

	for (int i = 0; i < lootlist.size(); i++)
	{
		auto loot = lootlist[i];

		if (loot.ItemId == ItemId && loot.Pos.plane == 0)
		{
			float distance = RS::GetDistance(Tile2D(loot.Pos.x, loot.Pos.y), playerPos);

			if (distance < Maxdistance && distance < closest)
			{
				closest = distance;
				index = i;
			}
		}
	}

	if (index == -1)
		return FakeItemEX();

	else return lootlist[index];
}

Tile3D Tile::GetTile3D(TileInfo* tileinfo)
{
	if (!tileinfo)
		return Tile3D();

	return Tile3D(tileinfo->Plane, tileinfo->TileX, tileinfo->TileY);
}

std::vector<TileInfo*> Tile::GetTilesWithLoot()
{
	TileList* tilelist = GetTileList();

	std::vector<TileInfo*> Result;

	if (tilelist->LootTileCount > 0 && tilelist->LastTile)
		Result.push_back(tilelist->LastTile);

	// if tile is not valid or there is no loot or next loot == last loot meaning no more loot, we are donezo
	if (!tilelist || tilelist->LootTileCount <= 0 || tilelist->NextTile == tilelist->LastTile) 
		return Result;

	TileInfo* currentTile = tilelist->NextTile;
	TileInfo* finalTile = tilelist->LastTile;


	if (currentTile != tilelist->LastTile)
	{
		do
		{
			Result.push_back(currentTile);

			UINT_PTR* nextTile = *(UINT_PTR**)currentTile;

			if (*(UINT_PTR*)currentTile)
			{
				currentTile = *(TileInfo**)currentTile;

				for (UINT_PTR* i = (UINT_PTR*)nextTile[1]; i; i = (UINT_PTR*)i[1])
					currentTile = (TileInfo*)i;
			}
			else
			{
				UINT_PTR* v13 = 0; // I am just following pseudo at this point i dont know what is going on dont ask me

				UINT_PTR* lastTile = (UINT_PTR*)((UINT_PTR*)currentTile)[2];

				if ((UINT_PTR *)currentTile == *(UINT_PTR**)lastTile)
				{
					do
					{
						currentTile = (TileInfo*)lastTile;
						lastTile = (UINT_PTR*)lastTile[2];

					} while ((UINT_PTR*)currentTile == *(UINT_PTR**)lastTile);

					v13 = *(UINT_PTR**)currentTile;
				}
				if (v13 != lastTile)
					currentTile = (TileInfo*)lastTile;
			}
	
		} while (currentTile != finalTile); // while v7 != v8
	}
	return Result;
}

std::vector<FakeItem> Tile::GetLootsInTile(TileInfo* tileinfo)
{
	LootInfo* lootInfo = tileinfo->LootInfo;

	FakeItem* Start = lootInfo->LootArray;
	UINT_PTR* End = (UINT_PTR*)lootInfo->LastLootPtr;

	std::vector<FakeItem> Result;

	do
	{
		Result.push_back(*Start);

		Start = (FakeItem*)((UINT_PTR)Start + 0x90);
	} 
	while ((UINT_PTR*)Start < End);

	return Result;
}

std::vector<FakeItemEX> Tile::GetAllLootsNearby()
{
	std::vector<FakeItemEX> Result;

	std::vector<TileInfo*> TileLootList = Tile::GetTilesWithLoot();

	for (auto lootlist : TileLootList)
	{
		auto fakeitemlist = GetLootsInTile(lootlist);

		for (auto fakeitem : fakeitemlist)
		{

			auto lootName = RS::ItemIdToString(fakeitem.ItemId);

			// If the item is not arrow nor bolt
			if(lootName.find("arrow") == std::string::npos && lootName.find("bolt") == std::string::npos)
				Result.push_back(FakeItemEX(fakeitem.ItemId, fakeitem.ItemQuantity, lootlist->TileX, lootlist->TileY, lootlist->Plane));
		}
	}

	return Result;
}

std::vector<FakeItemEX> Tile::GetAllLootsNearbyWithinRadius(Tile2D from, float MaxDistance)
{
	std::vector<FakeItemEX> Result;

	std::vector<TileInfo*> TileLootList = Tile::GetTilesWithLoot();

	for (auto lootlist : TileLootList)
	{
		if (RS::GetDistance(Tile2D(lootlist->TileX, lootlist->TileY), from) < MaxDistance)
		{
			auto fakeitemlist = GetLootsInTile(lootlist);

			for (auto fakeitem : fakeitemlist)
			{
				Result.push_back(FakeItemEX(fakeitem.ItemId, fakeitem.ItemQuantity, lootlist->TileX, lootlist->TileY, lootlist->Plane));
			}
		}

	}

	return Result;
}

std::vector<FakeItem*> Tile::SearchForLoot(char* ItemName)
{
	return std::vector<FakeItem*>();
}

std::vector<FakeItem*> Tile::SearchForLoot(uint32_t ItemId)
{
	return std::vector<FakeItem*>();
}

bool Tile::IsWithinAreaLoot(FakeItemEX origin, FakeItemEX point)
{
	if (RS::GetDistance(Tile2D(origin.Pos.x, origin.Pos.y), Tile2D(point.Pos.x, point.Pos.y)) <= 3.0f)
		return true;
	return false;
}

FakeItemEX Tile::GetBestLoot(std::vector<FakeItemEX> lootlist)
{
	int mostLoot = 0;
	int mostLootIndex = -1;

	for (int i = 0; i < lootlist.size(); i++)
	{
		int curLoot = 0;

		for (auto loot : lootlist)
		{
			if (IsWithinAreaLoot(lootlist[i], loot))
				curLoot++;
		}

		if (curLoot > mostLoot)
		{
			mostLoot = curLoot;
			mostLootIndex = i;
		}
	}

	if (mostLootIndex == -1)
		return FakeItemEX();
	else
		return lootlist[mostLootIndex];
}


