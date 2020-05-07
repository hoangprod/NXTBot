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
extern WildernessAgilityCourse* wildernessagi;

std::vector<AgilityCourse> WildernessAgi = { AgilityCourse(65362, Tile2D(3004, 3950)), AgilityCourse(64696, Tile2D(3005, 3958)), AgilityCourse(64699, Tile2D(2996, 3960)), AgilityCourse(64698, Tile2D(2994, 3945)), AgilityCourse(65734, Tile2D(2993, 3935))};

void WatchTowerAgi::FSM()
{

	Player player = RS::GetLocalPlayer();

	if (!player._base)
		return;

	if (Stair.Definition || !Trellis.Definition)
	{
		Stair = Static::GetClosestStaticObjectByNameWithOption("Ladder", "Climb-down", true);
		Trellis = Static::GetClosestStaticObjectByNameWithOption("Trellis", "Climb-up", true);

		if (!Stair.Definition || !Trellis.Definition)
		{
			printf("Could not find stair, trellis or player\n");
			return;
		}
		else
		{
			printf("Trellis: %d - %d (%d, %d)\n", Trellis.Definition->Id, Trellis.SecondId, Trellis.TileX, Trellis.TileY);
			printf("Ladder: %d - %d (%d, %d)\n", Stair.Definition->Id, Stair.SecondId, Stair.TileX, Stair.TileY);
		}
	}

	if (player.isMoving() || player.IsInAnimation())
	{
		//printf("In Animation / Moving\n");
		return;
	}

	auto player2DPos = player.GetPosition();

	if (player2DPos)
	{

		if (player2DPos[1] < 1000.0f)
		{
			//printf("Interact Trellis id %d - %d (%d , %d)\n", Trellis.Definition->Id, Trellis.SecondId, Trellis.TileX, Trellis.TileY);
			Common::StaticInteract(Trellis);
		}
		else
		{
			//printf("Interact Stair\n");
			Common::StaticInteract(Stair);
		}

		extraDelay = 1500;
	}
}


void WildernessAgilityCourse::FSM()
{
	Player player = RS::GetLocalPlayer();

	if (!player._base)
		return;

	if (Common::GetCurrentWildernessLevel() == 0)
	{
		Beep(500, 500);
		delete wildernessagi;
		wildernessagi = 0;
		return;
	}

	/*
	if (Varpbit::GetPlayerHealth() <= 0)
	{
		Beep(500, 500);
		delete wildernessagi;
		wildernessagi = 0;
		return;
	}*/

	if (player.IsInAnimation())
		return;

	auto playerPos = RS::GetLocalPlayerTilePos();

	if (currentObstacle == 65734 || isUnderWorld())
	{
		if (Varpbit::GetPlayerHealth() < 400)
		{
			ConsumeFood();
		}
	}

	enemy = RS::GetAnyPlayer();

	if (playerPos.x == 2994 && playerPos.y == 3945)
	{
	}
	else if (currentObstacle != 65734 && enemy)
	{
		printf("Hopping preq\n");
		if (Common::IsWorldWidgetUp())
		{
			Common::HopRandomWorld();
			extraDelay = 2500;
		}
		else
		{
			printf("Trying to open world hop because of %s!\n", enemy->Name);
			Common::HopWorldGUI();
			extraDelay = 800;
		}

		return;
	}
	else if (!enemy)
	{
		if (Common::IsWorldWidgetUp())
		{
			printf("Closing worldwidget\n");
			Common::InteractWithEquipment(1, -1, 0x6330072);
		}
	}


	if (player.isMoving())
	{
		printf("moving\n");
		return;
	}

	// We fell down
	if (isUnderWorld())
	{
		printf("is underworld\n");
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
		printf("can't find course!\n");
		return;
	}

	auto obstacle = Static::GetCStaticObjectById(next.objId);

	if (obstacle.Definition)
	{

		if (next.objId == currentObstacle)
		{
			printf("Elapsed Second = %d\n", player.GetElapsedSecondSinceLastAction());

			if (playerPos.x == 3002 && playerPos.y == 3963)
			{
				printf("Failed skipping pillar obstacle?\n");
			}
			else
			{
				if (player.GetElapsedSecondSinceLastAction() < 6)
				{
					return;
				}
			}

		}

		//printf("%d == %d && ( !%d && !%d)\n", next.objId, currentObstacle, playerPos.x, playerPos.y);

		currentObstacle = obstacle.Definition->Id;


		if (currentObstacle == 65362)
		{
			obstacle.TileX = 3004;
			obstacle.TileY = 3938;
		}


		printf("Clicking on next Agility obstacle ( %d ). Currently at (%d, %d)\n", currentObstacle, playerPos.x, playerPos.y);


		Common::StaticInteract(obstacle);

		extraDelay = 1700;
	}
	else
	{
		printf("Could not find the next obstacle. Maybe you are doing a course?\n");
	}
}



AgilityCourse WildernessAgilityCourse::GetNextCourse()
{
	Player player = RS::GetLocalPlayer();

	auto playerPos = player.GetTilePosition();

	if ((playerPos.x == 3005 && playerPos.y == 3962) || (playerPos.x == 3002 && playerPos.y == 3963))
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


	if (!enemy && !player.isMoving())
	{
		return currentObstacle;
	}

	return 0;
}

bool WildernessAgilityCourse::isUnderWorld()
{
	float playerZ = RS::GetLocalPlayerPos()[1];

	if (playerZ == 1471.0f || playerZ == 1511.0f || playerZ == 1421.0f || playerZ == 1551.0f || playerZ == 1491.0f)
		return true;
	else
		return false;
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