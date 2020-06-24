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
#include "SpiritualMage.h"

extern spiritual_mage* spiritualmage;
extern std::string botStatus;
extern bool to_suicide;
extern std::vector<int> food_id;

std::vector<int> alc_item_list = { 1305, 1149, 47284, 47312, 47280, 11732, 13006, 1397, 1395, 1399, 1393, 25740, 2366, 1249, 47315, 47286, 47286, 47240 };
std::vector<int> loot_item_list = { 1305, 1149, 39814, 29863, 574, 32341, 566, 995, 47284, 47312, 47280, 11732, 13006, 1397, 1395, 1399, 1393, 1392, 32380, 32381, 32382, 32383, 32384, 32385, 32386, 32387, 32388, 32389, 32390, 32391, 25740, 2366, 1249, 9342, 1618, 1632, 5289, 47315, 537, 48576, 384, 8783, 5316, 220, 3052, 1516 };


float max_distance = 12.0f;
extern int extraDelay;

void spiritual_mage::FSM()
{
	Player player = RS::GetLocalPlayer();

	if (!Origin.x || !Origin.y)
		Origin = player.GetTilePosition();

	// if list is not init
	if (unique_item_do_not_drop.size() == 0)
	{
		// Get inventory items
		auto current_inventory = Inventory::GetInventoryItems();

		log("current_inventory size is %lld", current_inventory.size());

		for (auto item : current_inventory)
		{
			// only pushback unique items
			if (std::find(unique_item_do_not_drop.begin(), unique_item_do_not_drop.end(), item.ItemId) == unique_item_do_not_drop.end())
			{
				log("Pushing unique item %d.", item.ItemId);
				unique_item_do_not_drop.push_back(item.ItemId);
			}
		}


		auto current_equipments = Inventory::GetEquipmentItems();


		for (auto item : current_equipments)
		{
			// only pushback unique items
			if (std::find(unique_item_do_not_drop.begin(), unique_item_do_not_drop.end(), item.ItemId) == unique_item_do_not_drop.end())
			{
				log("Pushing unique item %d.", item.ItemId);
				unique_item_do_not_drop.push_back(item.ItemId);
			}
		}

		for (auto item : unique_item_do_not_drop)
		{
			log("Unique items to keep: %d", item);
		}
	}

	if (!player._base)
	{
		log("Player is not valid");
		return;
	}

	if (Varpbit::GetPlayerHealth() <= 0)
	{
		// Dead
		Beep(500, 500);
		delete this; spiritualmage = 0;
		return;
	}

	// heal food logic
	if (this->need_heal())
	{
		if (this->have_food())
		{
			this->recover_hp();
			log("Healing...");
			return;
		}
		else
		{
			// TODO tp out logic


			log("Ran out of food, let's kill this mob and get out.");
			Beep(500, 500);
			delete this; spiritualmage = 0;
			return;
		}
	}

	if (Widgets::GetDialogType() == RSDialog::ModernDestroy)
	{
		Common::ConfirmGUI(0x49F0005);
		extraDelay = 800;
		return;
	}

	// alc 1 item
	spiritual_mage::alc_manager();

	// drop up to 3 items
	spiritual_mage::inventory_manager();

	// loot stuff from loot_bag
	spiritual_mage::loot_bag_manager(player);

	auto attackingEnemies = RS::GetInCombatNPCwithMe();

	if (player.isMoving() || player.inCombat() || player.CurrentUsefulAni() != -1)
	{
		log("moving or incombat or not afk");
		return;
	}

	// if we are being attacked, focus the attacker first
	if (attackingEnemies.size() > 0)
	{
		//log("Being attacked");
		player.Attack(attackingEnemies[0]->EntityId);
		return;
	}

	// loot when not in combat / moving
	if (spiritual_mage::loot_manager(player))
	{
		log("Moving to loot");
		return;
	}

	if (to_suicide)
	{
		// drop up to 3 items to make sure our inventory is clean for re-start later
		spiritual_mage::inventory_manager();

		delete this; spiritualmage = 0;
		return;
	}

	// if we are not being attack, get target to attack ourselves
	if (attackingEnemies.size() == 0)
	{
		auto target = RS::GetClosestMonsterNPCByNameFromOrigin("Spiritual mage", Origin, max_distance);

		// if monster is valid, attack it
		if (target)
		{
			//log("monster is valid.");
			player.Attack(target->EntityId);
		}

		return;
	}
}



void spiritual_mage::alc_manager()
{
	auto inventory = Inventory::GetInventoryItemsEx();

	for (int i = 0; i < inventory.size(); i++)
	{
		auto item = inventory[i];

		if (std::find(alc_item_list.begin(), alc_item_list.end(), item.ItemId) != alc_item_list.end())
		{
			Common::cast_high_alchemy();
			Common::select_high_alc_item(i);
			return;
		}
	}
}

void spiritual_mage::loot_bag_manager(Player player)
{
	if (GetTickCount64() - last_loot_time < 500)
		return;

	last_loot_time = GetTickCount64();

	auto loot_bag = Inventory::GetContainerObj(static_cast<uint32_t>(ContainerType::AreaLoot));

	// loot bag is not open
	if (!loot_bag)
		return;
	
	auto wanted_item = spiritual_mage::item_to_loot_from_lootbag();

	log("[ loot ] We found %lld items in lootbag that we want.", wanted_item.size());

	// nothing we want in the bag
	if (wanted_item.size() == 0)
		return;
	
	// loot the first item that we want from looting bag individually
	else if (wanted_item.size() <= 2)
	{
		log("[ loot ] Individually looting slot %d.", wanted_item[0]);
		extraDelay = 400;
		return Common::select_loot_from_lootbag(wanted_item[0]);
	}

	// loot the entire bag
	else
	{
		log("[ loot ] Looting the entire bag.");
		return player.LootAllConfirm();
	}
}

void spiritual_mage::inventory_manager()
{
	auto inventory = Inventory::GetInventoryItemsEx();

	int dropped_count = 0;

	for (int i = 0; i < inventory.size(); i++)
	{
		// only drop 3 items per "tick"
		if (dropped_count > 2)
			break;

		auto item = inventory[i];

		if (item.ItemId != -1)
		{
			// hard code for fire / water talis
			if (item.ItemId == 1445)
				continue;

			if (item.ItemId == 47286)
				continue;

			if (item.ItemId == 47240)
				continue;

			// if item is not in loot_item_list NOR initial inventory list, drop it
			if (std::find(loot_item_list.begin(), loot_item_list.end(), item.ItemId) == loot_item_list.end() &&
				std::find(unique_item_do_not_drop.begin(), unique_item_do_not_drop.end(), item.ItemId) == unique_item_do_not_drop.end())
			{
				Inventory::InteractItemOption(i, 8);
				dropped_count++;
			}
		}
	}

}

bool spiritual_mage::loot_manager(Player player)
{
	auto loots = Tile::GetAllLootsNearbyWithinRadius(Origin, max_distance);

	std::vector<FakeItemEX> wanted_loots;

	for (auto loot : loots)
	{

		// don't try to move to loot gold that are less than 4000
		if (loot.ItemId == 995 && loot.ItemQuantity < 4000)
			continue;

		// if there is a loot item that is on the ground that we want, store it to the list
		if (std::find(loot_item_list.begin(), loot_item_list.end(), loot.ItemId) != loot_item_list.end())
		{
			wanted_loots.push_back(loot);
		}
	}

	// only if 3+ items we want are on the floor
	if (wanted_loots.size() > 0)
	{
		log("[ loot ] Wanted_loot on the ground amount is %lld.", wanted_loots.size());

		FakeItemEX bestLoot = Tile::GetBestLoot(wanted_loots);

		if (bestLoot.ItemId != -1 && bestLoot.ItemQuantity > 0)
			return player.Loot(bestLoot);
	}

	return false;
}

bool spiritual_mage::can_high_alc()
{
	// At least lvl 55 magic
	if (Exp::GetSkillLevel(Stat::MAGIC) >= 55)
	{
		// Have at least 1 Nature rune
		if (Inventory::GetItemById(561) != -1)
		{
			auto fire_rune = Inventory::GetItemByIdEx(554);

			// Have at least 5 Fire runes
			if (fire_rune.ItemQuantity >= 5)
				return true;
		}
	}

	return false;
}

std::vector<uint32_t> spiritual_mage::item_to_loot_from_lootbag()
{
	std::vector<uint32_t> result;

	auto loot_window = Inventory::LootWindowItems();

	for (int i = 0; i < loot_window.size(); i++)
	{
		auto item = loot_window[i];

		if (item.ItemId == -1 || item.ItemQuantity == 0)
			continue;

		// water talisman
		if (item.ItemId == 1445)
			result.push_back(i);

		// salvage
		if (item.ItemId == 47286)
			result.push_back(i);

		// another type of salvage
		if (item.ItemId == 47240)
			result.push_back(i);

		// don't loot bolt less than 10
		if (item.ItemId == 28465 && item.ItemQuantity < 10)
			continue;

		// if item is in loot list, add the slot of that item to the result list
		if (std::find(loot_item_list.begin(), loot_item_list.end(), item.ItemId) != loot_item_list.end())
			result.push_back(i);
	}

	return result;
}


bool spiritual_mage::recover_hp()
{
	for (auto food : food_id)
	{
		auto slot = Inventory::GetItemById(food);

		if (slot != -1)
		{
			Common::InteractWithEquipment(1, slot, 0x5C10007);
			return true;
		}
	}

	return false;
}

bool spiritual_mage::need_heal()
{
	int MaxHeath = Exp::GetSkillLevel(Stat::HITPOINTS) * 100;

	int CurrentHealth = Varpbit::GetPlayerHealth();

	if (((float)CurrentHealth / (float)MaxHeath) < 0.5f)
		return true;

	return false;
}

bool spiritual_mage::have_food()
{
	for (auto food : food_id)
	{
		auto slot = Inventory::GetItemById(food);

		if (slot != -1)
			return true;
	}

	return false;
}