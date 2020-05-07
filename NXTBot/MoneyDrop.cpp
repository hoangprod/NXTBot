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
#include "MoneyDrop.h"

void MoneyDrop::FSM()
{
	Player player = RS::GetLocalPlayer();

	if (!player._base)
		return;

	// If inventory is empty
	if (Inventory::GetFreeSlot() == 28)
	{
		if (!Inventory::isBankOpened())
		{
			auto banker = RS::GetClosestEntityNPCByName("Banker");

			if (!banker)
			{
				StaticObjEX chest = Static::GetClosestStaticObjectByName("Bank chest");

				if (chest.Definition)
				{
					Common::StaticInteract2(chest);
					return;
				}
				else
				{
					return;
				}
			}
			else
			{
				Common::BankUsingNPC(banker->EntityId);
				return;
			}

		}

		// If Bank interface is up
		else
		{
			Common::BankLoadPreset(2);
			return;
		}
	}

	// if inventory is NOT empty
	else
	{
		auto inventory = Inventory::GetContainerObj(static_cast<uint32_t>(ContainerType::Backpack));

		if (!inventory || inventory->ContainerContent == 0)
		{
			return;
		}

		for (int i = 0; i < 28; i++)
		{
			FakeItem item = inventory->ContainerContent[i];

			if (item.ItemId != -1)
				Inventory::InteractItemOption(i, 8);
		}
	}
}
