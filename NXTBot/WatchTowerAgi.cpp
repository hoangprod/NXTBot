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
#include "WatchTowerAgi.h"

extern int extraDelay;

extern std::vector<std::string> foodlist;

std::vector<AgilityCourse> WildernessAgi = { AgilityCourse(65362, Tile2D(3004, 3950)), AgilityCourse(64696, Tile2D(3005, 3958)), AgilityCourse(64699, Tile2D(2996, 3960)), AgilityCourse(64698, Tile2D(2994, 3945)), AgilityCourse(65734, Tile2D(2993, 3935))};

void WatchTowerAgi::FSM()
{
	if (!player || !player->_base || !Stair.Definition || !Trellis.Definition)
	{
		player = new Player(RS::GetLocalPlayer());
		Stair = Static::GetClosestStaticObjectByNameWithOption("Ladder", "Climb-down", true);
		Trellis = Static::GetClosestStaticObjectByName("Trellis");

		if (!Stair.Definition || !Trellis.Definition || !player)
		{
			printf("Could not find stair, trellis or player\n");
			return;
		}
	}



	if (player->isMoving() || player->IsInAnimation())
	{
		return;
	}

	auto player2DPos = player->GetPosition();

	if (player2DPos)
	{

		if (player2DPos[1] < 1000.0f)
		{
			Common::StaticInteract(Trellis);
		}
		else
		{
			Common::StaticInteract(Stair);
		}

		extraDelay = 1500;
	}
}


void WildernessAgilityCourse::FSM()
{
	if (!player || !player->_base)
	{
		player = new Player(RS::GetLocalPlayer());
	}

	auto enemy = RS::GetClosestPlayer();

	if (currentObstacle != 65734 && enemy)
	{

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
	else if (!enemy)
	{
		if (Common::IsWorldWidgetUp())
		{
			Common::InteractWithEquipment(1, -1, 0x6330072);
		}
	}

	if (player->isMoving() || player->IsInAnimation())
	{
		return;
	}


	float playerZ = RS::GetLocalPlayerPos()[1];

	if (currentObstacle == 65734 || playerZ > 600.0f)
	{
		if (Varpbit::GetPlayerHealth() < 500)
		{
			ConsumeFood();
		}
	}

	// We fell down
	if (playerZ > 600.0f)
	{
		auto Ladder = Static::GetClosestStaticObjectByNameWithOption("Ladder", "Climb-up", true);

		if (Ladder.Definition)
		{
			Common::StaticInteract(Ladder);
			return;
		}
	}

	auto next = WildernessAgilityCourse::GetNextCourse();

	if (next.objId < 2)
	{
		return;
	}

	auto obstacle = Static::GetCStaticObjectById(next.objId);

	if (obstacle.Definition)
	{

		if (next.objId == currentObstacle)
		{

			if (player->GetElapsedSecondSinceLastAction() < 5)
			{
				return;
			}
		}

		currentObstacle = obstacle.Definition->Id;

		printf("Clicking on next Agility obstacle.\n");

		if (currentObstacle == 65362)
		{
			obstacle.TileX = 3004;
			obstacle.TileY = 3938;
		}

		Common::StaticInteract(obstacle);

		extraDelay = 1500;
	}
	else
	{
		printf("Could not find the next obstacle. Maybe you are doing a course?\n");
	}
}



AgilityCourse WildernessAgilityCourse::GetNextCourse()
{
	auto playerPos = player->GetTilePosition();

	if (playerPos.x == 3005 && playerPos.y == 3962)
	{
		return WildernessAgi[2];
	}


	auto lastCourse = WildernessAgi[WildernessAgi.size() - 1];

	if (lastCourse.EndPos.x == playerPos.x && lastCourse.EndPos.y == playerPos.y)
	{
		// We are at the end, return the first course.
		return WildernessAgi[0];
	}

	for (size_t i = 0; i < WildernessAgi.size(); i++)
	{
		auto obstacle = WildernessAgi[i];

		// If there is a match of end pos and player pos
		if (obstacle.EndPos.x == playerPos.x && obstacle.EndPos.y == playerPos.y)
		{
			auto nextObstacle = WildernessAgi[i + 1];;

			return nextObstacle;
		}
	}


	if (!player->isMoving())
	{
		return currentObstacle;
	}


	return 0;
}

void WildernessAgilityCourse::ConsumeFood()
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