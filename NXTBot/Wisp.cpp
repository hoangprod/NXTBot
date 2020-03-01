#include "pch.h"
#include "GameClasses.h"
#include "Patterns.h"
#include "Inventory.h"
#include "Game.h"
#include "PlayableEntity.h"
#include "Tile.h"
#include "Widgets.h"
#include "Wisp.h"

void Wisp::FSM()
{
	if (!player || !player->_base)
	{
		player = new Player(RS::GetLocalPlayer());
		origin = player->GetTilePosition();
	}

	// If I am moving around or targeting something, do not fuck with it
	if (player->isMoving() || (player->bTargeting() && player->CurrentTarget() != 4297))
	{
		printf("[+] Current moving or fighting\n");
		return;
	}

	if (player->inCombat())
	{
		// If already fighting, leave it alone.
		printf("[+] Current fighting\n");

		return;
	}

	auto attackingEnemies = RS::GetInCombatNPCwithMe();


	if (attackingEnemies.size() > 0)
	{
		printf("[+] Attacking Monster\n");

		// Attack the first monster in the list that is attacking us
		return Combat(attackingEnemies[0]);
	}

	if (Inventory::isInventoryFull())
	{
		printf("[+] Inventory is full, go bank\n");

		return Wisp::Banking();
	}

	auto lootsAvailable = Tile::GetAllLootsNearbyWithinRadius(origin, 10.0f);


	// If there are at least X items on the ground within 10 radius
	if (lootsAvailable.size() > 0)
	{
		printf("[+] Loots found and goign to loot\n");

		return Wisp::Looting(lootsAvailable);
	}

	EntityObj* EnemyMonsters = RS::GetMonsterWithinRadius(origin, 15.0f);


	// If there are a monster 15 tiles away from origin, fight it
	if (EnemyMonsters)
	{
		printf("[+] Fight monster\n");

		return Combat(EnemyMonsters);
	}

	// If not in combat, not being attacked, not full inventory and not have anything to loot
	printf("[+] Doing nothing, shouldn't really be here\n");

	// Do nothing
}

// Will add heal and other shits in here
void Wisp::Combat(EntityObj* Enemy)
{
	player->Attack(Enemy->EntityId);
}


//Widgets::GetWidgetUI(DEPOSIT_CHILD), Widgets::GetWidgetUI(CONVERSATION_CHILD)
void Wisp::Banking()
{
	// If not talking NOR banking, then go bank
	if (!Widgets::GetWidgetUI(CONVERSATION_CHILD) && !Widgets::GetWidgetUI(DEPOSIT_CHILD))
	{
		printf("[+] Going to talk to Wizard Myrtle\n");
		player->DepositActionNPC();
		return;
	}
	else
	{
		if (Widgets::GetWidgetUI(CONVERSATION_CHILD))
		{
			printf("[+] Clicking through chat option\n");
			player->ConfirmChat();
		}
		else if (Widgets::GetWidgetUI(DEPOSIT_CHILD))
		{
			printf("[+] Clicking deposit all items\n");
			player->DepositAll();
		}

		return;
	}
}

void Wisp::Looting(std::vector<FakeItemEX> loots)
{
	auto loot = Inventory::GetContainerObj(static_cast<uint32_t>(ContainerType::AreaLoot));

	if (loot)
		player->LootAllConfirm();
	else
		player->Loot(loots[0]);
}
