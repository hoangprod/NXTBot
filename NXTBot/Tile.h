#pragma once

struct Tile {

	static TileList* GetTileList();
	static FakeItemEX GetClosestItem(float Maxdistance);
	static FakeItemEX GetClosestItem(uint32_t ItemId, float MaxDistance);

	static Tile3D GetTile3D(TileInfo* tileinfo);

	static std::vector<TileInfo*> GetTilesWithLoot();
	static std::vector<FakeItem> GetLootsInTile(TileInfo* tileinfo);
	static std::vector<FakeItemEX> GetAllLootsNearby();
	static std::vector<FakeItemEX> GetAllLootsNearbyWithinRadius(Tile2D from, float MaxDistance);

	static std::vector<FakeItem*> SearchForLoot(char* ItemName);
	static std::vector<FakeItem*> SearchForLoot(uint32_t ItemId);

	static bool IsWithinAreaLoot(FakeItemEX origin, FakeItemEX point);
	static FakeItemEX GetBestLoot(std::vector<FakeItemEX> lootlist);
};

