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
#include "AnachroniaAgility.h"

extern std::string botStatus;

std::vector<AgilityCourse> AnachroniaAgi = { AgilityCourse(113687, Tile2D(5414, 2324)), AgilityCourse(113688, Tile2D(5410, 2325)), AgilityCourse(113689, Tile2D(5408, 2323)), AgilityCourse(113690, Tile2D(5393, 2320)), AgilityCourse(113691, Tile2D(5367, 2304)), AgilityCourse(113692, Tile2D(5369, 2282)), AgilityCourse(113693, Tile2D(5376, 2247)), AgilityCourse(113694, Tile2D(5397, 2240)), AgilityCourse(113695, Tile2D(5439, 2217)), AgilityCourse(113696, Tile2D(5456, 2179)), AgilityCourse(113697, Tile2D(5475, 2171)), AgilityCourse(113698, Tile2D(5489, 2171)), AgilityCourse(113699, Tile2D(5502, 2171)), AgilityCourse(113700, Tile2D(5527, 2182)), AgilityCourse(113701, Tile2D(5548, 2220)), AgilityCourse(113702, Tile2D(5548, 2244)), AgilityCourse(113703, Tile2D(5553, 2249)), AgilityCourse(113704, Tile2D(5565, 2272)), AgilityCourse(113705, Tile2D(5578, 2289)), AgilityCourse(113706, Tile2D(5587, 2295)), AgilityCourse(113707, Tile2D(5596, 2295)), AgilityCourse(113708, Tile2D(5629, 2287)), AgilityCourse(113709, Tile2D(5669, 2288)), AgilityCourse(113710, Tile2D(5680, 2290)), AgilityCourse(113711, Tile2D(5684, 2293)), AgilityCourse(113712, Tile2D(5686, 2310)), AgilityCourse(113713, Tile2D(5695, 2317)), AgilityCourse(113714, Tile2D(5696, 2346)), AgilityCourse(113715, Tile2D(5675, 2363)), AgilityCourse(113716, Tile2D(5655, 2377)), AgilityCourse(113717, Tile2D(5653, 2405)), AgilityCourse(113718, Tile2D(5643, 2420)), AgilityCourse(113719, Tile2D(5642, 2431)), AgilityCourse(113720, Tile2D(5626, 2433)), AgilityCourse(113721, Tile2D(5616, 2433)), AgilityCourse(113722, Tile2D(5608, 2433)), AgilityCourse(113723, Tile2D(5601, 2433)), AgilityCourse(113724, Tile2D(5591, 2450)), AgilityCourse(113725, Tile2D(5584, 2452)), AgilityCourse(113726, Tile2D(5574, 2453)), AgilityCourse(113727, Tile2D(5564, 2452)), AgilityCourse(113728, Tile2D(5536, 2492)), AgilityCourse(113729, Tile2D(5528, 2492)), AgilityCourse(113730, Tile2D(5505, 2478)), AgilityCourse(113731, Tile2D(5505, 2468)), AgilityCourse(113732, Tile2D(5505, 2462)), AgilityCourse(113733, Tile2D(5484, 2456)), AgilityCourse(113734, Tile2D(5431, 2417)), AgilityCourse(113735, Tile2D(5431, 2407)), AgilityCourse(113736, Tile2D(5425, 2397)), AgilityCourse(113737, Tile2D(5426, 2387)), AgilityCourse(113738, Tile2D(5428, 2383)) };

void MoneyAgi::FSM()
{
	if (!player || !player->_base)
	{
		player = new Player(RS::GetLocalPlayer());
		botStatus = "Money Agi initiated.";
	}


	if (player->isMoving() || player->IsInAnimation())
	{
		botStatus = "Moving or doing animation";
		return;
	}

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
		printf("Clicking on %d\n", currentObstacle);
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

	if (playerPos.x == 5655 && playerPos.y == 2370)
	{
		return 113716;
	}

	else if (playerPos.x == 5562 && playerPos.y == 2272)
	{
		return 113704;
	}

	else if (playerPos.x == 5426 && playerPos.y == 2390)
	{
		return 113737;
	}


	else if (playerPos.x == 5413 && playerPos.y == 2325)
	{
		return 113688;
	}


	if (!player->isMoving())
	{
		auto playerEntity = (EntityObj*)(player);

		if(playerEntity->ElapsedLastAction > 60)
			return currentObstacle;

	}

	//botStatus = "Moving inbetween obstacle OR you are not standing where u should be to start.";

	return AgilityCourse();
}