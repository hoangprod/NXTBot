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
#include "GeneralCombat.h"

extern std::string botStatus;

std::vector<std::string> foodlist = { "Shark", "Chocolate slice", "2/3 chocolate cake", "Chocolate cake", "Salmon", "Trout", "Catfish", "Beltfish", "Sea turtle", "Sailfish" };

void GeneralCombat::Looting(FakeItemEX loot)
{
	auto areaLoot = Inventory::GetContainerObj(static_cast<uint32_t>(ContainerType::AreaLoot));

	if (areaLoot)
		player->LootAllConfirm();
	else if (loot.ItemQuantity != 0)
		player->Loot(loot);
}

void GeneralCombat::ConsumeFood()
{
	for (auto food : foodlist)
	{
		auto eat = Inventory::GetItemNameSlot(food);

		if (eat >= 0)
		{
			Common::InteractWithEquipment(1, eat, 0x5C10007);
			return;
		}
	}

	// Out of food beep
	Beep(523, 100);

	return;
}

bool GeneralCombat::NeedHeal()
{
	int MaxHeath = Exp::GetSkillLevel(Stat::HITPOINTS) * 100;

	int CurrentHealth = Varpbit::GetPlayerHealth();

	if (((float)CurrentHealth / (float)MaxHeath) < 0.5f)
		return true;

	return false;
}

std::vector<uint32_t> Prayer_Potions = {143, 141, 139, 2434};

void GeneralCombat::FSM()
{
	if (!player || !player->_base || !monsterTargetName.data())
	{
		monsterTargetName = RS::GetClosestMonster()->Name;
		player = new Player(RS::GetLocalPlayer());
		origin = player->GetTilePosition();

		//printf("[+] Player %s will be targeting %s from tile position (%d, %d)\n", player->GetName().data(), monsterTargetName.data(), origin.x, origin.y);
	}

	if (GeneralCombat::NeedHeal())
	{
		GeneralCombat::ConsumeFood();
	}

	if (Manual)
		return;

	auto abyss = RS::GetEntityNPCByName("Abyssal walker");

	auto attackingEnemies = RS::GetInCombatNPCwithMe();

	if (abyss)
	{
		if (attackingEnemies.size() < 4)
		{
			// We lost aggro, move to corner
			int RandomX = rand() % 3 + 3059;
			int RandomY = rand() % 3 + 4848;

			player->Move(Tile2D(RandomX, RandomY));
			return;
		}

		if (RS::GetDistance(origin, player->GetTilePosition()) > 6.0f)
		{
			// move back to origin

			int RandomX = rand() % 3 + origin.x;
			int RandomY = rand() % 3 + origin.y;

			player->Move(Tile2D(RandomX, RandomY));
			return;
		}
	}

	auto Dagannoth = RS::GetEntityNPCByName("Dagannoth");

	if (Dagannoth)
	{
		if (!Varpbit::GetVarpBit(AGGRESSION))
		{
			for (int i = 37965; i <= 37971; i+=2)
			{
				auto Aggression_Potion = Inventory::GetItemById(i);

				if (Aggression_Potion >= 0)
				{
					Common::InteractWithEquipment(1, Aggression_Potion, 0x5C10007);
					return;
				}
			}
		}
	}

	// If I am moving around or targeting something, do not fuck with it
	if (player->isMoving() || (player->bTargeting() || player->CurrentTarget() > 0))
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

	if (Varpbit::GetPlayerPrayerToggle())
	{
		if (Varpbit::GetPlayerPrayer() < 1000)
		{
			for (auto prayerPot : Prayer_Potions)
			{
				auto Prayer_Pot = Inventory::GetItemById(prayerPot);

				if (Prayer_Pot >= 0)
				{
					Common::InteractWithEquipment(1, Prayer_Pot, 0x5C10007);
					return;
				}
			}
		}
	}

	if (abyss)
	{
		// Don't Loot if in abyss
	}
	else if(!Inventory::isInventoryFull())
	{
		auto lootsAvailable = Tile::GetAllLootsNearbyWithinRadius(origin, 12.0f);

		for (auto loot : lootsAvailable)
		{
			if (loot.ItemId == 12158 || loot.ItemId == 12159 || loot.ItemId == 12160 || loot.ItemId == 12161 || loot.ItemId == 12163 || loot.ItemId == 30139 || loot.ItemId == 30140 || loot.ItemId == 30141)
			{
				player->Loot(loot);
				return;
			}
		}
	}

	if (attackingEnemies.size() > 0)
	{
		botStatus = "Attacking Monster";

		// Attack the first monster in the list that is attacking us
		return Combat(attackingEnemies[0]);
	}


	//EntityObj* EnemyMonsters = RS::GetMonsterWithinRadiusWithName(monsterTargetName.data(), origin, 15.0f);
	EntityObj* EnemyMonsters = RS::GetClosestMonsterNPCByNameFromOrigin(monsterTargetName.data(), origin, 8.0f);

	// If there are a monster 15 tiles away from origin, fight it
	if (EnemyMonsters)
	{
		botStatus = "Fight monster";
		//auto ent = Entity(EnemyMonsters);

		//printf("Trying to fight %p %d %s %d %d\n", EnemyMonsters, EnemyMonsters->EntityId, EnemyMonsters->Name, ent.NPCCurHealth(), ent.CurrentTarget());
		return Combat(EnemyMonsters);
	}

	// If not in combat, not being attacked, not full inventory and not have anything to loot
	botStatus = "Doing nothing, shouldn't really be here unless no monster around";
}

void GeneralCombat::Combat(EntityObj* Enemy)
{
	player->Attack(Enemy->EntityId);
}

