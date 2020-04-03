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
#include "WatchTowerAgi.h"

extern int extraDelay;

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
		printf("Is moving\n");
		return;
	}

	auto player2DPos = player->GetPosition();

	if (player2DPos)
	{

		if (player2DPos[1] < 1000.0f)
		{
			printf("Clicking on trellis\n");
			Common::StaticInteract(Trellis);
		}
		else
		{
			printf("Clicking on stair\n");

			Common::StaticInteract(Stair);
		}

		extraDelay = 1500;
	}
}
