#pragma once


struct spiritual_mage
{

	void FSM();
	void alc_manager();
	void loot_bag_manager(Player player);
	void inventory_manager();
	bool loot_manager(Player player);


	bool recover_hp();
	bool need_heal();
	bool have_food();

	bool can_high_alc();

	std::vector<uint32_t> item_to_loot_from_lootbag();

	Tile2D Origin;
	std::vector<int> unique_item_do_not_drop;

	uint64_t last_loot_time = 0;
};