#include "pch.h"
#include "GameClasses.h"
#include "Patterns.h"
#include "Inventory.h"
#include "Game.h"
#include "PlayableEntity.h"
#include "Tile.h"
#include "Widgets.h"
#include "Experience.h"
#include "Varpbit.h"
#include "Common.h"
#include "Archeology.h"

extern int extraDelay;

std::vector<std::pair<std::string, int>> ArcheologyNodes = { std::pair<std::string, int>("Destroyed golem", 98),std::pair<std::string, int>("Gravitron research debris", 91),std::pair<std::string, int>("Big High War God shrine", 89), std::pair<std::string, int>("Goblin dorm debris", 83), std::pair<std::string, int>("Gladiatorial goblin remains", 76), std::pair<std::string, int>("Oikos studio debris", 72),std::pair<std::string, int>("Ikovian memorial", 70), std::pair<std::string, int>("Dominion Games podium", 69), std::pair<std::string, int>("Infernal art", 65), std::pair<std::string, int>("Stadio debris", 61),std::pair<std::string, int>("Ceramics studio debris", 56),std::pair<std::string, int>("Amphitheatre debris", 51), std::pair<std::string, int>("Monoceros remains", 48), std::pair<std::string, int>("Prodromoi remains", 42), std::pair<std::string, int>("Sacrificial altar", 36), std::pair<std::string, int>("Cultist footlocker", 29), std::pair<std::string, int>("Lodge art storage", 24), std::pair<std::string, int>("Lodge bar storage", 20), std::pair<std::string, int>("Castra debris", 17), std::pair<std::string, int>("Venator remains", 1) };

std::vector<std::pair<std::string, int>> ArcheologyCaches = { std::pair<std::string, int>("Material cache (Yu'biusk clay)", 83), std::pair<std::string, int>("Material cache (warforged bronze)", 76), std::pair<std::string, int>("Material cache (malachite green)", 76), std::pair<std::string, int>("Material cache (vulcanised rubber)", 76) };

std::vector<int> ArcheologySoil = { 49517 , 49521, 49519, 49523, 49525 };

std::vector<int> ArcheologyArtefacts = { 49644, 49642, 49747, 49745, 49676, 49666, 49837, 49829, 49736, 49732, 49690, 49714, 49722, 49927, 49933, 49833, 49839, 49767 , 49765, 49769, 49773, 49775, 49753, 49755, 49761, 49811, 49813, 49815 };


void Archeology::FSM()
{
	Player player = RS::GetLocalPlayer();

	if (!player._base)
	{
		return;
	}

	if(runOnce)
	{
		ArcheologyExp = Exp::GetCurrentExp(Stat::ARCHEOLOGY);
		Initializer();

		runOnce = false;
	}


	if (Widgets::IsInDialogWidget() && Widgets::GetDialogType() != RSDialog::ModernDestroy)
	{
		printf("clicking out of dialog\n");
		Common::StaticInteract(Node);
		extraDelay = 2000;
		return;
	}

	if (player.isMoving())
	{
		return;
	}

	//if(!player.IsInAnimation())
	//	Initializer();

	if (Widgets::GetWidgetUI(BANKING_WIDGET))
	{
		if (Inventory::GetFreeSlot() < 22)
		{
			Common::DepositAllThroughBank();
			return;
		}
	}

	if (isCache)
	{
		CacheHandler();
	}

	if (!Node.Definition)
	{
		printf("No Node def\n");
		return;
	}

	DepositBox = Static::GetClosestStaticObjectByName("Portable bank deposit box", true);

	if (!DepositBox.Definition)
	{

		int soil = -1;

		for (auto itemid : ArcheologySoil)
		{
			soil = Inventory::GetItemById(itemid);

			if (soil > -1)
				break;
		}

		for (auto artefact : ArcheologyArtefacts)
		{
			auto item = Inventory::GetItemById(artefact);

			if (item != -1)
			{
				if (Widgets::GetWidgetUI(CONVERSATION_WIDGET) && Widgets::GetDialogType() == RSDialog::ModernDestroy)
				{
					printf("Confirm destroy =)\n");
					Common::ConfirmGUI(0x49F0005);
				}
				else
				{
					printf("Deleting artefacts %d on slot %d\n", artefact, item);
					Inventory::InteractItemOption(item, 8);
				}

				extraDelay = 500;

				return;
			}
		}

		int CurrentExp = Exp::GetCurrentExp(Stat::ARCHEOLOGY);

		// Soilbox
		auto SoilBoxSlot = Inventory::GetItemById(49538);

		// Drop soil
		if (SoilBoxSlot == -1 && soil > -1)
		{
			printf("Dropping soil\n");
			Common::InteractWithEquipment(8, -1, 0x59600B5);
			extraDelay = 2000;
		}

		else if (CurrentExp > ArcheologyExp && soil > -1 && !isSoilBoxFull())
		{
			ArcheologyExp = CurrentExp;

			printf("Fill Soilbox\n");
			// Fill Soil on slot right after 0
			Common::InteractWithEquipment(1, -1, 93716674);
			return;
		}
	}

	if (Inventory::isInventoryFull())
	{
		printf("Banking\n");

		if (Widgets::GetWidgetUI(DEPOSIT_WIDGET))
		{
			Common::DepositAll();
			return;
		}

		if (DepositBox.Definition)
		{
			Common::StaticInteract(DepositBox);
			return;
		}

		MaterialCart = Static::GetClosestStaticObjectByName("Materials cart", true);

		if (MaterialCart.Definition)
		{
			Common::StaticInteract(MaterialCart);
			return;
		}

		auto locator = Static::GetClosestStaticObjectByName("Tunnel", true);

		if (locator.Definition)
		{
			printf("Banking 20\n");
			Archeology::Banking20();
		}
		else
		{
			printf("Banking 1\n");
			Archeology::Banking();
		}

		return;
	}

	auto Temp_Node = Static::GetClosestStaticObjectByName(NodeName.data(), true);

	if (!Temp_Node.Definition)
	{
		printf("Can't find Node.\n");

		auto Lift = Static::GetClosestStaticObjectByName("Lift");

		if (Lift.Definition)
		{
			if (Inventory::GetFreeSlot() == 1)
			{
				printf("Banking 20\n");
				Archeology::Banking20();
				return;
			}

			extraDelay = 9000;
			printf("Going down to lift with id %d\n", Lift.Definition->Id);
			Common::StaticInteract(Lift);
			return;
		}
	}

	if (!player.IsInAnimation() && Temp_Node.Definition)
	{
		printf("Trying to interact with node\n");
		Common::StaticInteract(Node);
		extraDelay = 5000;
		return;
	}
}

void Archeology::Initializer()
{
	Player player = RS::GetLocalPlayer();

	ArcheLevel = Exp::GetSkillLevel(Stat::ARCHEOLOGY);

	MaterialCart = Static::GetClosestStaticObjectByName("Materials cart", true);

	if (!isCache)
	{
		for (auto item : ArcheologyNodes)
		{
			if (ArcheLevel < item.second)
				continue;

			Node = Static::GetClosestStaticObjectByName(item.first.data(), true);

			if (Node.Definition && RS::GetDistance(Tile2D(Node.TileX, Node.TileY), player.GetTilePosition()) < 7.0f)
			{
				NodeName = item.first;
				break;
			}
		}


		//printf("Current have %d exp in Archeology\n", ArcheologyExp);

		if (Node.Definition == NULL)
		{
			printf("Cannot find the node you are looking for.\n");
			return;
		}
		else
		{
			printf("Set Excavation id %d - %d x %d y %d\n", Node.Definition->Id, Node.SecondId, Node.TileX, Node.TileY);
		}
	}

}

void Archeology::Banking()
{
	// If not talking NOR banking, then go bank
	if (!Widgets::GetWidgetUI(BANKING_WIDGET))
	{
		StaticObjEX chest = Static::GetClosestStaticObjectByName("Bank chest");
		
		if (chest.Definition)
		{
			printf("Clicking on chest\n");
			Common::StaticInteract2(chest);
			return;
		}

	}
	else
	{
		// Soilbox
		auto SoilBoxSlot = Inventory::GetItemById(49538);

		printf("Soilbox at %d and soil amount is %d\n", SoilBoxSlot, getSoilBoxAmount());

		if (SoilBoxSlot > -1 && getSoilBoxAmount() > 0)
		{
			printf("Emptying soilbox %d!\n", SoilBoxSlot);
			// 9 is to empty
			Common::BankInteractItem(SoilBoxSlot, 9);
			return;
		}

		if (Inventory::GetFreeSlot() < 27)
		{
			Common::DepositAllThroughBank();
			return;
		}
		else
		{
			if (isCache)
			{
				auto Cache = Inventory::GetItemById(49480);

				if (Cache > -1)
				{
					Common::BankInteractItem(Cache, 1);
				}
			}
			else
			{
				if (SoilBoxSlot == -1)
					Common::DepositAllThroughBank();
				else
					Common::BankLoadPreset(1);
			}
			return;
		}

		return;
	}
}

void Archeology::Banking20()
{
	// If not talking NOR banking, then go bank
	if (!Widgets::GetWidgetUI(BANKING_WIDGET))
	{
		StaticObjEX chest = Static::GetClosestStaticObjectByName("Bank chest");

		if (chest.Definition)
		{
			printf("Clicking on chest\n");
			Common::StaticInteract2(chest);
			return;
		}
		else
		{
			StaticObjEX tunnel = Static::GetClosestStaticObjectByName("Tunnel");

			if (tunnel.Definition)
			{
				extraDelay = 3000;
				printf("Going to tunnel\n");
				Common::StaticInteract(tunnel);
				return;
			}
		}

	}
	else
	{
		// Soilbox
		auto SoilBoxSlot = Inventory::GetItemById(49538);

		if (SoilBoxSlot > -1 && getSoilBoxAmount() > 0)
		{
			printf("Emptying soilbox %d!\n", SoilBoxSlot);
			// 9 is to empty
			Common::BankInteractItem(SoilBoxSlot, 9);
			return;
		}

		if (Inventory::GetFreeSlot() > 1)
		{
			Common::DepositAllThroughBank();
			return;
		}
		else
		{
			Common::BankLoadPreset(1);
			return;
		}

		return;
	}
}

void Archeology::CacheHandler()
{
	for (auto item : ArcheologyCaches)
	{
		if (ArcheLevel < item.second)
			continue;

		Node = Static::GetClosestStaticHarvestableObjectByNameWithOrigin(item.first.data(), "Excavate", RS::GetLocalPlayerTilePos());

		if (Node.Definition && RS::GetDistance(Tile2D(Node.TileX, Node.TileY), RS::GetLocalPlayerTilePos()) < 40.0f)
		{
			NodeName = item.first;

			//printf("Selected %s with option pointer %p\n", NodeName.data(), Node.Definition);

			break;
		}
	}

	if (!Node.Definition)
	{
		if (Common::IsWorldWidgetUp())
		{
			Common::HopRandomWorld();
			extraDelay = 6000;
		}
		else
		{
			Common::HopWorldGUI();
			extraDelay = 800;
		}

		return;
	}
	else
	{
		if (Common::IsWorldWidgetUp())
		{
			printf("Closing World Widget\n");
			Common::InteractWithEquipment(1, -1, 0x6330072);
		}
	}
}

bool Archeology::isSoilBoxFull()
{
	return getSoilBoxAmount() == 50;
}

int Archeology::getSoilBoxAmount()
{
	return Varpbit::GetVarp(SOIL_BOX_FIERY) + Varpbit::GetVarp(SOIL_BOX_GRAVEL) + Varpbit::GetVarp(SOIL_BOX_AERATED);
}
