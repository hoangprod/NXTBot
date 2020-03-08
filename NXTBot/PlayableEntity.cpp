#include "pch.h"
#include "GameClasses.h"
#include "Patterns.h"
#include "Inventory.h"
#include "Game.h"
#include "Helper.h"
#include "PlayableEntity.h"

extern UINT_PTR* g_GameContext;
extern UINT_PTR g_Module;
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
	uint8_t data[100] = { 0 };

	*reinterpret_cast<int*>(&data[0x50]) = Entity;
	*reinterpret_cast<int*>(&data[0x54]) = 0;
	*reinterpret_cast<int*>(&data[0x58]) = 0;

	uint64_t func_ptr = g_Module + 0x094A90;

	if (!func_ptr)
		return false;


	dataStruct dt;
	dt.dataPtr = data;

	typedef void(__cdecl* _Attack)(uint64_t* _this, void* dataPtr);
	reinterpret_cast<_Attack>(func_ptr)(g_GameContext, &dt);

	return true;
}

bool Player::Talk(uint32_t Entity)
{
	return false;
}

bool Player::BankUsingNPC(uint32_t targetEntity)
{
	auto ent = RS::GetEntityObjectByEntityId(targetEntity);

	if (!ent)
		return false;

	uint8_t data[100] = { 0 };

	*reinterpret_cast<int*>(&data[0x50]) = ent->EntityId;
	*reinterpret_cast<int*>(&data[0x54]) = 0;
	*reinterpret_cast<int*>(&data[0x58]) = 0;

	uint64_t func_ptr = g_Module + 0x94a70;

	if (!func_ptr)
		return false;


	dataStruct dt;
	dt.dataPtr = data;

	typedef void(__cdecl* _Loot)(uint64_t* _this, void* dataPtr);
	reinterpret_cast<_Loot>(func_ptr)(g_GameContext, &dt);

	return true;
}

bool Player::Loot(FakeItemEX ObjectId)
{
	uint8_t data[100] = { 0 };

	*reinterpret_cast<int*>(&data[0x50]) = ObjectId.ItemId;
	*reinterpret_cast<int*>(&data[0x54]) = ObjectId.Pos.x;
	*reinterpret_cast<int*>(&data[0x58]) = ObjectId.Pos.y;

	uint64_t func_ptr = g_Module + 0x94b90;

	if (!func_ptr)
		return false;


	dataStruct dt;
	dt.dataPtr = data;

	typedef void(__cdecl* _Loot)(uint64_t* _this, void* dataPtr);
	reinterpret_cast<_Loot>(func_ptr)(g_GameContext, &dt);

	return true;
}

bool Player::Mine(StaticObj obj)
{
	uint8_t data[100] = { 0 };

	*reinterpret_cast<int*>(&data[0x50]) = obj.ItemId;
	*reinterpret_cast<int*>(&data[0x54]) = obj.Pos.x;
	*reinterpret_cast<int*>(&data[0x58]) = obj.Pos.y;

	uint64_t func_ptr = g_Module + 0x94990;

	if (!func_ptr)
		return false;


	dataStruct dt;
	dt.dataPtr = data;

	typedef void(__cdecl* _Loot)(uint64_t* _this, void* dataPtr);
	reinterpret_cast<_Loot>(func_ptr)(g_GameContext, &dt);

	return true;
}

bool Player::LootAllConfirm()
{
	uint8_t data[100] = { 0 };

	*reinterpret_cast<int*>(&data[0x50]) = 1;
	*reinterpret_cast<int*>(&data[0x54]) = -1;
	*reinterpret_cast<int*>(&data[0x58]) = 0x6560015;

	uint64_t func_ptr = g_Module + 0x94940;

	if (!func_ptr)
		return false;


	dataStruct dt;
	dt.dataPtr = data;

	typedef void(__cdecl* _WidgetLootAll)(uint64_t* _this, void* dataPtr);
	reinterpret_cast<_WidgetLootAll>(func_ptr)(g_GameContext, &dt);

	return true;
}

bool Player::QuickDropSlot1()
{
	uint8_t data[100] = { 0 };

	*reinterpret_cast<int*>(&data[0x50]) = 8;
	*reinterpret_cast<int*>(&data[0x54]) = -1;
	*reinterpret_cast<int*>(&data[0x58]) = 93716544;

	uint64_t func_ptr = g_Module + 0x94940;

	if (!func_ptr)
		return false;


	dataStruct dt;
	dt.dataPtr = data;

	typedef void(__cdecl* _WidgetLootAll)(uint64_t* _this, void* dataPtr);
	reinterpret_cast<_WidgetLootAll>(func_ptr)(g_GameContext, &dt);

	return true;
}

bool Player::DepositActionNPC(uint32_t Entity)
{
	uint8_t data[100] = { 0 };

	*reinterpret_cast<int*>(&data[0x50]) = Entity;

	uint64_t func_ptr = g_Module + 0x94ab0;

	if (!func_ptr)
		return false;


	dataStruct dt;
	dt.dataPtr = data;

	typedef uintptr_t(__cdecl* _InteractionDepositNPC)(uint64_t* _this, void* dataPtr);
	auto test = reinterpret_cast<_InteractionDepositNPC>(func_ptr)(g_GameContext, &dt);

	return true;
}

bool Player::DepositAllThroughBank()
{
	uint8_t data[100] = { 0 };

	*reinterpret_cast<int*>(&data[0x50]) = 1;
	*reinterpret_cast<int*>(&data[0x54]) = -1;
	*reinterpret_cast<int*>(&data[0x58]) = 0x2050025;

	uint64_t func_ptr = g_Module + 0x94940;

	if (!func_ptr)
		return false;


	dataStruct dt;
	dt.dataPtr = data;

	typedef void(__cdecl* _WidgetLootAll)(uint64_t* _this, void* dataPtr);
	reinterpret_cast<_WidgetLootAll>(func_ptr)(g_GameContext, &dt);

	return true;
}

bool Player::ConfirmChat()
{
	uint8_t data[100] = { 0 };

	*reinterpret_cast<int*>(&data[0x50]) = 0;
	*reinterpret_cast<int*>(&data[0x54]) = -1;
	*reinterpret_cast<int*>(&data[0x58]) = 0x4A0000F; // Hardcoded

	uint64_t func_ptr = g_Module + 0x94930;

	if (!func_ptr)
		return false;


	dataStruct dt;
	dt.dataPtr = data;

	typedef uintptr_t(__cdecl* _InteractionDepositNPC)(uint64_t* _this, void* dataPtr);
	reinterpret_cast<_InteractionDepositNPC>(func_ptr)(g_GameContext, &dt);

	return true;
}

bool Player::DepositAll()
{
	uint8_t data[100] = { 0 };

	*reinterpret_cast<int*>(&data[0x50]) = 1;
	*reinterpret_cast<int*>(&data[0x54]) = -1;
	*reinterpret_cast<int*>(&data[0x58]) = 0xb0005; // Hardcoded

	uint64_t func_ptr = g_Module + 0x94940;

	if (!func_ptr)
		return false;


	dataStruct dt;
	dt.dataPtr = data;

	typedef uintptr_t(__cdecl* _BankDepositAll)(uint64_t* _this, void* dataPtr);
	reinterpret_cast<_BankDepositAll>(func_ptr)(g_GameContext, &dt);
	return true;
}

bool Player::isMining()
{
	if (!_base)
		return 0;

	return *(int*)((UINT_PTR)_base + Patterns.Offset_TargetedEntity - 4) == 3;
}

bool Player::inCombat()
{
	if (MovingState() == -1 && bTargeting() && CurrentTarget() != -1 && RS::GetInCombatNPCwithMe().size() > 0)
		return true;
	return false;
}

bool Player::isMoving()
{
	auto movingState = MovingState();

	if (movingState == 1 || movingState == 2)
		return true;

	return false;
}

bool Player::isNextTo(uint32_t Entity)
{
	return false;
}

void Player::Test()
{
	printf("===== Performing test =====\n");
	printf("Moving state: %d\n", MovingState());
	printf("Id: %d   Type: %d\n", Id(), GetType());
	printf("Targeting %d   CurrentTarget: %d\n", bTargeting(), CurrentTarget());
	printf("Current Ani: %d  SpotAni: %d\n", CurrentAnimation(), CurrentSpotAnimation());
	printf("Tile Pos: %d %d\n", GetTilePosition().x, GetTilePosition().y);
	printf("GetName: %s\n", GetName().data());
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

bool Entity::bTargeting()
{
	if (!_base)
		return 0;

	return *(int*)((UINT_PTR)_base + Patterns.Offset_IsCurrentlyTargeting) == 1;
}

int Entity::CurrentTarget()
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
