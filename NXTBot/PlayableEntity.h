#pragma once

class Entity
{
public:
	Entity(UINT_PTR* base) { _base = base; };

	int MovingState();
	int NPCCurHealth();
	int NPCMaxHealth();

	uint32_t Id();
	uint32_t GetType();
	uint32_t bTargeting();
	uint32_t CurrentTarget();
	uint32_t CurrentAnimation();
	uint32_t CurrentSpotAnimation();

	float* GetPosition();
	Tile2D GetTilePosition();
	std::string GetName();

	UINT_PTR* _base;
};


class Player : Entity
{
public:
	bool Move(Tile2D tile);
	bool Attack(uint32_t Entity);
	bool Talk(uint32_t Entity);
	bool Bank(uint32_t Entity);
	bool Loot(uint32_t ObjectId);
};
