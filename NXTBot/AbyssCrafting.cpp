#include "pch.h"
#include "GameClasses.h"
#include "Patterns.h"
#include "Inventory.h"
#include "Game.h"
#include "PlayableEntity.h"
#include "Tile.h"
#include "Widgets.h"
#include "Common.h"
#include "Experience.h"
#include "AbyssCrafting.h"

extern std::string botStatus;
extern int extraDelay;

std::vector<std::string> Pouches = { "Small pouch" , "Medium pouch" , "Large pouch", "Giant pouch" };


std::vector<std::pair<std::string, std::string>> CraftableRunes = { std::pair<std::string, std::string>("Nature rune", "Nature rift"), std::pair<std::string, std::string>("Cosmic rune", "Cosmic rift") };

std::vector<StaticObjEX> WallList = { StaticObjEX(65086, 3093, 3521), StaticObjEX(65082, 3096, 3521), StaticObjEX(65082, 3099, 3521), StaticObjEX(65084, 3102, 3521), StaticObjEX(65079, 3090, 3521) };


void AbyssCrafting::FSM()
{
	Player player = RS::GetLocalPlayer();


	size_t RCExp = Exp::GetCurrentExp(Stat::RUNECRAFT);

	// Level 91 
	if (RCExp > 5902830)
	{
		SelectedRune = "Nature rune";
		SelectedRift = "Nature rift";
	}
	else
	{
		SelectedRune = "Cosmic rune";
		SelectedRift = "Cosmic rift";
	}
	
	if (Inventory::GetFreeSlot() == 28) 
	{
		Beep(500, 200);
		return;
	}

	auto item = Inventory::GetItemNameSlot("Key token");

	if (item != -1)
	{
		if (Widgets::GetWidgetUI(CONVERSATION_WIDGET) && Widgets::GetDialogType() == RSDialog::ModernDestroy)
		{
			Common::ConfirmGUI(0x49F0005);
		}
		else
		{
			Inventory::InteractItemOption(item, 8);
		}
	}

	if (!IsInAbyss() && !IsInAltar())
	{
		auto enemy = RS::GetValidWildernessPlayerEnemy();

		if (enemy)
		{
			botStatus = "Enemy Found!";

			if (Common::IsWorldWidgetUp())
			{
				Common::HopRandomWorld();
				extraDelay = 4000;
			}
			else
			{
				printf("Trying to open world hop!\n");
				Common::HopWorldGUI();
			}

			return;
		}
		else
		{
			if (Common::IsWorldWidgetUp())
			{
				Common::InteractWithEquipment(1, -1, 0x6330072);
			}
		}
	}



	if (player.GetTilePosition().y > 3521 && !IsInAbyss() && RS::GetInCombatNPCwithMe().size() > 0)
	{
		botStatus = "Being attacked, teleporting out!";
		Common::InteractWithEquipment(2, 3);
		Beep(523, 100);
		return;
	}


	if (player.isMoving() || player.IsInAnimation())
		return;


	// If near bank and inventory not full AND have Cosmic rune, go bank
	auto banker = RS::GetMonsterWithinRadiusWithName("Banker", player.GetTilePosition(), 50.0f);

	if (banker && (!Inventory::isInventoryFull() && Inventory::HaveItemName(SelectedRune.data())) || (Inventory::HaveItemName("Pure essence") && Inventory::GetFreeSlot() == 1))
	{
		FillPouches();
		return;
	}

	// If inventory is full and I have pure essence and NOT in abyss
	else if (Inventory::isInventoryFull() && Inventory::HaveItemName("Pure essence") && !IsInAbyss() && !IsInAltar())
	{
		// If before wilderness wall
		if (player.GetTilePosition().y < 3521)
		{
			botStatus = "Going to wilderness wall";

			int randomIndex = rand() % WallList.size();

			auto wilderness = WallList[randomIndex];

			//printf("Going to %d %d %d\n", wilderness.SecondId, wilderness.TileX, wilderness.TileY);

			player.StaticInteractManual(wilderness.SecondId, wilderness.TileX, wilderness.TileY);

			return;
			/*
			auto wall = Static::GetClosestStaticObjectByName("Wilderness wall");

			if (wall.Definition)
			{
				botStatus = "Going to wilderness wall";

				player.StaticInteractManual(65084, 3102, 3521);
				return;
			}
			else
			{
				botStatus = "Can't find wilderness wall!\n";
				return;
			}*/
		}
		else
		{
			botStatus = "Going to the Mage.";
			//player.TeleportToAbyssThroughMage();

			//if (player.GetTilePosition().y > 3520 && player.GetTilePosition().y < 3524)
			//	player.InteractWithEquipment(1, -1, 0x59600B5); // Surge

			Common::TeleportToAbyssThroughMage();
			return;
		}
	}
	else if (IsInAbyss() && isOuterAbyss())
	{
		auto widget = Static::GetClosestAbyssEntrance();

		if (widget.Definition)
		{
			botStatus = "Trying to go to inner circle.";
			Common::StaticInteract(widget);
			return;
		}
		else
		{
			botStatus = "This is bad. Can't find the obstacle to go into the inner ring!!";
			return;
		}
	}
	else if (IsInAbyss() && isInnerAbyss())
	{
		if (NeedToRepairPouches())
		{
			RepairPouches();
			return;
		}
		else
		{
			auto runeGate = Static::GetClosestStaticObjectByName(SelectedRift.data());

			if (runeGate.Definition)
			{
				botStatus = "Going to the Rune Rift";
				Common::StaticInteract(runeGate);

				//if(RS::GetDistance(player.GetTilePosition(), Tile2D(runeGate.TileX, runeGate.TileY)) > 6.0f)
				//	player.InteractWithEquipment(1, -1, 0x59600B5); // Surge

				botStatus = "Going to the Rune Rift";
				Common::StaticInteract(runeGate);
				return;
			}
		}
	}
	else if (IsInAltar())
	{
		// If still have Pure essence means we havent crafted yet
		if (Inventory::HaveItemName("Pure essence"))
		{
			auto Altar = Static::GetClosestStaticObjectByName("Altar");

			if (Altar.Definition)
			{
				botStatus = "Crafting rune at altar";
				Common::StaticInteract(Altar);
				return;
			}
		}
		else
		{
			// Means we already crafted, time to TP home
			botStatus = "Teleporting back to Edgevile";
			Common::InteractWithEquipment(2, 3);
			return;
		}
	}

	botStatus = "I don't know what is going on here";

}


void AbyssCrafting::FillPouches()
{
	if (!Inventory::isBankOpened())
	{
		auto banker = RS::GetClosestEntityNPCByName("Banker");

		if (!banker)
		{
			botStatus = "Cannot locate banker.";
			return;
		}

		botStatus = "Going to talk to Bank";
		Common::BankUsingNPC(banker->EntityId); // Hardcode that bank of Gielinor
		return;
	}
	else
	{
		for (auto pouchName : Pouches)
		{
			auto pouch = Inventory::GetItemNameSlot(pouchName);

			if (pouch > -1)
				Common::BankInteractItem(pouch, 8);
		}

		botStatus = "Loaded bank preset and readied inventory";
		if (Inventory::GetFreeSlot() == 1)
		{
			Common::BankLoadPreset(2);
		}
		else
			Common::BankLoadPreset(1);
	}
}

void AbyssCrafting::RepairPouches()
{
	if (isInnerAbyss())
	{

		if (!Widgets::IsInDialogWidget())
		{
			auto darkMage = RS::GetEntityNPCByName("Dark mage");

			Common::DepositActionNPC(darkMage->EntityId);

			botStatus = "Going to talk to Dark Mage";

			return;
		}
		else
		{
			// We just talked to him
			if (Widgets::GetDialogType() == RSDialog::PlayerDialog)
			{
				botStatus = "Clicking through first dialog";
				Common::ConfirmGUI(0x4A7000F);
				return;
			}
			// He just repaired our shits
			else if (Widgets::GetDialogType() == RSDialog::NPCDialog)
			{
				botStatus = "Clicking through second dialog";

				Common::ConfirmGUI(0x4A0000F);
				return;
			}
			// Probably already repaired so aka u dont need it anymore
			else if (Widgets::GetDialogType() == RSDialog::GameDialog)
			{
				botStatus = "Pouches already repaired bro.";
				return;
			}

		}
	}

	botStatus = "Is not in inner abyss, how is this being called?";
	return;
}



bool AbyssCrafting::FinishedCrafting()
{
	if (IsInAltar() && !Inventory::isInventoryFull() && Inventory::HaveItemName(SelectedRune.data())) // Update rune name
		return true;

	return false;
}

//Cosmic rift
bool AbyssCrafting::IsInAbyss()
{
	if (RS::GetEntityNPCByName("Abyssal walker") || RS::GetEntityNPCByName("Abyssal leech") || RS::GetEntityNPCByName("Abyssal guardian") || RS::GetEntityNPCByName("Dark mage"))
		return true;
	return false;
}

bool AbyssCrafting::IsInAltar()
{
	auto Altar = Static::GetClosestStaticObjectByName("Altar");

	if (Altar.Definition)
	{
		if (RS::GetDistance(RS::GetLocalPlayerTilePos(), Tile2D(Altar.TileX, Altar.TileY)) < 10.0f)
			return true;
	}

	return false;
}

bool AbyssCrafting::isOuterAbyss()
{

	if (!IsInAbyss())
		return false;

	auto darkMage = RS::GetEntityNPCByName("Dark mage");

	if (!darkMage)
	{
		return true;
	}

	float distance = RS::GetDistance(RS::GetLocalPlayerTilePos(), RS::GetEntityTilePos(darkMage));

	if (distance > 17.5f)
		return true;

	return false;
}

bool AbyssCrafting::isInnerAbyss()
{
	if (!IsInAbyss())
		return false;

	auto darkMage = RS::GetEntityNPCByName("Dark mage");

	if (!darkMage)
	{
		return true;
	}

	float distance = RS::GetDistance(RS::GetLocalPlayerTilePos(), RS::GetEntityTilePos(darkMage));

	if (distance < 17.4f)
		return true;

	return false;
}

bool AbyssCrafting::NeedToRepairPouches()
{
	if (Inventory::GetItemById(5511) > -1 || Inventory::GetItemById(5513) > -1 || Inventory::GetItemById(5515) > -1)
		return true;

	return false;
}
