#include "pch.h"
#include "GameClasses.h"
#include "Patterns.h"
#include "Inventory.h"
#include "Game.h"
#include "PlayableEntity.h"
#include "Tile.h"
#include "Widgets.h"
#include "Experience.h"
#include "Common.h"
#include "Wisp.h"

extern std::string botStatus;
extern Wisp* wisp;
extern Rabbit* rabbit;

extern bool to_suicide;
extern int SelectedWood;
extern int SelectedOre;
extern std::vector<std::string> OreName;
extern std::vector<std::string> OreNode;
extern std::vector<std::string> TreeNames;
extern std::vector<std::string> LogNames;

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
			botStatus = "Cannot find Myrtle, myrtle is too far maybe?";
			return;
		}

		MyrtleId = Myrtle->EntityId;

		//printf("[+] Found Myrtle with id %d\n", MyrtleId);
	}

	// If I am moving around or targeting something, do not fuck with it
	if (player->isMoving() || (player->bTargeting() && player->CurrentTarget() != MyrtleId))
	{
		botStatus = "Current moving or fighting";
		return;
	}

	if (player->inCombat())
	{
		// If already fighting, leave it alone.
		botStatus = "Current fighting";

		return;
	}

	auto attackingEnemies = RS::GetInCombatNPCwithMe();


	if (attackingEnemies.size() > 0)
	{
		botStatus = "Attacking Monster";

		// Attack the first monster in the list that is attacking us
		return Combat(attackingEnemies[0]);
	}

	if (Inventory::isInventoryFull())
	{
		botStatus = "Inventory is full, go bank";

		return Wisp::Banking();
	}

	auto lootsAvailable = Tile::GetAllLootsNearbyWithinRadius(origin, 10.0f);

	if (lootsAvailable.size() > 6)
	{
		botStatus = "Loots found and going to loot";

		FakeItemEX bestLoot = Tile::GetBestLoot(lootsAvailable);

		return Wisp::Looting(bestLoot);
	}

	EntityObj* EnemyMonsters = RS::GetMonsterWithinRadiusWithName("Spellwisp", origin, 15.0f);


	if (to_suicide)
	{
		delete this;
		wisp = 0;
		return;
	}

	// If there are a monster 15 tiles away from origin, fight it
	if (EnemyMonsters)
	{
		botStatus = "Fight monster";

		return Combat(EnemyMonsters);
	}


	// If there are at least X items on the ground within 10 radius
	if (lootsAvailable.size() > 0)
	{
		botStatus = "Loots found and going to loot";

		FakeItemEX bestLoot = Tile::GetBestLoot(lootsAvailable);

		return Wisp::Looting(bestLoot);
	}


	// If not in combat, not being attacked, not full inventory and not have anything to loot
	botStatus = "Doing nothing, shouldn't really be here";

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
		botStatus = "Going to talk to Wizard Myrtle";

		Common::DepositActionNPC(MyrtleId);
		return;
	}
	else
	{
		if (Widgets::GetWidgetUI(CONVERSATION_WIDGET))
		{
			botStatus = "Clicking through chat option";
			Common::ConfirmGUI(0x4A0000F);
		}
		else if (Widgets::GetWidgetUI(DEPOSIT_WIDGET))
		{
			botStatus = "Clicking deposit all items";
			Common::DepositAll();
		}

		return;
	}
}

void Wisp::Looting(FakeItemEX loot)
{
	auto areaLoot = Inventory::GetContainerObj(static_cast<uint32_t>(ContainerType::AreaLoot));

	if (areaLoot)
		return player->LootAllConfirm();
	else if (loot.ItemQuantity != 0)
		player->Loot(loot);
}



// RABBIT =========================================




void Rabbit::FSM()
{
	Player player = RS::GetLocalPlayer();

	if (!player._base)
		return;

	if (!runOnce)
	{
		origin = player.GetTilePosition();

		runOnce = !runOnce;
	}


	// If I am moving around or targeting something, do not fuck with it
	if (player.isMoving() || (player.bTargeting() && player.CurrentTarget() != 25688)) // Hardcode bank id
	{
		botStatus = "Current moving or fighting";
		return;
	}

	if (player.inCombat())
	{
		// If already fighting, leave it alone.
		botStatus = "Current fighting";

		return;
	}

	auto attackingEnemies = RS::GetInCombatNPCwithMe();


	if (attackingEnemies.size() > 0)
	{
		botStatus = "Attacking Monster";
		player.Attack(attackingEnemies[0]->EntityId);

		// Attack the first monster in the list that is attacking us
		return;
	}

	if (Inventory::isInventoryFull())
	{
		botStatus = "Inventory is full, go bank";

		return Rabbit::Banking();
	}

	auto lootsAvailable = Tile::GetAllLootsNearbyWithinRadius(origin, 7.0f);


	// If there are at least X items on the ground within 10 radius
	if (lootsAvailable.size() > 9)
	{
		botStatus = "Loots found and going to loot";

		FakeItemEX bestLoot = Tile::GetBestLoot(lootsAvailable);

		return Rabbit::Looting(bestLoot);
	}

	EntityObj* EnemyMonsters = RS::GetMonsterWithinRadiusWithName("Rabbit", origin, 7.0f);

	/*
	if (to_suicide)
	{
		delete this;
		rabbit = 0;
		return;
	}
	*/

	// If there are a monster 15 tiles away from origin, fight it
	if (EnemyMonsters)
	{
		botStatus = "Fight monster";

		player.Attack(EnemyMonsters->EntityId);
		return;
	}

	// If not in combat, not being attacked, not full inventory and not have anything to loot
	botStatus = "Doing nothing, shouldn't really be here";
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

		botStatus = "Going to talk to Bank";
		Common::BankUsingNPC(banker->EntityId); // Hardcode that bank of Gielinor
		return;
	}
	else
	{
		if (Widgets::GetWidgetUI(BANKING_WIDGET))
		{
			botStatus = "Depositing all inventory to bank";
			Common::DepositAllThroughBank();
		}
		return;
	}
}


void Rabbit::Looting(FakeItemEX loot)
{
	Player player = RS::GetLocalPlayer();

	auto areaLoot = Inventory::GetContainerObj(static_cast<uint32_t>(ContainerType::AreaLoot));

	if (areaLoot)
	{
		player.LootAllConfirm();
		return;
	}
	else if (loot.ItemQuantity != 0)
	{
		player.Loot(loot);
		return;
	}
}

void GenMining::FSM()
{
	if (!player || !player->_base)
	{
		player = new Player(RS::GetLocalPlayer());
		MiningExp = Exp::GetCurrentExp(Stat::MINING);

		TargetNode = OreNode[SelectedOre];
		TargetOre = OreName[SelectedOre];

		Node = Static::GetClosestStaticObjectByName(TargetNode.data());
		if (Node.Definition == NULL)
		{
			botStatus = "Cannot find Node to mine!";
			player = 0;
		}
	}

	if (!player->isMining())
	{
		botStatus = ("Start mining.");
		Common::StaticInteract(Node);
		return;
	}

	int CurrentExp = Exp::GetCurrentExp(Stat::MINING);

	if (CurrentExp > MiningExp)
	{
		botStatus = "Clicking on Mining node";
		Common::StaticInteract(Node);
		MiningExp = CurrentExp;

		if (Inventory::HaveItemName(TargetOre.data()))
		{
			player->QuickDropSlot1();
			botStatus = "Dropping ore.";
		}

		return;
	}

	if (Inventory::isInventoryFull())
	{
		botStatus = "Inventory is full.";

		if (Inventory::HaveItemName(TargetOre.data()))
		{
			botStatus = "Inventory is full, dropping ore";
			player->QuickDropSlot1();
		}

		return;
	}
}

void Penguins::FSM()
{
	if (!player || !player->_base)
	{
		player = new Player(RS::GetLocalPlayer());
	}
	
	botStatus = "Clock work Penguins started.";

	player->ReleaseClockWork();

	player->WindClockWork();

	player->ReleaseClockWork();

	auto mon = RS::GetEntityNPCByName("Penguin suit");

	if(mon)
		Common::BankUsingNPC(mon->EntityId);

	player->ReleaseClockWork();

}

void BikeAgi::FSM()
{
	if (!player || !player->_base)
	{
		player = new Player(RS::GetLocalPlayer());
		botStatus = "Empty Throne room started.";
	}

	auto Bike = Static::GetClosestStaticObjectByName("Manual Auto-cycle (Empowered)");

	if (!Bike.Definition)
	{
		botStatus = "Can't find empowered bike.";
		return;
	}

	auto curAnimation = player->CurrentAnimation();

	if (curAnimation != 29609)
	{
		Common::StaticInteract(Bike);
		return;
	}


	else if (curAnimation == 29609)
	{
		auto playerPos = player->GetTilePosition();

		//printf("%d : %d   %d : %d\n", Bike.TileX + 1 , playerPos.x, Bike.TileY - 12288, playerPos.y);

		float distance = RS::GetDistance(Tile2D(Bike.TileX + 1, Bike.TileY - 12287), playerPos);

		if (distance > 2.0f)
		{
			botStatus = "Going to new empowered bike";
			Common::StaticInteract(Bike);
			return;
		}
		else
		{
			botStatus = "Current Bike is still empowered.";
			return;
		}
	}
	else
	{
		botStatus = "Literally impossible to be here.";
	}
}



void Woodcutting::FSM()
{
	if (!player || !player->_base)
	{
		player = new Player(RS::GetLocalPlayer());
		WoodchoppingExp = Exp::GetCurrentExp(Stat::WOODCUTTING);
		origin = player->GetTilePosition();
		TargetLog = LogNames[SelectedWood];
		TargetTree = TreeNames[SelectedWood];
	}

	if (player->IsInAnimation())
	{

		int CurrentExp = Exp::GetCurrentExp(Stat::WOODCUTTING);

		if (CurrentExp > WoodchoppingExp)
		{
			WoodchoppingExp = CurrentExp;

			player->QuickDropSlot1();

			return;
		}

		if (Inventory::isInventoryFull())
		{
			player->QuickDropSlot1();

			return;
		}

		botStatus = "Currently chopping a tree.";
		return;
	}

	if (player->isMoving())
	{
		botStatus = "Coming to chop down a tree.";
		return;
	}

	auto Closest = Static::GetClosestStaticHarvestableObjectByNameWithOrigin(TargetTree.data(), "Chop down", origin);

	if (Closest.Definition)
	{
		float distance = RS::GetDistance(origin, Tile2D(Closest.TileX, Closest.TileY));

		if (distance > 25.0f)
		{
			botStatus = "No tree in range from origin :)";
			//printf("Closest tree is out of range (%d %d)  (%d  %d) with ditstance: %f\n", origin.x, origin.y, Closest.TileX, Closest.TileY, distance);
			return;
		}

		else {
			botStatus = "Switching tree";

			Common::StaticInteract(Closest);
		}
	}
	else
	{
		botStatus = "Not finding another tree";
		return;
	}

	botStatus = "Not quite sure how we are here..";

	return;
}
