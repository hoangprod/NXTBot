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

	*reinterpret_cast<int*>(&data[0x5c]) = tile.x;
	*reinterpret_cast<int*>(&data[0x60]) = tile.y;

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

bool Player::MoveRandom(Tile2D tile, int x_delta, int y_delta)
{
	int RandomX = rand() % x_delta + tile.x;
	int RandomY = rand() % y_delta + tile.y;

	return this->Move(Tile2D(RandomX, RandomY));
}

bool Player::Attack(uint32_t Entity)
{
	uint8_t data[100] = { 0 };

	*reinterpret_cast<int*>(&data[0x58]) = Entity;
	*reinterpret_cast<int*>(&data[0x5c]) = 0;
	*reinterpret_cast<int*>(&data[0x60]) = 0;

	uint64_t func_ptr = g_Module + 0xffa90;

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


bool Player::Loot(FakeItemEX ObjectId)
{
	uint8_t data[100] = { 0 };

	*reinterpret_cast<int*>(&data[0x58]) = ObjectId.ItemId;
	*reinterpret_cast<int*>(&data[0x5c]) = ObjectId.Pos.x;
	*reinterpret_cast<int*>(&data[0x60]) = ObjectId.Pos.y;

	uint64_t func_ptr = g_Module + 0xffb90;

	if (!func_ptr)
		return false;


	dataStruct dt;
	dt.dataPtr = data;

	typedef void(__cdecl* _Loot)(uint64_t* _this, void* dataPtr);
	reinterpret_cast<_Loot>(func_ptr)(g_GameContext, &dt);

	return true;
}



bool Player::StaticInteractManual(uint32_t id, uint32_t x, uint32_t y)
{
	uint8_t data[100] = { 0 };

	*reinterpret_cast<int*>(&data[0x58]) = id;
	*reinterpret_cast<int*>(&data[0x5c]) = x;
	*reinterpret_cast<int*>(&data[0x60]) = y;

	uint64_t func_ptr = g_Module + 0xff990;

	if (!func_ptr)
		return false;


	dataStruct dt;
	dt.dataPtr = data;

	typedef void(__cdecl* _Loot)(uint64_t* _this, void* dataPtr);
	reinterpret_cast<_Loot>(func_ptr)(g_GameContext, &dt);

	return true;
}

void Player::LootAllConfirm()
{
	uint8_t data[100] = { 0 };

	*reinterpret_cast<int*>(&data[0x58]) = 1;
	*reinterpret_cast<int*>(&data[0x5c]) = -1;
	*reinterpret_cast<int*>(&data[0x60]) = 0x6560015;

	uint64_t** handler = (uint64_t**)Patterns.Addr_InventoryActionHandler;
	if (!handler)
		return;

	uint64_t* handler_vtable = *handler;

	if (!handler_vtable)
		return;

	uint64_t func_ptr = handler_vtable[2];

	if (!func_ptr)
		return;


	dataStruct dt;
	dt.dataPtr = data;

	typedef void(__cdecl* _WidgetLootAll)(uint64_t* _this, void* dataPtr);
	return reinterpret_cast<_WidgetLootAll>(func_ptr)(g_GameContext, &dt);
}


bool Player::QuickDropSlot1()
{
	uint8_t data[100] = { 0 };

	*reinterpret_cast<int*>(&data[0x58]) = 8;
	*reinterpret_cast<int*>(&data[0x5c]) = -1;
	*reinterpret_cast<int*>(&data[0x60]) = 93716544;

	uint64_t** handler = (uint64_t**)Patterns.Addr_InventoryActionHandler;
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

	typedef void(__cdecl* _WidgetLootAll)(uint64_t* _this, void* dataPtr);
	reinterpret_cast<_WidgetLootAll>(func_ptr)(g_GameContext, &dt);

	return true;
}

bool Player::ExitToLobby()
{
	uint8_t data[100] = { 0 };

	*reinterpret_cast<int*>(&data[0x58]) = 1;
	*reinterpret_cast<int*>(&data[0x5c]) = -1;
	*reinterpret_cast<int*>(&data[0x60]) = 93913154;

	uint64_t** handler = (uint64_t**)Patterns.Addr_InventoryActionHandler;
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

	typedef void(__cdecl* _WidgetLootAll)(uint64_t* _this, void* dataPtr);
	reinterpret_cast<_WidgetLootAll>(func_ptr)(g_GameContext, &dt);

	return true;
}

bool Player::WindClockWork()
{
	uint8_t data[100] = { 0 };

	*reinterpret_cast<int*>(&data[0x58]) = 1;
	*reinterpret_cast<int*>(&data[0x5c]) = -1;
	*reinterpret_cast<int*>(&data[0x60]) = 93716544;

	uint64_t** handler = (uint64_t**)Patterns.Addr_InventoryActionHandler;
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

	typedef void(__cdecl* _WidgetLootAll)(uint64_t* _this, void* dataPtr);
	reinterpret_cast<_WidgetLootAll>(func_ptr)(g_GameContext, &dt);

	return true;
}

bool Player::ReleaseClockWork()
{
	uint8_t data[100] = { 0 };

	*reinterpret_cast<int*>(&data[0x58]) = 7;
	*reinterpret_cast<int*>(&data[0x5c]) = -1;
	*reinterpret_cast<int*>(&data[0x60]) = 93716557;

	uint64_t** handler = (uint64_t**)Patterns.Addr_InventoryActionHandler;
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

	typedef void(__cdecl* _WidgetLootAll)(uint64_t* _this, void* dataPtr);
	reinterpret_cast<_WidgetLootAll>(func_ptr)(g_GameContext, &dt);

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
	auto DestFlag = RS::GetDestinationFlag();
	
	if (DestFlag->X == -1 || DestFlag->Y == -1)
		return false;

	return true;
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
	case 18041:
		return 0; // Stand
		break;
	case 18043:
		return 1; // Walk
		break;
	case 18042:
		return 2; // Run
		break;
	case 18019:
		return 0; // Stand
		break;
	case 18021:
		return 1; // Walk
		break;
	case 18020:
		return 2; // Run
		break;
	case 15069:
		return 0; // Stand
		break;
	case 15073:
		return 1; // Walk
		break;
	case 15070:
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

uint32_t Entity::GetElapsedSecondSinceLastAction()
{
	if (!_base)
		return 0;

	auto entity = (EntityObj*)_base;

	return entity->ElapsedLastAction;
}

bool Entity::IsInAnimation()
{
	if (!_base)
		return 0;

	auto entity = (EntityObj*)_base;

	return (entity->DefautAni != entity->CurrentAni);
}

int Entity::CurrentUsefulAni()
{
	if (!_base)
		return 0;

	auto entity = (EntityObj*)_base;

	return entity->UsefulAni;
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
