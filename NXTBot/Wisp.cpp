#include "pch.h"
#include "GameClasses.h"
#include "Patterns.h"
#include "Inventory.h"
#include "Game.h"
#include "PlayableEntity.h"
#include "Tile.h"
#include "Widgets.h"
#include "Experience.h"
#include "Wisp.h"


extern std::string botStatus;

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

		player->DepositActionNPC(MyrtleId);
		return;
	}
	else
	{
		if (Widgets::GetWidgetUI(CONVERSATION_WIDGET))
		{
			botStatus = "Clicking through chat option";
			player->ConfirmChat();
		}
		else if (Widgets::GetWidgetUI(DEPOSIT_WIDGET))
		{
			botStatus = "Clicking deposit all items";
			player->DepositAll();
		}

		return;
	}
}

void Wisp::Looting(FakeItemEX loot)
{
	auto areaLoot = Inventory::GetContainerObj(static_cast<uint32_t>(ContainerType::AreaLoot));

	if (areaLoot)
		player->LootAllConfirm();
	else if (loot.ItemQuantity != 0)
		player->Loot(loot);
}



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

}

bool GeneralCombat::NeedHeal()
{
	return false;
}

void GeneralCombat::FSM()
{
	if (!player || !player->_base)
	{
		monsterTargetName = RS::GetClosestMonster()->Name;
		player = new Player(RS::GetLocalPlayer());
		origin = player->GetTilePosition();

		//printf("[+] Player %s will be targeting %s from tile position (%d, %d)\n", player->GetName().data(), monsterTargetName.data(), origin.x, origin.y);
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

	auto lootsAvailable = Tile::GetAllLootsNearbyWithinRadius(origin, 17.0f);

	for (auto loot : lootsAvailable)
	{
		if (loot.ItemId == 12158 || loot.ItemId == 12159 || loot.ItemId == 12160 || loot.ItemId == 12161 || loot.ItemId == 12163 || loot.ItemId == 30139 || loot.ItemId == 30140 || loot.ItemId == 30141)
		{
			player->Loot(loot);
			return;
		}
	}

	auto attackingEnemies = RS::GetInCombatNPCwithMe();

	if (attackingEnemies.size() > 0)
	{
		botStatus = "Attacking Monster";

		// Attack the first monster in the list that is attacking us
		return Combat(attackingEnemies[0]);
	}


	EntityObj* EnemyMonsters = RS::GetMonsterWithinRadiusWithName(monsterTargetName.data(), origin, 15.0f);

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




// RABBIT =========================================




void Rabbit::FSM()
{
	if (!player || !player->_base)
	{
		player = new Player(RS::GetLocalPlayer());
		origin = player->GetTilePosition();
	}

	// If I am moving around or targeting something, do not fuck with it
	if (player->isMoving() || (player->bTargeting() && player->CurrentTarget() != 25688)) // Hardcode bank id
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


	// If there are a monster 15 tiles away from origin, fight it
	if (EnemyMonsters)
	{
		botStatus = "Fight monster";

		return Rabbit::Combat(EnemyMonsters);
	}

	// If not in combat, not being attacked, not full inventory and not have anything to loot
	botStatus = "Doing nothing, shouldn't really be here";
}

void Rabbit::Combat(EntityObj* Enemies)
{
	player->Attack(Enemies->EntityId);
	return;
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
		player->BankUsingNPC(banker->EntityId); // Hardcode that bank of Gielinor
		return;
	}
	else
	{
		if (Widgets::GetWidgetUI(BANKING_WIDGET))
		{
			botStatus = "Depositing all inventory to bank";
			player->DepositAllThroughBank();
		}
		return;
	}
}

void Rabbit::Looting(FakeItemEX loot)
{
	auto areaLoot = Inventory::GetContainerObj(static_cast<uint32_t>(ContainerType::AreaLoot));

	if (areaLoot)
	{
		player->LootAllConfirm();
		return;
	}
	else if (loot.ItemQuantity != 0)
	{
		player->Loot(loot);
		return;
	}
}

void GenMining::FSM()
{
	if (!player || !player->_base)
	{
		player = new Player(RS::GetLocalPlayer());
		MiningExp = Exp::GetCurrentExp(SkillType::MINING);

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
		player->StaticInteract(Node);
		return;
	}

	int CurrentExp = Exp::GetCurrentExp(SkillType::MINING);

	if (CurrentExp > MiningExp)
	{
		botStatus = "Clicking on Mining node";
		player->StaticInteract(Node);
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
		player->BankUsingNPC(mon->EntityId);

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
		player->StaticInteract(Bike);
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
			player->StaticInteract(Bike);
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


std::vector<AgilityCourse> AnachroniaAgi = { AgilityCourse(113687, Tile2D(5414, 2324)), AgilityCourse(113688, Tile2D(5410, 2325)), AgilityCourse(113689, Tile2D(5408, 2323)), AgilityCourse(113690, Tile2D(5393, 2320)), AgilityCourse(113691, Tile2D(5367, 2304)), AgilityCourse(113692, Tile2D(5369, 2282)), AgilityCourse(113693, Tile2D(5376, 2247)), AgilityCourse(113694, Tile2D(5397, 2240)), AgilityCourse(113695, Tile2D(5439, 2217)), AgilityCourse(113696, Tile2D(5456, 2179)), AgilityCourse(113697, Tile2D(5475, 2171)), AgilityCourse(113698, Tile2D(5489, 2171)), AgilityCourse(113699, Tile2D(5502, 2171)), AgilityCourse(113700, Tile2D(5527, 2182)), AgilityCourse(113701, Tile2D(5548, 2220)), AgilityCourse(113702, Tile2D(5548, 2244)), AgilityCourse(113703, Tile2D(5553, 2249)), AgilityCourse(113704, Tile2D(5565, 2272)), AgilityCourse(113705, Tile2D(5578, 2289)), AgilityCourse(113706, Tile2D(5587, 2295)), AgilityCourse(113707, Tile2D(5596, 2295)), AgilityCourse(113708, Tile2D(5629, 2287)), AgilityCourse(113709, Tile2D(5669, 2288)), AgilityCourse(113710, Tile2D(5680, 2290)), AgilityCourse(113711, Tile2D(5684, 2293)), AgilityCourse(113712, Tile2D(5686, 2310)), AgilityCourse(113713, Tile2D(5695, 2317)), AgilityCourse(113714, Tile2D(5696, 2346)), AgilityCourse(113715, Tile2D(5675, 2363)), AgilityCourse(113716, Tile2D(5655, 2377)), AgilityCourse(113717, Tile2D(5653, 2405)), AgilityCourse(113718, Tile2D(5643, 2420)), AgilityCourse(113719, Tile2D(5642, 2431)), AgilityCourse(113720, Tile2D(5626, 2433)), AgilityCourse(113721, Tile2D(5616, 2433)), AgilityCourse(113722, Tile2D(5608, 2433)), AgilityCourse(113723, Tile2D(5601, 2433)), AgilityCourse(113724, Tile2D(5591, 2450)), AgilityCourse(113725, Tile2D(5584, 2452)), AgilityCourse(113726, Tile2D(5574, 2453)), AgilityCourse(113727, Tile2D(5564, 2452)), AgilityCourse(113728, Tile2D(5536, 2492)), AgilityCourse(113729, Tile2D(5528, 2492)), AgilityCourse(113730, Tile2D(5505, 2478)), AgilityCourse(113731, Tile2D(5505, 2468)), AgilityCourse(113732, Tile2D(5505, 2462)), AgilityCourse(113733, Tile2D(5484, 2456)), AgilityCourse(113734, Tile2D(5431, 2417)), AgilityCourse(113735, Tile2D(5431, 2407)), AgilityCourse(113736, Tile2D(5425, 2397)), AgilityCourse(113737, Tile2D(5426, 2387)), AgilityCourse(113738, Tile2D(5428, 2383))};

void MoneyAgi::FSM()
{
	if (!player || !player->_base)
	{
		player = new Player(RS::GetLocalPlayer());
		botStatus = "Money Agi initiated.";
	}


	if (player->isMoving())
		return;

	if (RS::GetDistance(player->GetTilePosition(), Tile2D(5418, 2348)) < 7.0f)
	{
		//botStatus = "Going to start of course with id" + std::to_string(AnachroniaAgi[0].objId);
		auto obstacle = Static::GetCStaticObjectById(AnachroniaAgi[0].objId);

		player->StaticInteract(obstacle);

		return;
	}

	auto next = MoneyAgi::GetNextCourse();

	if (next.objId < 2)
	{
		return;
	}

	auto obstacle = Static::GetCStaticObjectById(next.objId);

	if (obstacle.Definition)
	{
		currentObstacle = obstacle.Definition->Id;
		botStatus = "Clicking on next obstacle";
		player->StaticInteract(obstacle);
	}
	else
	{
		botStatus = "Could not find the next obstacle. Maybe you are doing a course?";
	}
}

AgilityCourse MoneyAgi::GetNextCourse()
{
	auto playerPos = player->GetTilePosition();

	auto lastCourse = AnachroniaAgi[AnachroniaAgi.size() - 1];


	if (playerPos.x == lastCourse.EndPos.x && playerPos.y == lastCourse.EndPos.y)
	{
		//botStatus = "Starting the course again :D";
		player->Move(Tile2D(rand() % 2 + 5417, rand() % 5 + 2344));
		return AgilityCourse();
	}


	for (size_t i = 0; i < AnachroniaAgi.size(); i++)
	{
		auto obstacle = AnachroniaAgi[i];

		// If there is a match of end pos and player pos
		if (obstacle.EndPos.x == playerPos.x && obstacle.EndPos.y == playerPos.y)
		{
			auto nextObstacle = AnachroniaAgi[i + 1];;

			//botStatus = "going to Anachronia obstacle id " + std::to_string(nextObstacle.objId);
			return nextObstacle;
		}
	}


	if (currentObstacle == 113708)
	{
		auto Asciatops = RS::GetEntityNPCByName("Asciatops");

		if (RS::GetDistance(playerPos, RS::GetEntityTilePos(Asciatops)) < 10.0f)
		{
			for (auto course : AnachroniaAgi)
			{
				if (course.objId == 113708)
				{
					return course;
				}
			}
		}
	}
	else if (currentObstacle == 113716)
	{
		auto Rex = RS::GetEntityNPCByName("Corbicula rex");

		if (RS::GetDistance(playerPos, RS::GetEntityTilePos(Rex)) < 15.0f)
		{
			for (auto course : AnachroniaAgi)
			{
				if (course.objId == 113716)
				{
					return course;
				}
			}
		}
	}
	else if (currentObstacle == 113704)
	{
		auto Rex = RS::GetEntityNPCByName("Spicati apoterrasaur");

		if (RS::GetDistance(playerPos, RS::GetEntityTilePos(Rex)) < 13.0f)
		{
			for (auto course : AnachroniaAgi)
			{
				if (course.objId == 113704)
				{
					return course;
				}
			}
		}
	}
	else if (playerPos.x == 5655 && playerPos.y == 2370)
	{
		for (auto course : AnachroniaAgi)
		{
			if (course.objId == 113716)
			{
				return course;
			}
		}
	}
	
	else if (playerPos.x == 5562 && playerPos.y == 2272)
	{
		for (auto course : AnachroniaAgi)
		{
			if (course.objId == 113704)
			{
				return course;
			}
		}
	}
	
	//botStatus = "Moving inbetween obstacle OR you are not standing where u should be to start.";

	return AgilityCourse();
}

void Woodcutting::FSM()
{
	if (!player || !player->_base)
	{
		player = new Player(RS::GetLocalPlayer());
		WoodchoppingExp = Exp::GetCurrentExp(SkillType::WOODCUTTING);
		origin = player->GetTilePosition();
		TargetLog = LogNames[SelectedWood];
		TargetTree = TreeNames[SelectedWood];
	}

	if (player->IsInAnimation())
	{

		int CurrentExp = Exp::GetCurrentExp(SkillType::WOODCUTTING);

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

	auto Closest = Static::GetClosestStaticTreeObjectByNameWithOrigin(TargetTree.data(), origin);

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

			player->StaticInteract(Closest);
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

std::vector<std::pair<std::string, std::string>> CraftableRunes = {std::pair<std::string, std::string>("Nature rune", "Nature rift"), std::pair<std::string, std::string>("Cosmic rune", "Cosmic rift") };

void AbyssCrafting::FSM()
{
	if (!player || !player->_base)
	{
		player = new Player(RS::GetLocalPlayer());

		size_t RCExp = Exp::GetCurrentExp(SkillType::RUNECRAFT);

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
	}


	if (player->GetTilePosition().y > 3521 && !IsInAbyss() && RS::GetInCombatNPCwithMe().size() > 0)
	{
		botStatus = "Being attacked, teleporting out!";
		player->InteractWithEquipment(2, 3);
		Beep(523, 100);
		return;
	}


	if (player->isMoving() || player->IsInAnimation())
		return;


	// If near bank and inventory not full AND have Cosmic rune, go bank
	auto banker = RS::GetMonsterWithinRadiusWithName("Banker", player->GetTilePosition(), 50.0f);

	if (banker && (!Inventory::isInventoryFull() && Inventory::HaveItemName(SelectedRune.data())) || (Inventory::HaveItemName("Pure essence") && Inventory::GetFreeSlot() == 1))
	{
		FillPouches();
		return;
	}

	// If inventory is full and I have pure essence and NOT in abyss
	else if (Inventory::isInventoryFull() && Inventory::HaveItemName("Pure essence") && !IsInAbyss() && !IsInAltar())
	{
		// If before wilderness wall
		if (player->GetTilePosition().y < 3521)
		{

			botStatus = "Going to wilderness wall";

			player->StaticInteractManual(65084, 3102, 3521);
			return;
			/*
			auto wall = Static::GetClosestStaticObjectByName("Wilderness wall");

			if (wall.Definition)
			{
				botStatus = "Going to wilderness wall";

				player->StaticInteractManual(65084, 3102, 3521);
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
			//player->TeleportToAbyssThroughMage();

			if(player->GetTilePosition().y > 3520 && player->GetTilePosition().y < 3526)
				player->InteractWithEquipment(1, -1, 0x59600B5); // Surge

			player->TeleportToAbyssThroughMage();
			return;
		}
	}
	else if (IsInAbyss() && isOuterAbyss())
	{
		auto widget = Static::GetClosestAbyssEntrance();

		if (widget.Definition)
		{
			botStatus = "Trying to go to inner circle.";
			player->StaticInteract(widget);
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
				player->StaticInteract(runeGate);

				//if(RS::GetDistance(player->GetTilePosition(), Tile2D(runeGate.TileX, runeGate.TileY)) > 6.0f)
				//	player->InteractWithEquipment(1, -1, 0x59600B5); // Surge

				botStatus = "Going to the Rune Rift";
				player->StaticInteract(runeGate);
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
				player->StaticInteract(Altar);
				return;
			}
		}
		else
		{
			// Means we already crafted, time to TP home
			botStatus = "Teleporting back to Edgevile";
			player->InteractWithEquipment(2, 3);
			return;
		}
	}
		
	botStatus = "I don't know what is going on here";

}

std::vector<std::string> Pouches = { "Small pouch" , "Medium pouch" , "Large pouch", "Giant pouch"};

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
		player->BankUsingNPC(banker->EntityId); // Hardcode that bank of Gielinor
		return;
	}
	else
	{
		for (auto pouchName : Pouches)
		{
			auto pouch = Inventory::GetItemNameSlot(pouchName);

			if (pouch > -1)
				player->BankInteractItem(pouch, 8);
		}

		botStatus = "Loaded bank preset and readied inventory";
		if (Inventory::GetFreeSlot() == 1)
		{
			player->BankLoadPreset(2);
		}
		else
			player->BankLoadPreset(1);
	}
}

void AbyssCrafting::RepairPouches()
{
	if (isInnerAbyss())
	{

		if(!Widgets::IsInDialogWidget())
		{
			auto darkMage = RS::GetEntityNPCByName("Dark mage");

			player->DepositActionNPC(darkMage->EntityId);

			botStatus = "Going to talk to Dark Mage";

			return;
		}
		else
		{
			// We just talked to him
			if (Widgets::GetDialogType() == RSDialog::PlayerDialog)
			{
				botStatus = "Clicking through first dialog";
				player->ConfirmChat(78053391);
				return;
			}
			// He just repaired our shits
			else if(Widgets::GetDialogType() == RSDialog::NPCDialog)
			{
				botStatus = "Clicking through second dialog";

				player->ConfirmChat();
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
		if(RS::GetDistance(player->GetTilePosition(), Tile2D(Altar.TileX, Altar.TileY)) < 10.0f)
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
	if (Inventory::HaveItemId(5511) || Inventory::HaveItemId(5513) || Inventory::HaveItemId(5515) )
		return true;

	return false;
}
