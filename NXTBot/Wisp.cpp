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
	if (!player || !player->_base || !MyrtleId)
	{
		player = new Player(RS::GetLocalPlayer());
		origin = player->GetTilePosition();
	}

	if (!MyrtleId)
	{
		auto Myrtle = RS::GetEntityNPCByName("Wizard Myrtle");

		if (!Myrtle)
		{
			printf("[!] Cannot find Myrtle, myrtle is too far maybe?\n");
			return;
		}

		MyrtleId = Myrtle->EntityId;

		printf("[+] Found Myrtle with id %d\n", MyrtleId);
	}

	// If I am moving around or targeting something, do not fuck with it
	if (player->isMoving() || (player->bTargeting() && player->CurrentTarget() != MyrtleId))
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
	if (lootsAvailable.size() > 4)
	{
		printf("[+] Loots found and going to loot\n");

		return Wisp::Looting(lootsAvailable);
	}

	EntityObj* EnemyMonsters = RS::GetMonsterWithinRadiusWithName("Spellwisp", origin, 15.0f);


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
	if (!Widgets::GetWidgetUI(CONVERSATION_WIDGET) && !Widgets::GetWidgetUI(DEPOSIT_WIDGET))
	{
		printf("[+] Going to talk to Wizard Myrtle\n");

		player->DepositActionNPC(MyrtleId);
		return;
	}
	else
	{
		if (Widgets::GetWidgetUI(CONVERSATION_WIDGET))
		{
			printf("[+] Clicking through chat option\n");
			player->ConfirmChat();
		}
		else if (Widgets::GetWidgetUI(DEPOSIT_WIDGET))
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

void GeneralCombat::FSM()
{
	if (!player || !player->_base)
	{
		monsterTargetName = RS::GetClosestMonster()->Name;
		player = new Player(RS::GetLocalPlayer());
		origin = player->GetTilePosition();

		printf("[+] Player %s will be targeting %s from tile position (%d, %d)\n", player->GetName().data(), monsterTargetName.data(), origin.x, origin.y);
	}

	// If I am moving around or targeting something, do not fuck with it
	if (player->isMoving() || (player->bTargeting() || player->CurrentTarget() > 0))
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

	EntityObj* EnemyMonsters = RS::GetMonsterWithinRadiusWithName(monsterTargetName.data(), origin, 10.0f);


	// If there are a monster 15 tiles away from origin, fight it
	if (EnemyMonsters)
	{
		printf("[+] Fight monster\n");

		return Combat(EnemyMonsters);
	}

	// If not in combat, not being attacked, not full inventory and not have anything to loot
	printf("[+] Doing nothing, shouldn't really be here unless no monster around\n");
}

void GeneralCombat::Combat(EntityObj* Enemy)
{
	player->Attack(Enemy->EntityId);
}




// RABBIT =========================================




void Rabbit::FSM()
{
	if (!player || !player->_base)
	{
		player = new Player(RS::GetLocalPlayer());
		origin = player->GetTilePosition();
	}

	// If I am moving around or targeting something, do not fuck with it
	if (player->isMoving() || (player->bTargeting() && player->CurrentTarget() != 25688)) // Hardcode bank id
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

		return Rabbit::Banking();
	}

	auto lootsAvailable = Tile::GetAllLootsNearbyWithinRadius(origin, 7.0f);


	// If there are at least X items on the ground within 10 radius
	if (lootsAvailable.size() > 9)
	{
		printf("[+] Loots found and going to loot\n");

		return Rabbit::Looting(lootsAvailable);
	}

	EntityObj* EnemyMonsters = RS::GetMonsterWithinRadiusWithName("Rabbit", origin, 7.0f);


	// If there are a monster 15 tiles away from origin, fight it
	if (EnemyMonsters)
	{
		printf("[+] Fight monster\n");

		return Rabbit::Combat(EnemyMonsters);
	}

	// If not in combat, not being attacked, not full inventory and not have anything to loot
	printf("[+] Doing nothing, shouldn't really be here\n");
}

void Rabbit::Combat(EntityObj* Enemies)
{
	player->Attack(Enemies->EntityId);
}

void Rabbit::Banking()
{
	if (!Widgets::GetWidgetUI(BANKING_WIDGET))
	{
		auto banker = RS::GetEntityNPCByName("Gnome Banker");

		if (!banker)
		{
			printf("Bank find Gnome Banker to bank\n");
			return;
		}

		printf("[+] Going to talk to Bank\n");
		player->BankUsingNPC(banker->EntityId); // Hardcode that bank of Gielinor
		return;
	}
	else
	{
		if (Widgets::GetWidgetUI(BANKING_WIDGET))
		{
			printf("[+] Depositing all inventory to bank\n");
			player->DepositAllThroughBank();
		}
		return;
	}
}

void Rabbit::Looting(std::vector<FakeItemEX> loots)
{
	auto loot = Inventory::GetContainerObj(static_cast<uint32_t>(ContainerType::AreaLoot));

	if (loot)
		player->LootAllConfirm();
	else
		player->Loot(loots[0]);
}
