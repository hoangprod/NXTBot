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

std::vector<std::pair<std::string, int>> ArcheologyNodes = { std::pair<std::string, int>("Oikos studio debris", 72),std::pair<std::string, int>("Dominion games podium", 69), std::pair<std::string, int>("Infernal art", 65), std::pair<std::string, int>("Stadio debris", 61),std::pair<std::string, int>("Ceramics studio debris", 56),std::pair<std::string, int>("Amphitheatre debris", 51), std::pair<std::string, int>("Monoceros remains", 48), std::pair<std::string, int>("Prodromoi remains", 42), std::pair<std::string, int>("Sacrificial altar", 36), std::pair<std::string, int>("Cultist footlocker", 29), std::pair<std::string, int>("Lodge art storage", 24), std::pair<std::string, int>("Lodge bar storage", 20), std::pair<std::string, int>("Venator remains", 1) };

std::vector<int> ArcheologySoil = { 49517 , 49521, 49519, 49523, 49525 };

std::vector<int> ArcheologyArtefacts = { 49833, 49839, 49767 , 49765, 49769, 49773, 49775, 49753, 49755, 49761 };

void Archeology::FSM()
{
	if (!player || !player->_base)
	{
		player = new Player(RS::GetLocalPlayer());

		int ArcheLevel = Exp::GetSkillLevel(Stat::ARCHEOLOGY);

		MaterialCart = Static::GetClosestStaticObjectByName("Materials cart", true);

		for (auto item : ArcheologyNodes)
		{
			if (ArcheLevel < item.second)
				continue;

			Node = Static::GetClosestStaticObjectByName(item.first.data(), true);

			if (Node.Definition && RS::GetDistance(Tile2D(Node.TileX, Node.TileY), player->GetTilePosition()) < 7.0f)
			{
				NodeName = item.first;
				break;
			}
		}


		ArcheologyExp = Exp::GetCurrentExp(Stat::ARCHEOLOGY);

		printf("Current have %d exp in Archeology\n", ArcheologyExp);



		if (Node.Definition == NULL)
		{
			printf("Cannot find the node you are looking for.\n");
			player = 0;
			return;
		}
		else
		{
			printf("Set Excavation id %d - %d x %d y %d\n", Node.Definition->Id, Node.SecondId, Node.TileX, Node.TileY);
		}
	}

	if (Widgets::IsInDialogWidget() && Widgets::GetDialogType() != RSDialog::ModernDestroy)
	{
		printf("clicking out of dialog\n");
		Common::StaticInteract(Node);
		extraDelay = 2000;
		return;
	}

	if (player->isMoving())
	{
		printf("Moving...\n");
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
					printf("Deleting artefacts\n");
					Inventory::InteractItemOption(item, 8);
					extraDelay = 300;
				}

				return;
			}
		}

		int CurrentExp = Exp::GetCurrentExp(Stat::ARCHEOLOGY);

		// Soilbox
		auto SoilBoxSlot = Inventory::GetItemById(49538);

		// Drop soil
		if (SoilBoxSlot == -1 && soil > -1)
		{
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

	if (!player->IsInAnimation() && Temp_Node.Definition)
	{
		printf("Trying to interact with node\n");
		Common::StaticInteract(Node);
		extraDelay = 5000;
		return;
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

bool Archeology::isSoilBoxFull()
{
	return getSoilBoxAmount() == 50;
}

int Archeology::getSoilBoxAmount()
{
	return Varpbit::GetVarp(SOIL_BOX_FIERY) + Varpbit::GetVarp(SOIL_BOX_GRAVEL);
}
