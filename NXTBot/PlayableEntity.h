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
	bool BankUsingNPC(uint32_t Entity);
	bool Loot(FakeItemEX lootItem);
	bool StaticInteract(StaticObjEX obj);

	bool LootAllConfirm();
	bool QuickDropSlot1();
	bool ExitToLobby();

	bool WindClockWork();
	bool ReleaseClockWork();

	bool DepositActionNPC(uint32_t Entity);
	bool DepositAllThroughBank();
	bool ConfirmChat();
	bool DepositAll();
	bool BankInteractItem(int slot, int option);
	bool BankLoadPreset(int preset);

	bool TeleportToAbyssThroughMage();

	bool isMining();
	bool inCombat();
	bool isMoving();
	bool isNextTo(uint32_t Entity);

	void Test();
};
