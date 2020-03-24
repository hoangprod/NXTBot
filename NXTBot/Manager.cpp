#include "pch.h"
#include "Patterns.h"
#include "GameClasses.h"
#include "PlayableEntity.h"
#include "Wisp.h"
#include "Game.h"
#include "Manager.h"

extern HWND hWnd;

Wisp* wisp = 0;
Rabbit* rabbit = 0;
GeneralCombat* genCombat = 0;
GenMining* genMining = 0;
Penguins* peng = 0;
BikeAgi* bikeagi = 0;
Woodcutting* WoodCutting = 0;
MoneyAgi* AnachAgi = 0;
AbyssCrafting* abyssCrafting = 0;

extern std::vector<const char*> botList;
extern std::string botStatus;
extern int SelectedBot;
extern int SelectedWood;
extern int SelectedOre;
extern std::vector<std::string> TreeNames;
extern std::vector<std::string> OreNode;

void Manager::Manage()
{
	static uint64_t last_tick = 0;

	if (wisp || genCombat || rabbit || WoodCutting)
	{
		static uint32_t randomTick = 0;
		// If X ticks have not past yet + a random of 30-300~ ticks
		if ((last_tick + tick + randomTick) < GetTickCount64())
		{
			randomTick = (rand() % 1000 + 300);

			if (randomTick % 22 == 0)
			{
				SendMessage(hWnd, WM_KEYDOWN, VK_INSERT, 0);
				SendMessage(hWnd, WM_KEYUP, VK_INSERT, 0);
			}

			if (RS::IsInGame())
			{
				// Will only do 1 or the other

				if (wisp)
				{
					wisp->FSM();
				}
				else if (genCombat)
				{
					genCombat->FSM();
				}
				else if (rabbit)
				{
					rabbit->FSM();
				}
				else if (WoodCutting)
				{
					WoodCutting->FSM();
				}
			}

			last_tick = GetTickCount64();
		}
	}
	else if (genMining)
	{
		static uint32_t randomTick = 0;
		// If X ticks have not past yet + a random of 30-300~ ticks
		if ((last_tick + tick + randomTick) < GetTickCount64())
		{
			randomTick = (rand() % 150);

			if (randomTick % 22 == 0)
			{
				SendMessage(hWnd, WM_KEYDOWN, VK_INSERT, 0);
				SendMessage(hWnd, WM_KEYUP, VK_INSERT, 0);
			}

			if (RS::IsInGame())
			{
				genMining->FSM();
			}

			last_tick = GetTickCount64();
		}
	}
	else if (peng || AnachAgi || abyssCrafting)
	{
		static uint32_t randomTick = 0;

		// If X ticks have not past yet + a random of 30-300~ ticks
		if ((last_tick + tick + randomTick) < GetTickCount64())
		{
			randomTick = (rand() % 200 + 200);

			if (randomTick % 22 == 0)
			{
				SendMessage(hWnd, WM_KEYDOWN, VK_INSERT, 0);
				SendMessage(hWnd, WM_KEYUP, VK_INSERT, 0);
			}

			if (RS::IsInGame())
			{
				if (peng)
					peng->FSM();
				else if (AnachAgi)
					AnachAgi->FSM();
				else if (abyssCrafting)
					abyssCrafting->FSM();
			}

			last_tick = GetTickCount64();
		}
	}
	else
	{
		botStatus = "Not botting";
	}


}

void Manager::Keystates(WPARAM wParam)
{

	if (wParam == VK_ADD)
	{
		if (SelectedBot == -1 || SelectedBot <= (botList.size() - 2))
		{
			SelectedBot++;
		}
	}
	else if (wParam == VK_SUBTRACT)
	{
		if (SelectedBot > 0)
		{
			SelectedBot--;
		}
	}

	if (SelectedBot != -1)
	{
		if (strcmp(botList[SelectedBot], "WoodCutting") == 0)
		{
			if (wParam == VK_MULTIPLY)
			{
				if (SelectedWood == -1 || SelectedWood <= (TreeNames.size() - 2))
				{
					SelectedWood++;
				}
			}
			else if (wParam == VK_DIVIDE)
			{
				if (SelectedWood > 0)
				{
					SelectedWood--;
				}
			}
		}
		else if (strcmp(botList[SelectedBot], "Mining") == 0)
		{
			if (wParam == VK_MULTIPLY)
			{
				if (SelectedOre == -1 || SelectedOre <= (OreNode.size() - 2))
				{
					SelectedOre++;
				}
			}
			else if (wParam == VK_DIVIDE)
			{
				if (SelectedOre > 0)
				{
					SelectedOre--;
				}
			}
		}
	}

	if (wParam == VK_NUMPAD8)
	{
		if (!abyssCrafting)
		{
			abyssCrafting = new AbyssCrafting();
		}
		else if (abyssCrafting)
		{
			delete abyssCrafting; abyssCrafting = 0;
		}
	}

	if (wParam == VK_NUMPAD9)
	{
		switch (SelectedBot)
		{
		case 0 :
			if (!wisp)
			{
				wisp = new Wisp();
			}
			else if (wisp)
			{
				delete wisp; wisp = 0;
			}
			break;
		case 1:
			if (!rabbit)
			{
				rabbit = new Rabbit();
			}
			else if (rabbit)
			{
				delete rabbit; rabbit = 0;
			}
			break;
		case 2:
			if (!genCombat)
			{
				genCombat = new GeneralCombat();
			}
			else if (genCombat)
			{
				delete genCombat; genCombat = 0;
			}
			break;
		case 3:
			if (!genMining)
			{
				genMining = new GenMining();
			}
			else if (genMining)
			{
				delete genMining; genMining = 0;
			}
			break;
		case 4:
			if (!peng)
			{
				peng = new Penguins();
			}
			else if (peng)
			{
				delete peng; peng = 0;
			}
			break;
		case 5:
			if (!WoodCutting)
			{
				WoodCutting = new Woodcutting();
			}
			else if (WoodCutting)
			{
				delete WoodCutting; WoodCutting = 0;
			}
			break;
		case 6:
			if (!AnachAgi)
			{
				AnachAgi = new MoneyAgi();
			}
			else if (AnachAgi)
			{
				delete AnachAgi; AnachAgi = 0;
			}
			break;
		case 7:
			if (!abyssCrafting)
			{
				abyssCrafting = new AbyssCrafting();
			}
			else if (abyssCrafting)
			{
				delete abyssCrafting; abyssCrafting = 0;
			}
			break;
		default:
			break;
		}

	}
}
