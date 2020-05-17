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
#include "Summoning.h"
#include "MoneyDrop.h"
#include "SlayerTower.h"
#include "Game.h"
#include "Auth.h"
#include "Common.h"
#include "Antiban.h"
#include "Inventory.h"
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
TaverlySummoning* taverlySummon = 0;
MoneyDrop* money_drop = 0;
SlayerTower* slayer_tower = 0;


extern std::vector<const char*> botList;
extern std::string botStatus;
extern int SelectedBot;
extern int SelectedWood;
extern int SelectedOre;
extern std::vector<std::string> TreeNames;
extern std::vector<std::string> OreNode;

extern bool break_type;
extern bool auto_start;
extern std::string auto_username;
extern std::string auto_password;

int extraDelay = 0;
bool firstLog = false;

void Manager::Manage()
{
	static uint64_t last_tick = 0;

	if (wisp || genCombat || rabbit || WoodCutting || auto_start || slayer_tower)
	{
		static uint32_t randomTick = 0;
		// If X ticks have not past yet + a random of 30-300~ ticks
		if ((last_tick + tick + randomTick + extraDelay) < GetTickCount64())
		{
			extraDelay = 0;

			randomTick = (rand() % 1000 + 300);

			if (randomTick % 89 == 0)
			{
				antiban::anti_afk();
			}

			if (auto_start)
			{
				Manager::Auto_Start();
			}

			else if (RS::IsInGame())
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
				else if (slayer_tower)
				{
					slayer_tower->FSM();
				}
			}
			else if (RS::GetGameState() == _game_state::Lobby)
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
			else if (RS::GetGameState() == _game_state::LoginScreen)
			{
				if (auto_username.size() > 5 && auto_password.size() > 5)
				{
					log("Attempting to log into %s:%s.", auto_username.data(), auto_password.data());
					Common::Login(auto_username.data(), auto_password.data());
					extraDelay = 9000;
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
			randomTick = (rand() % 101);

			if (randomTick % 88 == 0)
			{
				antiban::anti_afk();
			}

			if (RS::IsInGame())
			{
				if (genMining)
					genMining->FSM();
			}

			last_tick = GetTickCount64();
		}
	}
	else if (peng || AnachAgi || abyssCrafting || archelogy || watchtoweragi || wildernessagi || divination || fungalMage || taverlySummon || money_drop)
	{
		static uint32_t randomTick = 0;

		// If X ticks have not past yet + a random of 30-300~ ticks
		if ((last_tick + tick + randomTick + extraDelay) < GetTickCount64())
		{
			extraDelay = 0;

			randomTick = (rand() % 200 + 200);

			last_tick = GetTickCount64();

			if (!abyssCrafting && !wildernessagi && !money_drop && (antiban::long_break_manager() || antiban::short_break_manager()))
				return;

			break_type = 0;

			if (randomTick % 79 == 0)
			{
				antiban::anti_afk();
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
				else if (taverlySummon)
					taverlySummon->FSM();
				else if (money_drop)
					money_drop->FSM();
			}
			else if (RS::GetGameState() == _game_state::Lobby)
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
			else if (RS::GetGameState() == _game_state::LoginScreen)
			{
				if (auto_username.size() > 5 && auto_password.size() > 5)
				{
					log("Attempting to log into %s:%s.", auto_username.data(), auto_password.data());
					Common::Login(auto_username.data(), auto_password.data());
					extraDelay = 9000;
				}

			}

			//last_tick = GetTickCount64();
		}
	}
	else
	{
		botStatus = "Not botting";
	}
}

void Manager::Auto_Start()
{
	if (RS::GetGameState() == _game_state::LoginScreen)
	{
		log("Attempting to log into %s:%s.", auto_username.data(), auto_password.data());
		Common::Login(auto_username.data(), auto_password.data());
		extraDelay = 9000;
	}

	else if (RS::GetGameState() == _game_state::Lobby)
	{
		Common::SelectFavoriteWorld(); extraDelay = 9000;
		log("Logging from lobby into game."); return; 
	}

	else if (RS::GetGameState() == _game_state::Ingame)
	{
		Player player = RS::GetLocalPlayer();

		if (!player._base)
			return;

		// If inventory have pure ess, nature rune, or cosmic rune - start Abyss RC
		if (Inventory::GetItemById(7936) != -1 || Inventory::GetItemById(561) != -1 || Inventory::GetItemById(564) != -1)
		{
			if (!abyssCrafting)
			{
				if (AIOAuth("Abyss_RuneCrafting", "Start", player.GetName()) != -1)
					abyssCrafting = new AbyssCrafting(); auto_start = false; SelectedBot = 7;
			}
		}

		else if (MoneyAgi::is_on_endpos())
		{
			if (!AnachAgi)
			{
				if (AIOAuth("Anachronia_Agility", "Start", player.GetName()) != -1)
					AnachAgi = new MoneyAgi(); auto_start = false; SelectedBot = 6;
			}
		}

		else if (Archeology::is_next_to_archeology_node())
		{
			if (!archelogy)
			{
				if (AIOAuth("Archeology_Farming", "Start", player.GetName()) != -1)
					archelogy = new Archeology(); auto_start = false;
			}
		}

		else if (RS::GetClosestNonEnrichedWisp())
		{
			if (!divination)
			{
				if (AIOAuth("Divination_Bot", "Start", player.GetName()) != -1)
					divination = new Divination(); auto_start = false; SelectedBot = 10;
			}
		}
		
		else if (RS::GetClosestMonsterNPCByName("Fungal mage"))
		{
			if (!fungalMage)
			{
				if (AIOAuth("Fungal_Mage", "Start", player.GetName()) != -1)
					fungalMage = new FungalMage(); auto_start = false; SelectedBot = 11;
			}

		}

		else if (RS::GetClosestMonsterNPCByName("Spellwisp"))
		{
			if (!wisp)
			{
				if (AIOAuth("Wisp_Farming", "Start", player.GetName()) != -1)
					wisp = new Wisp(); auto_start = false; SelectedBot = 0;
			}

		}
		else if (Static::GetClosestStaticObjectByNameWithOption("Trellis", "Climb-up", true).Definition)
		{
			if (!watchtoweragi)
			{
				if (AIOAuth("Watchtower_Agility", "Start", player.GetName()) != -1)
					watchtoweragi = new WatchTowerAgi(); auto_start = false; SelectedBot = 8;
			}

		}
		else
		{
			log("[ Critical ] - Cannot auto start bot!"); auto_start = false;
		}

	}

}


_current_bot Manager::get_current_bot()
{
	if (peng)
		return _current_bot::Penguin_Agility;
	else if(AnachAgi)
		return _current_bot::Anachronia_Agility;
	else if (abyssCrafting)
		return _current_bot::Abyss_Crafting;
	else if (archelogy)
		return _current_bot::Archeology;
	else if (watchtoweragi)
		return _current_bot::Watchtower_Agility;
	else if (wildernessagi)
		return _current_bot::Wilderness_Agility;
	else if (divination)
		return _current_bot::Divination;
	else if (fungalMage)
		return _current_bot::Fungal_Mage;
	else if (taverlySummon)
		return _current_bot::Taverly_Summoning;
	else if (money_drop)
		return _current_bot::Money_Drop;
	else if (wisp)
		return _current_bot::Wisp_Farming;
	else if (genCombat)
		return _current_bot::General_Combat;
	else if (rabbit)
		return _current_bot::Rabbit_Farming;
	else if (WoodCutting)
		return _current_bot::Woodcutting;
	else if (genMining)
		return _current_bot::General_Mining;
	else if (slayer_tower)
		return _current_bot::Slayer_Contract;

	return _current_bot::None;
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

	if (wParam == VK_NUMPAD6)
	{
		auto_start = !auto_start;
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
		case 12:
			if (!taverlySummon)
			{
				if (AIOAuth("Taverly_Summoning", "Start", player.GetName()) != -1)
					taverlySummon = new TaverlySummoning();
			}
			else if (taverlySummon)
			{
				AIOAuth("Taverly_Summoning", "Stop", player.GetName());

				delete taverlySummon; taverlySummon = 0;
			}
			break;
		case 13:
			if (!money_drop)
			{
				if (AIOAuth("Money_Drop", "Start", player.GetName()) != -1)
					money_drop = new MoneyDrop();
			}
			else if (money_drop)
			{
				AIOAuth("Money_Drop", "Stop", player.GetName());

				delete money_drop; money_drop = 0;
			}
			break;
		case 14:
			if (!slayer_tower)
			{
				if (AIOAuth("Slayer_Tower", "Start", player.GetName()) != -1)
					slayer_tower = new SlayerTower();
			}
			else if (slayer_tower)
			{
				AIOAuth("Slayer_Tower", "Stop", player.GetName());

				delete slayer_tower; slayer_tower = 0;
			}
			break;
		default:
			break;
		}

	}
}
