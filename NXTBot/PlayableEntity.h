#pragma once

class Entity
{
public:
	Entity() { _base = 0; };
	Entity(UINT_PTR* base) { _base = base; };
	Entity(EntityObj* base) { _base = (UINT_PTR*)base; };

	int MovingState();
	int NPCCurHealth();
	int NPCMaxHealth();

	uint32_t Id();
	uint32_t GetType();
	uint32_t GetElapsedSecondSinceLastAction();
	int CurrentTarget();
	uint32_t CurrentAnimation();
	uint32_t CurrentSpotAnimation();

	bool IsInAnimation();
	bool bTargeting();
	float* GetPosition();
	Tile2D GetTilePosition();
	std::string GetName();

	UINT_PTR* _base;
};


class Player : public Entity
{
public:
	Player(UINT_PTR* player) { _base = player; };
	Player(EntityObj* player) { _base = (UINT_PTR*)player; };

	bool Move(Tile2D tile);
	bool Attack(uint32_t Entity);
	bool Talk(uint32_t Entity);
	bool Loot(FakeItemEX lootItem);
	bool StaticInteract(StaticObjEX obj);
	bool StaticInteract2(StaticObjEX obj);
	bool StaticInteractManual(uint32_t id, uint32_t x, uint32_t y);

	bool LootAllConfirm();
	bool InteractWithEquipment(int slot, int option, int64_t param3 = 0x5B8000F);
	bool QuickDropSlot1();
	bool ExitToLobby();

	bool WindClockWork();
	bool ReleaseClockWork();


	bool DepositAllThroughBank();
	bool DepositAll();


	bool isMining();
	bool inCombat();
	bool isMoving();
	bool isNextTo(uint32_t Entity);

	void Test();
};
