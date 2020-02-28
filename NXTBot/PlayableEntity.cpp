#include "pch.h"
#include "GameClasses.h"
#include "Patterns.h"
#include "Inventory.h"
#include "Game.h"
#include "PlayableEntity.h"

extern UINT_PTR* g_GameContext;
extern Addr Patterns;

bool Player::Move(Tile2D tile)
{
	uint8_t data[100] = { 0 };

	*reinterpret_cast<int*>(&data[0x54]) = tile.x;
	*reinterpret_cast<int*>(&data[0x58]) = tile.y;

	uint64_t** handler = (uint64_t**)Patterns.Addr_MoveActionHandler;

	if (!handler)
		return false;

	uint64_t* handler_vtable = *handler;

	if (!handler_vtable)
		return false;

	uint64_t func_ptr = handler_vtable[2];

	if (!func_ptr)
		return false;


	dataStruct dt;
	dt.dataPtr = data;

	typedef void(__cdecl* _MoveTo)(uint64_t** _this, void* dataPtr);
	reinterpret_cast<_MoveTo>(func_ptr)(handler, &dt);

	return true;
}

bool Player::Attack(uint32_t Entity)
{
	return false;
}

bool Player::Talk(uint32_t Entity)
{
	return false;
}

bool Player::Bank(uint32_t Entity)
{
	return false;
}

bool Player::Loot(uint32_t ObjectId)
{
	return false;
}

int Entity::MovingState()
{
	uint32_t curAni = CurrentAnimation();

	switch (curAni)
	{
	case 18038:
		return 0; // Stand
		break;
	case 18040:
		return 1; // Walk
		break;
	case 18039:
		return 2; // Run
		break;
	default:
		return -1; // Unknown
		break;
	}
}

int Entity::NPCCurHealth()
{
	if (!_base)
		return 0;

	uint32_t health = 0, i = 0;

	uint32_t maxHealth = 0;

	while (!maxHealth)
	{
		maxHealth = *(uint32_t*)((UINT_PTR)_base + Patterns.Offset_Health + 4 * (INT64)i + 0x18);

		if (maxHealth)
			break;

		i++;
	}

	health = *(uint32_t*)((UINT_PTR)_base + Patterns.Offset_Health + 4 * (INT64)i);


	return health;
}

int Entity::NPCMaxHealth()
{
	if (!_base)
		return 0;

	uint32_t maxHealth = 0, i = 0;

	while (!maxHealth)
	{
		maxHealth = *(uint32_t*)((UINT_PTR)_base + Patterns.Offset_Health + 4 * i + 0x18);
		i++;
	}

	return maxHealth;
}

uint32_t Entity::Id()
{
	if (!_base)
		return 0;

	auto entity = (EntityObj*)_base;

	return entity->EntityId;
}

uint32_t Entity::GetType()
{
	if (!_base)
		return 0;

	auto entity = (EntityObj*)_base;

	return entity->EntityType;
}

uint32_t Entity::bTargeting()
{
	if (!_base)
		return 0;

	return *(int*)((UINT_PTR)_base + Patterns.Offset_IsCurrentlyTargeting);
}

uint32_t Entity::CurrentTarget()
{
	if (!_base)
		return 0;

	return *(int*)((UINT_PTR)_base + Patterns.Offset_TargetedEntity);
}

uint32_t Entity::CurrentAnimation()
{
	if (!_base)
		return 0;

	return *(int*)((UINT_PTR)_base + Patterns.Offset_AnimationId);
}

uint32_t Entity::CurrentSpotAnimation()
{
	if (!_base)
		return 0;

	return *(int*)((UINT_PTR)_base + Patterns.Offset_AnimationClass + Patterns.Offset_AnimationSpotId);
}

float* Entity::GetPosition()
{
	if (!_base)
		return 0;

	auto entity = (EntityObj*)_base;

	return (float*)entity->GetPos();
}

Tile2D Entity::GetTilePosition()
{
	auto worldPos = GetPosition();

	if (!worldPos)
		return Tile2D();

	return RS::WorldToTilePos(worldPos[0], worldPos[2]);
}

std::string Entity::GetName()
{
	if (!_base)
		return std::string("[InvalidPtr]");

	auto entity = (EntityObj*)_base;

	if(strlen(entity->Name) > 0)
		return std::string(entity->Name);
	return std::string("[Unknown]");
}
