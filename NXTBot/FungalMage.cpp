#pragma once
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
#include "Varpbit.h"
#include "FungalMage.h"

extern int extraDelay;

extern FungalMage* fungalMage;

extern std::vector<std::string> foodlist;

extern bool to_suicide;

void FungalMage::FSM()
{
	Player player = RS::GetLocalPlayer();

	if (!player._base)
		return;

	if (!runOnce)
	{
		printf("Starting\n");


		if(IsInCave())
			origin = player.GetTilePosition();
		else
		{
			origin = Tile2D(4640, 5488);
		}
		runOnce = !runOnce;
	}

	if (FungalMage::NeedHeal())
	{
		if (!FungalMage::ConsumeFood() && !player.inCombat())
		{
			// We are out of food
			if (Widgets::GetWidgetUI(SELECT_AN_OPTION_TELEPORT_WIDGET))
			{
				printf("Teleport option open\n");

				// Select Teleport to Dual Arena
				Common::ConfirmGUI(0x62A0001);
				extraDelay = 8000;
				return;
			}
			else
			{
				// We haven't rub the ring yet
				for (int i = 2552; i <= 2566; i++)
				{
					int RingOfDueling = Inventory::GetItemById(i);

					if (RingOfDueling != -1)
					{
						printf("Teleporting using RoD\n");

						Common::InteractWithEquipment(7, RingOfDueling, 0x5C10007);
						extraDelay = 800;
						return;
					}

				}

				// Does not have Ring of Dueling!
				Beep(500, 500);
				extraDelay = 1500;
				return;
			}
		}
		return;
	}

	if (player.isMoving() || player.IsInAnimation() || player.inCombat())
	{
		printf("Is busy\n");
		return;
	}

	auto attackingEnemies = RS::GetInCombatNPCwithMe();


	if (!HaveFoodInInvent() && !FungalMage::IsInCave())
	{
		printf("Need food and not in cave\n");

		auto Mubariz = RS::GetClosestEntityNPCByName("Mubariz", false);

		if (Mubariz && RS::GetDistance(RS::GetEntityObjTile2d(Mubariz), player.GetTilePosition()) < 15.0f)
		{
			printf("Walking to midpoint to chest\n");

			int RandomX = rand() % 5 + 3366;
			int RandomY = rand() % 5 + 3270;

			player.Move(Tile2D(RandomX, RandomY));

			extraDelay = 500;

			return;
		}
		else
		{
			if (!Widgets::GetWidgetUI(BANKING_WIDGET))
			{
				auto obj = Static::GetClosestStaticObjectByName("Bank chest");

				printf("Finding bank chest\n");

				if (obj.Definition)
				{
					printf("Found it %d %d %d %d\n", obj.Definition->Id, obj.SecondId, obj.TileX, obj.TileY);
					Common::StaticInteract2(obj);

					extraDelay = 500;

					return;
				}
			}
			else
			{
				printf("Preset loading 1\n");
				Common::BankLoadPreset(1);
				return;
			}
		}
	}
	// Have food but not in cave
	else if (HaveFoodInInvent() && !FungalMage::IsInCave() && attackingEnemies.size() == 0)
	{
		auto obj = Static::GetClosestStaticObjectByName("Bank chest");

		printf("Have food and not in cave\n");

		// If near chest
		if (obj.Definition && RS::GetDistance(player.GetTilePosition(), Tile2D(obj.TileX, obj.TileY)) < 8.0f)
		{
			printf("Walking to midpoint to cave\n");

			// Walk to midpoint to cave
			int RandomX = rand() % 5 + 3379;
			int RandomY = rand() % 5 + 3292;

			player.Move(Tile2D(RandomX, RandomY));

			extraDelay = 500;

			return;
		}
		// If far from chest == at midpoint already
		else
		{
			auto obj = Static::GetClosestStaticObjectByName("Cave entrance");
			if (obj.Definition)
			{
				printf("Found Cave %d %d %d %d\n", obj.Definition->Id, obj.SecondId, obj.TileX, obj.TileY);
				Common::StaticInteract(obj);
				extraDelay = 500;

				return;
			}
		}
	}
	else if(!HaveFoodInInvent() && FungalMage::IsInCave())
	{
		if (Widgets::GetWidgetUI(SELECT_AN_OPTION_TELEPORT_WIDGET))
		{
			printf("Teleport option open\n");

			// Select Teleport to Dual Arena
			Common::ConfirmGUI(0x62A0001);
			extraDelay = 8000;
			return;
		}
		else
		{
			// We haven't rub the ring yet
			for (int i = 2552; i <= 2566; i++)
			{
				int RingOfDueling = Inventory::GetItemById(i);

				if (RingOfDueling != -1)
				{
					printf("Teleporting using RoD\n");

					Common::InteractWithEquipment(7, RingOfDueling, 0x5C10007);
					extraDelay = 800;
					return;
				}

			}

			// Does not have Ring of Dueling!
			Beep(500, 500);
			extraDelay = 1500;
			return;
		}
	}


	auto lootsAvailable = Tile::GetAllLootsNearbyWithinRadius(origin, 9.0f);

	for (auto loot : lootsAvailable)
	{

		if (loot.ItemId == 3138 || loot.ItemId == 3139 || loot.ItemId == 12176)
		{
			printf("looting item %d at %d %d\n", loot.ItemId, loot.Pos.x, loot.Pos.y);
			player.Loot(loot);
			extraDelay = 400;

			return;
		}
	}

	if (FungalMage::IsFarFromOrigin(player.GetTilePosition()))
	{
		int RandomX = rand() % 2 + origin.x;
		int RandomY = rand() % 2 + origin.y;

		player.Move(Tile2D(RandomX, RandomY));

		extraDelay = 500;

		return;
	}
	else
	{
		if (attackingEnemies.size() > 0)
		{
			// Attack the first monster in the list that is attacking us
			player.Attack(attackingEnemies[0]->EntityId);
			return;
		}

		auto Fungal = RS::GetClosestMonsterNPCByNameFromOrigin("Fungal mage", origin, 7.0f);

		if (Fungal)
		{
			if (to_suicide)
			{
				delete this;
				fungalMage = 0;
				return;
			}

			player.Attack(Fungal->EntityId);
			return;
		}
	}

}

bool FungalMage::ConsumeFood()
{
	for (auto food : foodlist)
	{
		auto eat = Inventory::GetItemNameSlot(food);

		if (eat >= 0)
		{
			Common::InteractWithEquipment(1, eat, 0x5C10007);
			return true;
		}
	}

	return false;
}

bool FungalMage::NeedHeal()
{
	int MaxHeath = Exp::GetSkillLevel(Stat::HITPOINTS) * 100;

	int CurrentHealth = Varpbit::GetPlayerHealth();

	if (((float)CurrentHealth / (float)MaxHeath) < 0.5f)
		return true;

	return false;
}

bool FungalMage::HaveFoodInInvent()
{
	for (auto food : foodlist)
	{
		auto eat = Inventory::GetItemNameSlot(food);

		if (eat >= 0)
		{
			return true;
		}
	}

	return false;
}

bool FungalMage::IsInCave()
{
	auto Fungal = RS::GetClosestMonsterNPCByName("Fungal mage");

	if (Fungal)
		return true;
	else
	{
		auto Rodent = RS::GetClosestMonsterNPCByName("Fungal rodent");

		if (Rodent)
			return true;

		return false;
	}
}

bool FungalMage::IsFarFromOrigin(Tile2D currentPos)
{
	auto distance = RS::GetDistance(currentPos, origin);

	if (distance > 15.0f)
		return true;

	return false;
}
