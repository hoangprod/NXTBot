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


void Manager::Manage()
{
	static uint64_t last_tick = 0;

	if (wisp || genCombat || rabbit)
	{
		static uint32_t randomTick = 0;
		// If X ticks have not past yet + a random of 30-300~ ticks
		if ((last_tick + tick + randomTick) < GetTickCount64())
		{
			randomTick = (rand() % 1000 + 300);

			if (randomTick % 5 == 0)
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
			}

			last_tick = GetTickCount64();
		}
	}


}

void Manager::Keystates(WPARAM wParam)
{
	if (wParam == VK_NUMPAD9)
	{

		if (!wisp)
		{
			printf("[+] ************************Starting Spellwisp farming*********************************\n");

			wisp = new Wisp();
		}
		else if (wisp)
		{
			printf("[+] *************************= Stopping Spellwisp farming =********************************\n");

			delete wisp; wisp = 0;
		}
	}

	else if (wParam == VK_NUMPAD8)
	{
		if (!genCombat)
		{
			printf("[+] ****************************= Starting general combat =******************************\n");
			genCombat = new GeneralCombat();
		}
		else if (genCombat)
		{
			printf("[+] ****************************= Stopping general combat =*****************************\n");

			delete genCombat; genCombat = 0;
		}
	}

	else if (wParam == VK_NUMPAD7)
	{
		if (!rabbit)
		{
			printf("[+] ****************************= Starting rabbit farming =****************************\n");

			rabbit = new Rabbit();
		}
		else if (rabbit)
		{
			printf("[+] ****************************=Stopping rabbit combat =******************************\n");

			delete rabbit; rabbit = 0;
		}
	}
}
