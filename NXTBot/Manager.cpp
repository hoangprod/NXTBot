#include "pch.h"
#include "Patterns.h"
#include "GameClasses.h"
#include "PlayableEntity.h"
#include "Wisp.h"
#include "GeneralCombat.h"
#include "AbyssCrafting.h"
#include "AnachroniaAgility.h"
#include "WatchTowerAgi.h"
#include "Archeology.h"
#include "Divination.h"
#include "FungalMage.h"
#include "Game.h"
#include "Auth.h"
#include "Common.h"
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
Archeology* archelogy = 0;
WildernessAgilityCourse* wildernessagi = 0;
WatchTowerAgi* watchtoweragi = 0;
Divination* divination = 0;
FungalMage* fungalMage = 0;

extern std::vector<const char*> botList;
extern std::string botStatus;
extern int SelectedBot;
extern int SelectedWood;
extern int SelectedOre;
extern std::vector<std::string> TreeNames;
extern std::vector<std::string> OreNode;

int extraDelay = 0;
bool firstLog = false;

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
	else if (peng || AnachAgi || abyssCrafting || archelogy || watchtoweragi || wildernessagi || divination || fungalMage)
	{
		static uint32_t randomTick = 0;

		// If X ticks have not past yet + a random of 30-300~ ticks
		if ((last_tick + tick + randomTick + extraDelay) < GetTickCount64())
		{

			extraDelay = 0;

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
				else if (archelogy)
					archelogy->FSM();
				else if (watchtoweragi)
					watchtoweragi->FSM();
				else if (wildernessagi)
					wildernessagi->FSM();
				else if (divination)
					divination->FSM();
				else if (fungalMage)
					fungalMage->FSM();
			}
			else if (RS::GetGameState() == GameState::Lobby)
			{
				if (firstLog)
				{
					firstLog = false;
					Common::LoginFromLobby();
					printf("Logging in from lobby\n");
					extraDelay = 9000;
				}
				else
				{
					firstLog = true;
					extraDelay = 5000;
				}

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

	if (archelogy)
	{
		if (wParam == VK_MULTIPLY)
		{
			archelogy->isCache = !archelogy->isCache;
		}
	}

	if (genCombat)
	{
		if (wParam == VK_MULTIPLY)
		{
			genCombat->Manual = !genCombat->Manual;
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

	if (wParam == VK_NUMPAD7)
	{
		Player player = RS::GetLocalPlayer();
		if (!archelogy)
		{
			if (AIOAuth("Archeology_Farming", "Start", player.GetName()) != -1)
				archelogy = new Archeology();
		}
		else if (archelogy)
		{
			AIOAuth("Archeology_Farming", "Stop", player.GetName());
			delete archelogy; archelogy = 0;
		}
	}

	if (wParam == VK_NUMPAD8)
	{
		Player player = RS::GetLocalPlayer();
		if (!abyssCrafting)
		{
			if(AIOAuth("Abyss_RuneCrafting", "Start", player.GetName()) != -1)
				abyssCrafting = new AbyssCrafting();
		}
		else if (abyssCrafting)
		{
			AIOAuth("Abyss_RuneCrafting", "Stop", player.GetName());
			delete abyssCrafting; abyssCrafting = 0;
		}
	}

	if (wParam == VK_NUMPAD9)
	{
		Player player = RS::GetLocalPlayer();

		switch (SelectedBot)
		{
		case 0 :
			if (!wisp)
			{
				if(AIOAuth("Wisp_Farming", "Start", player.GetName()) != -1)
					wisp = new Wisp();
			}
			else if (wisp)
			{
				AIOAuth("Wisp_Farming", "Stop", player.GetName());
				delete wisp; wisp = 0;
			}
			break;
		case 1:
			if (!rabbit)
			{
				if(AIOAuth("Rabbit_Killing", "Start", player.GetName()) != -1)
					rabbit = new Rabbit();
			}
			else if (rabbit)
			{
				AIOAuth("Rabbit_Killing", "Stop", player.GetName());
				delete rabbit; rabbit = 0;
			}
			break;
		case 2:
			if (!genCombat)
			{
				if(AIOAuth("General_Combat", "Start", player.GetName()) != -1)
					genCombat = new GeneralCombat();
			}
			else if (genCombat)
			{
				AIOAuth("General_Combat", "Stop", player.GetName());
				delete genCombat; genCombat = 0;
			}
			break;
		case 3:
			if (!genMining)
			{
				if(AIOAuth("General_Mining", "Start", player.GetName()) != -1)
					genMining = new GenMining();
			}
			else if (genMining)
			{
				AIOAuth("General_Mining", "Stop", player.GetName());

				delete genMining; genMining = 0;
			}
			break;
		case 4:
			if (!peng)
			{
				if(AIOAuth("Penguin_Suits", "Start", player.GetName()) != -1)
					peng = new Penguins();
			}
			else if (peng)
			{
				AIOAuth("Penguin_Suits", "Stop", player.GetName());

				delete peng; peng = 0;
			}
			break;
		case 5:
			if (!WoodCutting)
			{
				if(AIOAuth("WoodCutting", "Start", player.GetName()) != -1)
					WoodCutting = new Woodcutting();
			}
			else if (WoodCutting)
			{
				AIOAuth("WoodCutting", "Stop", player.GetName());

				delete WoodCutting; WoodCutting = 0;
			}
			break;
		case 6:
			if (!AnachAgi)
			{
				if(AIOAuth("Anachronia_Agility", "Start", player.GetName()) != -1)
					AnachAgi = new MoneyAgi();
			}
			else if (AnachAgi)
			{
				AIOAuth("Anachronia_Agility", "Stop", player.GetName());

				delete AnachAgi; AnachAgi = 0;
			}
			break;
		case 7:
			if (!abyssCrafting)
			{
				if(AIOAuth("Abyss_RuneCrafting", "Start", player.GetName()) != -1)
					abyssCrafting = new AbyssCrafting();
			}
			else if (abyssCrafting)
			{
				AIOAuth("Abyss_RuneCrafting", "Stop", player.GetName());

				delete abyssCrafting; abyssCrafting = 0;
			}
			break;
		case 8:
			if (!watchtoweragi)
			{
				if(AIOAuth("Watchtower_Agility", "Start", player.GetName()) != -1)
					watchtoweragi = new WatchTowerAgi();
			}
			else if (watchtoweragi)
			{
				AIOAuth("Watchtower_Agility", "Stop", player.GetName());

				delete watchtoweragi; watchtoweragi = 0;
			}
			break;
		case 9:
			if (!wildernessagi)
			{
				if(AIOAuth("Wilderness_Agility", "Start", player.GetName()) != -1)
					wildernessagi = new WildernessAgilityCourse();
			}
			else if (wildernessagi)
			{
				AIOAuth("Wilderness_Agility", "Stop", player.GetName());

				delete wildernessagi; wildernessagi = 0;
			}
			break;
		case 10:
			if (!divination)
			{
				if (AIOAuth("Divination_Bot", "Start", player.GetName()) != -1)
					divination = new Divination();
			}
			else if (divination)
			{
				AIOAuth("Divination_Bot", "Stop", player.GetName());

				delete divination; divination = 0;
			}
			break;
		case 11:
			if (!fungalMage)
			{
				if (AIOAuth("Fungal_Mage", "Start", player.GetName()) != -1)
					fungalMage = new FungalMage();
			}
			else if (fungalMage)
			{
				AIOAuth("Fungal_Mage", "Stop", player.GetName());

				delete fungalMage; fungalMage = 0;
			}
			break;
		default:
			break;
		}

	}
}
