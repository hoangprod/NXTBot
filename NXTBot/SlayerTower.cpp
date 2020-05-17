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
#include "Varpbit.h"
#include "SlayerTower.h"

extern int extraDelay;
extern std::vector<std::string> foodlist;

std::vector<int> food_id = { 385, 1901, 1899, 1897, 329 , 333, 40295, 40297, 397, 42251 };

std::vector<SlayerContract> contracts ={ SlayerContract("Crawling hand", 1, 5, Tile2D(3404, 3561)), SlayerContract("Banshee", 1, 15, Tile2D(3444, 3557)), SlayerContract("Infernal Mage", 2, 45, Tile2D(3410, 3565)),
SlayerContract("Bloodveld", 2, 50, Tile2D(3409, 3545)), SlayerContract("Aberrant spectre", 2, 60, Tile2D(344, 3574)), SlayerContract("Gargoyle", 3, 75, Tile2D(3441, 3550)), SlayerContract("Nechryael", 3, 80, Tile2D(3405, 3571)) };


/*
---- Floor system

Tower have 4 floors, including the "base" floor outside. 
Base   : 3500.0f
Floor 1: 3600.0f
Floor 2: 5800.0f
Floor 3: 7900.0f

*/

void SlayerTower::FSM()
{
	Player player = RS::GetLocalPlayer();

	if (!player._base)
		return;

	task_manager();

	// heal food logic
	if (this->need_heal())
	{
		if (this->have_food())
		{
			this->recover_hp();
			return;
		}
		else
		{
			log("Ran out of food, let's kill this mob and get out.");
		}
	}

	if (player.isMoving() || player.inCombat())
		return;

	if (Exp::GetSkillLevel(Stat::SLAYER) < 5)
		return;

	// manage all food / bank related
	navigate_bank(player);

	// If we don't have contract, contract_manager will get us the contract
	if (contract_finished || !SlayerTower::have_contract())
	{
		return contract_manager(player);
	}
	
	// If we DO have the contract, go fight some monster
	else
	{
		SlayerContract contract = get_contract();

		// Impossible but still, just check if we somehow are no longer able to do even a lvl 5 contract
		if (!contract.required_level > Exp::GetSkillLevel(Stat::SLAYER))
			return;

		if (is_on_floor(contract.floor_int))
		{
			//log("Is on the right floor, fighting monster.");

			this->combat_monster(player, contract);
			return;
		}

		else
		{
			//log("Not on the right contract floor, navitgating there.");
			return navigate_contract(player, contract);
		}
	}

}

void SlayerTower::contract_manager(Player player)
{
	if (!SlayerTower::is_in_tower_area(player))
		return;


	if (Widgets::GetUIType() == UIType::SLAYER_CONTRACT)
	{
		contract_finished = false;

		int contract_index = SlayerTower::get_slayer_contract_index();

		if (!contract_index)
			return;

		// Select contract type
		Common::InteractWithEquipment(1, contract_index, 91357186);

		// Confirm contract (hacky)
		Common::InteractWithEquipment(1, contract_index, 91357197);

		return;
	}


	else if (Widgets::GetDialogType() == RSDialog::NPCDialog)
	{
		contract_finished = false;
		return;
	}

	else if (Widgets::GetDialogType() == RSDialog::MultiSelectionDialog)
	{
		contract_finished = false;

		// Select cash
		Common::ConfirmGUI(77856781);
		return;
	}


	int current_floor = SlayerTower::get_current_floor();

	// Go talk to Manager
	if (current_floor == 1 || current_floor == 0)
	{
		auto Markus = RS::GetEntityNPCByName("Markus");

		if (!Markus)
		{
			log("contract_manager can't find manager.");
			return;
		}

		Common::DepositActionNPC(Markus->EntityId);
		return;
	}

	// if not on base floor / outside, go to base floor
	else
	{
		return navigate_floor_1(player);
	}

}

void SlayerTower::task_manager()
{
	if (!contract_finished)
	{
		// means we killed something
		if (Exp::GetCurrentExp(Stat::HITPOINTS) > previous_hitpoint_xp)
		{
			monster_killed_with_hp_tracking++;
		}

		if (Exp::GetCurrentExp(Stat::SLAYER) > previous_slayer_xp)
		{
			monster_killed_with_hp_tracking = 0;
		}

		// if we killed X monsters with no change in slayer xp, it means contract is finished
		if (monster_killed_with_hp_tracking > 5)
		{
			monster_killed_with_hp_tracking = 0;
			contract_finished = true;
		}

		// update these
		previous_hitpoint_xp = Exp::GetCurrentExp(Stat::HITPOINTS);
		previous_slayer_xp = Exp::GetCurrentExp(Stat::SLAYER);
	}
}

void SlayerTower::combat_monster(Player player, SlayerContract contract)
{
	if (!is_on_floor(contract.floor_int))
	{
		// log("Not on the right floor for %s. %d : %d", contract.Monster_Name.data(), get_current_floor(), contract.floor_int);
		return;
	}

	// we are not too far from the origin point
	if (RS::GetDistance(player.GetTilePosition(), contract.Origin) > 7.0f)
	{
		player.MoveRandom(contract.Origin, 2, 2);
		return;
	}

	else
	{
		auto attackingEnemies = RS::GetInCombatNPCwithMe();

		// if we are being attacked, focus the attacker first
		if (attackingEnemies.size() > 0)
		{
			player.Attack(attackingEnemies[0]->EntityId);
			return;
		}

		// if we are not being attack, get target to attack ourselves
		else
		{
			auto target = RS::GetClosestMonsterNPCByNameFromOrigin(contract.Monster_Name.data(), contract.Origin, 10.0f);

			// if monster is valid, attack it
			if (target)
			{
				player.Attack(target->EntityId);
			}

			return;
		}
	}
}

void SlayerTower::navigate_floor_1(Player player)
{
	if (SlayerTower::is_on_floor(1))
		return;

	int cur_floor = SlayerTower::get_current_floor();

	Tile2D player_pos = player.GetTilePosition();

	// if on roof
	if (cur_floor == 3)
	{
		// EAST Stair down
		if (player_pos.x >= 3432)
		{
			auto top_stair = Static::GetClosestStaticObjectByNameWithOptionAtPosition("Staircase", "Climb bottom floor", Tile2D(3445, 3573), false);

			if (!top_stair.Definition)
			{
				log("[ bot ] Cannot find top floor stair.");
				return;
			}

			Common::StaticInteract2(top_stair);
			return;
		}

		// West side stair down
		else
		{
			auto top_stair = Static::GetClosestStaticObjectByNameWithOptionAtPosition("Staircase", "Climb bottom floor", Tile2D(3395, 3573), false);

			if (!top_stair.Definition)
			{
				log("[ bot ] Cannot find top floor stair.");
				return;
			}

			Common::StaticInteract2(top_stair);
			return;

		}

	}

	// 
	else if (cur_floor == 2)
	{
		// EAST Stair down
		if (player_pos.x >= 3432)
		{
			auto top_stair = Static::GetClosestStaticObjectByNameWithOptionAtPosition("Staircase", "Climb", Tile2D(3445, 3573), false);

			if (!top_stair.Definition)
			{
				log("[ bot ] Cannot find top floor stair.");
				return;
			}

			Common::StaticInteract2(top_stair);
			return;
		}

		// West side stair down
		else
		{
			auto top_stair = Static::GetClosestStaticObjectByNameWithOptionAtPosition("Staircase", "Climb", Tile2D(3395, 3573), false);

			if (!top_stair.Definition)
			{
				log("[ bot ] Cannot find top floor stair.");
				return;
			}

			Common::StaticInteract2(top_stair);
			return;

		}
	}
	else
	{
		// we are either fucking in the sky, or already on / below stair 1
		return;
	}
}

void SlayerTower::navigate_contract(Player player, SlayerContract contract)
{
	int cur_floor = SlayerTower::get_current_floor();

	Tile2D player_pos = player.GetTilePosition();

	if (cur_floor == 0)
	{
		// Crawling hand
		if (contract.required_level == 5)
		{
			player.MoveRandom(contract.Origin, 2, 2);
			return;
		}
		// Banshee
		else if (contract.required_level == 15)
		{
			player.MoveRandom(contract.Origin, 2, 2);
			return;
		}
		// Infernal Mage
		else if (contract.required_level == 45)
		{
			auto bottom_stair_sw = Static::GetClosestStaticObjectByNameWithOptionAtPosition("Staircase", "Climb top floor", Tile2D(3395, 3536), false);

			if (bottom_stair_sw.Definition)
				Common::StaticInteract(bottom_stair_sw);

			return;
		}
		// Bloodveld
		else if (contract.required_level == 50)
		{
			auto bottom_stair_sw = Static::GetClosestStaticObjectByNameWithOptionAtPosition("Staircase", "Climb top floor", Tile2D(3395, 3536), false);

			if (bottom_stair_sw.Definition)
				Common::StaticInteract(bottom_stair_sw);

			return;
		}
		// Aberrant spectre
		else if (contract.required_level == 60)
		{
			auto bottom_stair_ne = Static::GetClosestStaticObjectByNameWithOptionAtPosition("Staircase", "Climb top floor", Tile2D(3445, 3573), false);

			if (bottom_stair_ne.Definition)
				Common::StaticInteract(bottom_stair_ne);

			return;
		}
		// Gargoyle
		else if (contract.required_level == 75)
		{
			auto bottom_stair_se = Static::GetClosestStaticObjectByNameWithOptionAtPosition("Staircase", "Climb top floor", Tile2D(3445, 3536), false);

			if (bottom_stair_se.Definition)
				Common::StaticInteract2(bottom_stair_se);

			return;
		}
		// Nechryael
		else if (contract.required_level == 80)
		{
			auto bottom_stair_nw = Static::GetClosestStaticObjectByNameWithOptionAtPosition("Staircase", "Climb top floor", Tile2D(3395, 3573), false);

			if (bottom_stair_nw.Definition)
				Common::StaticInteract2(bottom_stair_nw);

			return;
		}
		else
		{
			log("[ bot ] Fighting a monster that does not exist");
			return;
		}
	}
	else if (cur_floor == 1)
	{
		// Crawling hand
		if (contract.required_level == 5)
		{
			player.MoveRandom(contract.Origin, 2, 2);
			return;
		}
		// Banshee
		else if (contract.required_level == 15)
		{
			player.MoveRandom(contract.Origin, 2, 2);
			return;
		}
		// Infernal Mage
		else if (contract.required_level == 45)
		{
			auto bottom_stair_sw = Static::GetClosestStaticObjectByNameWithOptionAtPosition("Staircase", "Climb top floor", Tile2D(3395, 3536), false);

			if (bottom_stair_sw.Definition)
				Common::StaticInteract(bottom_stair_sw);

			return;
		}
		// Bloodveld
		else if (contract.required_level == 50)
		{
			auto bottom_stair_sw = Static::GetClosestStaticObjectByNameWithOptionAtPosition("Staircase", "Climb top floor", Tile2D(3395, 3536), false);

			if (bottom_stair_sw.Definition)
				Common::StaticInteract(bottom_stair_sw);

			return;
		}
		// Aberrant spectre
		else if (contract.required_level == 60)
		{
			auto bottom_stair_ne = Static::GetClosestStaticObjectByNameWithOptionAtPosition("Staircase", "Climb top floor", Tile2D(3445, 3573), false);

			if (bottom_stair_ne.Definition)
				Common::StaticInteract(bottom_stair_ne);

			return;
		}
		// Gargoyle
		else if (contract.required_level == 75)
		{
			auto bottom_stair_se = Static::GetClosestStaticObjectByNameWithOptionAtPosition("Staircase", "Climb top floor", Tile2D(3445, 3536), false);

			if (bottom_stair_se.Definition)
				Common::StaticInteract2(bottom_stair_se);

			return;
		}
		// Nechryael
		else if (contract.required_level == 80)
		{
			auto bottom_stair_nw = Static::GetClosestStaticObjectByNameWithOptionAtPosition("Staircase", "Climb top floor", Tile2D(3395, 3573), false);

			if (bottom_stair_nw.Definition)
				Common::StaticInteract2(bottom_stair_nw);

			return;
		}
		else
		{
			log("[ bot ] Fighting a monster that does not exist");
			return;
		}
	}
	else if (cur_floor == 2)
	{
		// Crawling hand
		if (contract.required_level == 5)
		{
			// is not in Spectre's room
			if (player_pos.x <= 3431 && player_pos.y <= 3546)
			{
				auto middle_stair_sw = Static::GetClosestStaticObjectByNameWithOptionAtPosition("Staircase", "Climb", Tile2D(3395, 3573), false);

				if (middle_stair_sw.Definition)
					Common::StaticInteract3(middle_stair_sw);
				
				return;
			}

			// is in Spectre's room
			else
			{
				auto middle_stair_ne = Static::GetClosestStaticObjectByNameWithOptionAtPosition("Staircase", "Climb", Tile2D(3445, 3573), false);

				if (middle_stair_ne.Definition)
					Common::StaticInteract3(middle_stair_ne);

				return;
			}
		}
		// Banshee
		else if (contract.required_level == 15)
		{
			// is not in Spectre's room
			if (player_pos.x <= 3431 && player_pos.y <= 3546)
			{
				auto middle_stair_se = Static::GetClosestStaticObjectByNameWithOptionAtPosition("Staircase", "Climb", Tile2D(3445, 3536), false);

				if (middle_stair_se.Definition)
					Common::StaticInteract3(middle_stair_se);

				return;
			}
			// is in Spectre's room
			else
			{
				auto middle_stair_ne = Static::GetClosestStaticObjectByNameWithOptionAtPosition("Staircase", "Climb", Tile2D(3445, 3573), false);

				if (middle_stair_ne.Definition)
					Common::StaticInteract3(middle_stair_ne);

				return;
			}

		}
		// Infernal Mage
		else if (contract.required_level == 45)
		{
			player.MoveRandom(contract.Origin, 2, 2);
			return;
		}
		// Bloodveld
		else if (contract.required_level == 50)
		{
			player.MoveRandom(contract.Origin, 2, 2);
			return;
		}
		// Aberrant spectre
		else if (contract.required_level == 60)
		{
			// is not in Spectre's room
			if (player_pos.x <= 3431 && player_pos.y <= 3546)
			{
				auto closest_middle_stair = Static::GetClosestStaticObjectByNameWithOptionAtPosition("Staircase", "Climb", Tile2D(3445, 3573), true);

				if (closest_middle_stair.Definition)
					Common::StaticInteract3(closest_middle_stair);

				return;
			}

			else
			{
				player.MoveRandom(contract.Origin, 2, 2);
				return;
			}
		}
		// Gargoyle
		else if (contract.required_level == 75)
		{
			// is not in Spectre's room
			if (player_pos.x <= 3431 && player_pos.y <= 3546)
			{
				auto middle_stair_se = Static::GetClosestStaticObjectByNameWithOptionAtPosition("Staircase", "Climb", Tile2D(3445, 3536), false);

				if (middle_stair_se.Definition)
					Common::StaticInteract2(middle_stair_se);

				return;
			}

			// is in spectre's room
			else
			{
				auto middle_stair_ne = Static::GetClosestStaticObjectByNameWithOptionAtPosition("Staircase", "Climb", Tile2D(3445, 3573), false);

				if (middle_stair_ne.Definition)
					Common::StaticInteract2(middle_stair_ne);

				return;
			}
		}
		// Nechryael
		else if (contract.required_level == 80)
		{
			// is not in Spectre's room
			if (player_pos.x <= 3431 && player_pos.y <= 3546)
			{
				auto middle_stair_nw = Static::GetClosestStaticObjectByNameWithOptionAtPosition("Staircase", "Climb", Tile2D(3395, 3573), false);

				if (middle_stair_nw.Definition)
					Common::StaticInteract2(middle_stair_nw);

				return;
			}

			// is in spectre's room
			else
			{
				auto middle_stair_ne = Static::GetClosestStaticObjectByNameWithOptionAtPosition("Staircase", "Climb", Tile2D(3445, 3573), false);

				if (middle_stair_ne.Definition)
					Common::StaticInteract3(middle_stair_ne);

				return;
			}
		}
		else
		{
			log("[ bot ] Fighting a monster that does not exist");
			return;
		}
	}
	else if (cur_floor == 3)
	{
		// Crawling hand
		if (contract.required_level == 5)
		{
			// is not in Gargoyle room
			if (player_pos.x <= 3431)
			{
				auto top_stair_nw = Static::GetClosestStaticObjectByNameWithOptionAtPosition("Staircase", "Climb bottom floor", Tile2D(3395, 3573), false);

				if (top_stair_nw.Definition)
					Common::StaticInteract2(top_stair_nw);

				return;
			}
			else
			{
				auto top_stair_ne = Static::GetClosestStaticObjectByNameWithOptionAtPosition("Staircase", "Climb bottom floor", Tile2D(3445, 3573), false);

				if (top_stair_ne.Definition)
					Common::StaticInteract2(top_stair_ne);

				return;
			}
		}
		// Banshee
		else if (contract.required_level == 15)
		{
			// is not in Gargoyle room
			if (player_pos.x <= 3431)
			{
				auto top_stair_nw = Static::GetClosestStaticObjectByNameWithOptionAtPosition("Staircase", "Climb bottom floor", Tile2D(3395, 3573), false);

				if (top_stair_nw.Definition)
					Common::StaticInteract2(top_stair_nw);

				return;
			}
			else
			{
				auto top_stair_se = Static::GetClosestStaticObjectByNameWithOptionAtPosition("Staircase", "Climb bottom floor", Tile2D(3445, 3536), false);

				if (top_stair_se.Definition)
					Common::StaticInteract2(top_stair_se);

				return;
			}
		}
		// Infernal Mage
		else if (contract.required_level == 45)
		{
			// is not in Gargoyle room
			if (player_pos.x <= 3431)
			{
				auto top_stair_nw = Static::GetClosestStaticObjectByNameWithOptionAtPosition("Staircase", "Climb bottom floor", Tile2D(3395, 3573), false);

				if (top_stair_nw.Definition)
					Common::StaticInteract(top_stair_nw);

				return;
			}

			else
			{
				auto top_stair_ne = Static::GetClosestStaticObjectByNameWithOptionAtPosition("Staircase", "Climb bottom floor", Tile2D(3445, 3573), false);

				if (top_stair_ne.Definition)
					Common::StaticInteract2(top_stair_ne);

				return;
			}
		}
		// Bloodveld
		else if (contract.required_level == 50)
		{
			// is not in Gargoyle room
			if (player_pos.x <= 3431)
			{
				auto top_stair_nw = Static::GetClosestStaticObjectByNameWithOptionAtPosition("Staircase", "Climb bottom floor", Tile2D(3395, 3573), false);

				if (top_stair_nw.Definition)
					Common::StaticInteract(top_stair_nw);

				return;
			}

			else
			{
				auto top_stair_ne = Static::GetClosestStaticObjectByNameWithOptionAtPosition("Staircase", "Climb bottom floor", Tile2D(3445, 3573), false);

				if (top_stair_ne.Definition)
					Common::StaticInteract2(top_stair_ne);

				return;
			}
		}
		// Aberrant spectre
		else if (contract.required_level == 60)
		{
			// is not in Gargoyle room
			if (player_pos.x <= 3431)
			{
				auto top_stair_nw = Static::GetClosestStaticObjectByNameWithOptionAtPosition("Staircase", "Climb bottom floor", Tile2D(3395, 3573), false);

				if (top_stair_nw.Definition)
					Common::StaticInteract2(top_stair_nw);

				return;
			}
			else
			{
				auto top_stair_ne = Static::GetClosestStaticObjectByNameWithOptionAtPosition("Staircase", "Climb bottom floor", Tile2D(3445, 3573), false);

				if (top_stair_ne.Definition)
					Common::StaticInteract(top_stair_ne);

				return;
			}
		}
		// Gargoyle
		else if (contract.required_level == 75)
		{
			// is not in Gargoyle room
			if (player_pos.x <= 3431)
			{
				auto top_stair_ne = Static::GetClosestStaticObjectByNameWithOptionAtPosition("Staircase", "Climb bottom floor", Tile2D(3445, 3573), false);

				if (top_stair_ne.Definition)
					Common::StaticInteract2(top_stair_ne);

				return;
			}
			else
			{
				player.MoveRandom(contract.Origin, 2, 2);
				return;
			}
		}
		// Nechryael
		else if (contract.required_level == 80)
		{
			// is not in Gargoyle room
			if (player_pos.x <= 3431)
			{
				player.MoveRandom(contract.Origin, 2, 2);
				return;
			}
			else
			{
				auto top_stair_nw = Static::GetClosestStaticObjectByNameWithOptionAtPosition("Staircase", "Climb bottom floor", Tile2D(3395, 3573), false);

				if (top_stair_nw.Definition)
					Common::StaticInteract2(top_stair_nw);

				return;
			}
		}
		else
		{
			log("[ bot ] Fighting a monster that does not exist");
			return;
		}
	}
	else
	{
		log("[ bot ] We better be in the fucking sky right now.");
		return;
	}

}

void SlayerTower::navigate_bank(Player player)
{
	if (this->have_food())
	{
		// if have food and still in tower, does not matter to us and we should go on and kill stuff
		if (this->is_in_tower_area(player))
		{
			return;
		}

		// if we have food but we are not in tower area, it means we came from a bank so go back to tower
		else
		{
			auto banker = RS::GetClosestEntityNPCByName("Banker");
			auto Markus = RS::GetClosestEntityNPCByName("Markus");

			// we can see banker, meaning we JUST got it from the banker
			if (banker)
			{
				log("going to markus closest spot.");
				// Go to Markus' near spot
				player.MoveRandom(Tile2D(3457, 3500), 3, 3);
				return;
			}

			// if we can see Markus, it means we have food, and finally got back to tower. Go to the beginning of tower stair to start
			else if (Markus)
			{
				log("going to stair");
				// Go to stair of tower
				player.MoveRandom(Tile2D(3423, 3524), 2, 2);
				return;
			}

			// if we have food and we can't see neither banker nor Markus, it mean we got stopped midway. Meaning let's go to markus
			else
			{
				log("going to markus spot. outside tower");
				// Go to Markus' near spot
				player.MoveRandom(Tile2D(3445, 3503), 3, 3);
				return;
			}
		}

	} 

	// if we don't have food
	else
	{
		// Use bankload preset 4 for food / weapon
		if (Inventory::isBankOpened())
		{
			Common::BankLoadPreset(5);
			return;
		}

		// we are still in the tower
		if (this->is_in_tower_area(player))
		{
			int cur_floor = SlayerTower::get_current_floor();

			// make sure we are on the bottom floor
			if (cur_floor > 1)
				return navigate_floor_1(player);

			// we are currently on the first floor
			if (cur_floor == 1)
			{
				// this should put us to right outside tower aka floor 0
				log("1. put us right outside.");
				player.MoveRandom(Tile2D(3445, 3503), 3, 3);
				return;
			}

			else if (cur_floor == 0)
			{
				log("2. put us near bank.");

				// this should put us near bank and outside of tower area
				player.MoveRandom(Tile2D(3491, 3484), 3, 3);
				return;
			}
		}

		// if we don't have food AND not in tower meaning we are on our way to get food from bank
		else
		{
			auto banker = RS::GetClosestEntityNPCByName("Banker");
			auto Markus = RS::GetClosestEntityNPCByName("Markus");

			// if we can see banker, it means we are at least already inside the town
			if (banker)
			{
				// if we don't have food and see banker, then go to bank interface
				Common::BankUsingNPC(banker->EntityId);
				return;
			}


			// if we can still see markus, it means we didn't get that far from the tower yet
			if (Markus)
			{
				log("should put us nearbnk and outside of tower area");
				// this should put us near bank and outside of tower area
				player.MoveRandom(Tile2D(3491, 3484), 3, 3);
				return;
			}

			// if we don't have food and we can't see Markus nor Banker, it means we were on our way to Banker and got stuck inbetween, go to banker town
			else
			{
				log("2 should put us nearbnk and outside of tower area");

				// this should put us near bank and outside of tower area
				player.MoveRandom(Tile2D(3491, 3484), 3, 3);
				return;
			}

		}


	}


}

SlayerContract SlayerTower::get_contract()
{
	int slayer_level = Exp::GetSkillLevel(Stat::SLAYER);

	for (uint64_t i = 0; i < contracts.size(); i++)
	{
		SlayerContract contract = contracts[i];

		if (slayer_level >= contract.required_level)
			continue;
		else
		{
			// Impossible
			if (i == 0)
				return SlayerContract();

			int cur_contract_index = i - 1;

			// banshee
			if (cur_contract_index == 1)
			{
				int helmet_slot = Inventory::GetContainerObj(static_cast<uint32_t>(ContainerType::Equipment))->ContainerContent[0].ItemId;

				if (helmet_slot != 4166)
				{
					int earsmuff = Inventory::GetItemById(4166);

					if (earsmuff != -1)
					{
						Inventory::InteractItemOption(earsmuff, 2);
						cur_contract_index = 1;
					}
					else
						cur_contract_index = 0;
				}
			}

			// Spectre
			else if (cur_contract_index == 4)
			{
				int helmet_slot = Inventory::GetContainerObj(static_cast<uint32_t>(ContainerType::Equipment))->ContainerContent[0].ItemId;

				if (helmet_slot != 4168)
				{
					int nose_peg = Inventory::GetItemById(4168);

					if (nose_peg != -1)
					{
						Inventory::InteractItemOption(nose_peg, 2);
						cur_contract_index = 4;
					}
					else
						cur_contract_index = 3;
				}


			}

			return contracts[cur_contract_index];
		}
	}
}

int SlayerTower::get_current_floor()
{
	/*
	---- Floor system

	Tower have 4 floors, including the "base" floor outside.
	Base   : 3500.0f
	Floor 1: 3600.0f
	Floor 2: 5800.0f
	Floor 3: 7900.0f

	*/

	float player_z = RS::GetLocalPlayerPos()[1];

	// If higher than floor 2, must be on floor 3
	if (player_z > 5800.0f)
		return 3;

	// If not higher than floor 2, but higher than floor 1, then must be on floor 2
	else if (player_z > 3600.0f)
		return 2;

	// if higher than base floor, must be on floor 1
	else if (player_z > 3500.0f)
		return 1;

	// if higher than none, base floor it is.
	return 0;
}

int SlayerTower::get_slayer_contract_index()
{
	int slayer_level = Exp::GetSkillLevel(Stat::SLAYER);

	if (slayer_level >= 80)
		return 13;
	else if (slayer_level >= 75)
		return 11;
	else if (slayer_level >= 60)
	{
		// Abberant Spectre
		int helmet_slot = Inventory::GetContainerObj(static_cast<uint32_t>(ContainerType::Equipment))->ContainerContent[0].ItemId;

		// Nose peg
		if (helmet_slot == 4168)
			return 9;

		int nose_peg = Inventory::GetItemById(4168);

		if (nose_peg != -1)
		{
			Inventory::InteractItemOption(nose_peg, 2);
			return 9;
		}

		return 7;
	}
	else if (slayer_level >= 50)
		return 7;
	else if (slayer_level >= 45)
		return 5;
	else if (slayer_level >= 15)
	{
		// Banshee
		int helmet_slot = Inventory::GetContainerObj(static_cast<uint32_t>(ContainerType::Equipment))->ContainerContent[0].ItemId;

		// Earsmuff
		if(helmet_slot == 4166)
			return 3;

		int earsmuff = Inventory::GetItemById(4166);

		if (earsmuff != -1)
		{
			Inventory::InteractItemOption(earsmuff, 2);
			return 3;
		}

		return 1;
	}
	else if (slayer_level >= 5)
		return 1;
	return 0;
}

bool SlayerTower::is_on_floor(int floor_check)
{
	if (SlayerTower::get_current_floor() == floor_check)
		return true;

	return false;
}

bool SlayerTower::is_in_tower_area(Player player)
{
	Tile2D player_pos = player.GetTilePosition();

	/*
	Tower's 4 corners

	(3380, 3579)           (3455, 3579)

    (3380, 3518)           (3455, 3518)

	*/

	if (player_pos.x >= 3380 && player_pos.x <= 3455 &&
		player_pos.y >= 3518 && player_pos.y <= 3579)
		return true;

	return false;
}

bool SlayerTower::have_contract()
{
	if (Inventory::GetItemById(27360) == -1)
	{
		contract_finished = false;
		return false;
	}

	return true;
}

bool SlayerTower::recover_hp()
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

bool SlayerTower::need_heal()
{
	int MaxHeath = Exp::GetSkillLevel(Stat::HITPOINTS) * 100;

	int CurrentHealth = Varpbit::GetPlayerHealth();

	if (((float)CurrentHealth / (float)MaxHeath) < 0.5f)
		return true;

	return false;
}

bool SlayerTower::have_food()
{
	for (auto food : food_id)
	{
		auto slot = Inventory::GetItemById(food);

		if (slot != -1)
			return true;
	}

	return false;
}