#include "pch.h"
#include "GameClasses.h"
#include "Patterns.h"
#include "Inventory.h"
#include "Game.h"
#include "PlayableEntity.h"
#include "Tile.h"
#include "Widgets.h"
#include "Experience.h"
#include "Wisp.h"

extern std::string botStatus;

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
			botStatus = std::string("[!] Cannot find Myrtle, myrtle is too far maybe?\n");
			return;
		}

		MyrtleId = Myrtle->EntityId;

		//printf("[+] Found Myrtle with id %d\n", MyrtleId);
	}

	// If I am moving around or targeting something, do not fuck with it
	if (player->isMoving() || (player->bTargeting() && player->CurrentTarget() != MyrtleId))
	{
		botStatus = std::string("[+] Current moving or fighting\n");
		return;
	}

	if (player->inCombat())
	{
		// If already fighting, leave it alone.
		botStatus = std::string("[+] Current fighting\n");

		return;
	}

	auto attackingEnemies = RS::GetInCombatNPCwithMe();


	if (attackingEnemies.size() > 0)
	{
		botStatus = std::string("[+] Attacking Monster\n");

		// Attack the first monster in the list that is attacking us
		return Combat(attackingEnemies[0]);
	}

	if (Inventory::isInventoryFull())
	{
		botStatus = std::string("[+] Inventory is full, go bank\n");

		return Wisp::Banking();
	}

	auto lootsAvailable = Tile::GetAllLootsNearbyWithinRadius(origin, 10.0f);

	if (lootsAvailable.size() > 6)
	{
		botStatus = std::string("[+] Loots found and going to loot\n");

		FakeItemEX bestLoot = Tile::GetBestLoot(lootsAvailable);

		return Wisp::Looting(bestLoot);
	}

	EntityObj* EnemyMonsters = RS::GetMonsterWithinRadiusWithName("Spellwisp", origin, 15.0f);


	// If there are a monster 15 tiles away from origin, fight it
	if (EnemyMonsters)
	{
		botStatus = std::string("[+] Fight monster\n");

		return Combat(EnemyMonsters);
	}


	// If there are at least X items on the ground within 10 radius
	if (lootsAvailable.size() > 0)
	{
		botStatus = std::string("[+] Loots found and going to loot\n");

		FakeItemEX bestLoot = Tile::GetBestLoot(lootsAvailable);

		return Wisp::Looting(bestLoot);
	}


	// If not in combat, not being attacked, not full inventory and not have anything to loot
	botStatus = std::string("[+] Doing nothing, shouldn't really be here\n");

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
		botStatus = std::string("[+] Going to talk to Wizard Myrtle\n");

		player->DepositActionNPC(MyrtleId);
		return;
	}
	else
	{
		if (Widgets::GetWidgetUI(CONVERSATION_WIDGET))
		{
			botStatus = std::string("[+] Clicking through chat option\n");
			player->ConfirmChat();
		}
		else if (Widgets::GetWidgetUI(DEPOSIT_WIDGET))
		{
			botStatus = std::string("[+] Clicking deposit all items\n");
			player->DepositAll();
		}

		return;
	}
}

void Wisp::Looting(FakeItemEX loot)
{
	auto areaLoot = Inventory::GetContainerObj(static_cast<uint32_t>(ContainerType::AreaLoot));

	if (areaLoot)
		player->LootAllConfirm();
	else if (loot.ItemQuantity != 0)
		player->Loot(loot);
}

void GeneralCombat::FSM()
{
	if (!player || !player->_base)
	{
		monsterTargetName = RS::GetClosestMonster()->Name;
		player = new Player(RS::GetLocalPlayer());
		origin = player->GetTilePosition();

		//printf("[+] Player %s will be targeting %s from tile position (%d, %d)\n", player->GetName().data(), monsterTargetName.data(), origin.x, origin.y);
	}

	// If I am moving around or targeting something, do not fuck with it
	if (player->isMoving() || (player->bTargeting() || player->CurrentTarget() > 0))
	{
		botStatus = std::string("[+] Current moving or fighting\n");
		return;
	}

	if (player->inCombat())
	{
		// If already fighting, leave it alone.
		botStatus = std::string("[+] Current fighting\n");

		return;
	}

	auto attackingEnemies = RS::GetInCombatNPCwithMe();


	if (attackingEnemies.size() > 0)
	{
		botStatus = std::string("[+] Attacking Monster\n");

		// Attack the first monster in the list that is attacking us
		return Combat(attackingEnemies[0]);
	}

	EntityObj* EnemyMonsters = RS::GetMonsterWithinRadiusWithName(monsterTargetName.data(), origin, 10.0f);


	// If there are a monster 15 tiles away from origin, fight it
	if (EnemyMonsters)
	{
		botStatus = std::string("[+] Fight monster\n");

		return Combat(EnemyMonsters);
	}

	// If not in combat, not being attacked, not full inventory and not have anything to loot
	botStatus = std::string("[+] Doing nothing, shouldn't really be here unless no monster around\n");
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
		botStatus = std::string("[+] Current moving or fighting\n");
		return;
	}

	if (player->inCombat())
	{
		// If already fighting, leave it alone.
		botStatus = std::string("[+] Current fighting\n");

		return;
	}

	auto attackingEnemies = RS::GetInCombatNPCwithMe();


	if (attackingEnemies.size() > 0)
	{
		botStatus = std::string("[+] Attacking Monster\n");

		// Attack the first monster in the list that is attacking us
		return Combat(attackingEnemies[0]);
	}

	if (Inventory::isInventoryFull())
	{
		botStatus = std::string("[+] Inventory is full, go bank\n");

		return Rabbit::Banking();
	}

	auto lootsAvailable = Tile::GetAllLootsNearbyWithinRadius(origin, 7.0f);


	// If there are at least X items on the ground within 10 radius
	if (lootsAvailable.size() > 9)
	{
		botStatus = std::string("[+] Loots found and going to loot\n");

		FakeItemEX bestLoot = Tile::GetBestLoot(lootsAvailable);

		return Rabbit::Looting(bestLoot);
	}

	EntityObj* EnemyMonsters = RS::GetMonsterWithinRadiusWithName("Rabbit", origin, 7.0f);


	// If there are a monster 15 tiles away from origin, fight it
	if (EnemyMonsters)
	{
		botStatus = std::string("[+] Fight monster\n");

		return Rabbit::Combat(EnemyMonsters);
	}

	// If not in combat, not being attacked, not full inventory and not have anything to loot
	botStatus = std::string("[+] Doing nothing, shouldn't really be here\n");
}

void Rabbit::Combat(EntityObj* Enemies)
{
	player->Attack(Enemies->EntityId);
	return;
}

void Rabbit::Banking()
{
	if (!Widgets::GetWidgetUI(BANKING_WIDGET))
	{
		auto banker = RS::GetEntityNPCByName("Gnome Banker");

		if (!banker)
		{
			printf("[!] Can't find Gnome Banker to bank\n");
			return;
		}

		botStatus = std::string("[+] Going to talk to Bank\n");
		player->BankUsingNPC(banker->EntityId); // Hardcode that bank of Gielinor
		return;
	}
	else
	{
		if (Widgets::GetWidgetUI(BANKING_WIDGET))
		{
			botStatus = std::string("[+] Depositing all inventory to bank\n");
			player->DepositAllThroughBank();
		}
		return;
	}
}

void Rabbit::Looting(FakeItemEX loot)
{
	auto areaLoot = Inventory::GetContainerObj(static_cast<uint32_t>(ContainerType::AreaLoot));

	if (areaLoot)
	{
		player->LootAllConfirm();
		return;
	}
	else if (loot.ItemQuantity != 0)
	{
		player->Loot(loot);
		return;
	}
}

void GenMining::FSM()
{
	if (!player || !player->_base)
	{
		player = new Player(RS::GetLocalPlayer());
		MiningExp = Exp::GetCurrentExp(SkillType::MINING);
	}

	if (!player->isMining())
	{
		printf("[+] Start mining.");
		player->Mine(StaticObj());
		return;
	}

	int CurrentExp = Exp::GetCurrentExp(SkillType::MINING);

	if (CurrentExp > MiningExp)
	{
		player->Mine(StaticObj());
		MiningExp = CurrentExp;

		if (Inventory::HaveItemName("Mithril ore"))
		{
			player->QuickDropSlot1();
			printf("[+] Dropping ore.");
		}

		return;
	}

	if (Inventory::isInventoryFull())
	{
		if (Inventory::HaveItemName("Mithril ore"))
		{
			player->QuickDropSlot1();
		}

		return;
	}
}
