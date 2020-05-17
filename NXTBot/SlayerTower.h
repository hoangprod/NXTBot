#pragma once


struct SlayerContract
{
	SlayerContract() { required_level = 0; };
	SlayerContract(const char* name, int floor, int lvl, Tile2D orig)
	{
		Monster_Name = name;
		floor_int = floor;
		Origin = orig;
		required_level = lvl;
	};
	std::string Monster_Name;
	int required_level;
	int floor_int;
	Tile2D Origin;
};

class SlayerTower
{
public:
	void FSM();

	// to set contract_finished
	void contract_manager(Player player);

	void task_manager();

	void combat_monster(Player player, SlayerContract contract);

	void navigate_floor_1(Player player);

	void navigate_contract(Player player, SlayerContract contract);

	void navigate_bank(Player player);

	SlayerContract get_contract();

	int get_current_floor();

	int get_slayer_contract_index();

	bool is_on_floor(int floor_check);

	bool is_in_tower_area(Player player);

	bool recover_hp();

	bool have_food();

	bool need_heal();

	bool have_contract();

	bool contract_finished = false;

	int previous_hitpoint_xp;
	int previous_slayer_xp;

	int monster_killed_with_hp_tracking = 0;
};
