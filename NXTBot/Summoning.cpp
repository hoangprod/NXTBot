#include "pch.h"
#include "GameClasses.h"
#include "Patterns.h"
#include "Inventory.h"
#include "Game.h"
#include "PlayableEntity.h"
#include "Tile.h"
#include "Widgets.h"
#include "Experience.h"
#include "Varpbit.h"
#include "Common.h"
#include "Summoning.h"

extern int extraDelay;
extern TaverlySummoning *taverlySummon;

void TaverlySummoning::FSM()
{
	Player player = RS::GetLocalPlayer();

	if (!player._base)
	{
		return;
	}

	if (runOnce)
	{
		// Noted form
		auto inventory = Inventory::GetContainerObj(static_cast<uint32_t>(ContainerType::Backpack));

		if (!inventory || inventory->ContainerContent == 0)
		{
			return;
		}

		tertiary_product = inventory->ContainerContent[27].ItemId;


		Obelisk = Static::GetClosestStaticObjectByName("Obelisk");

		Magestrix = RS::GetClosestEntityNPCByName("Magestix", false);

		printf("Starting... %d %d %d\n", tertiary_product, Obelisk.Definition, Magestrix);

		if (tertiary_product == -1 || Obelisk.Definition == 0 || Magestrix == 0)
		{
			printf("Something went wrong\n");
			delete taverlySummon;
			taverlySummon = 0;
			return;
		}

		runOnce = false;
	}

	// Have unnoted version, convert to pouches
	if (Inventory::GetItemById(tertiary_product - 1) != -1)
	{

		// If we haven't click on Obelisk yet
		if (Widgets::GetUIType() != UIType::OBELISK_WIDGET)
		{

			Common::StaticInteract(Obelisk);
			return;
		}

		// If Obelisk is up
		else if (Widgets::GetUIType() == UIType::OBELISK_WIDGET)
		{

			Common::ConfirmGUI(0x55A001E);
			return;
		}
	}

	// Does not have any unnoted tertiary item
	else
	{
		// If have Pouches, convert to scroll
		if (HasSummoningPouches())
		{
			// If we haven't click on Obelisk yet
			if (!Widgets::GetWidgetUI(SUMMONING_WIDGET) || Widgets::GetUIType() != UIType::OBELISK_WIDGET)
			{
				Common::StaticInteract(Obelisk);
				return;
			}

			// If Obelisk is up
			else if (Widgets::GetUIType() == UIType::OBELISK_WIDGET)
			{
				Common::ConfirmGUI(0x55A001E);
				return;
			}
		}

		// Does not have Pouch or Tertiary item, therefore must unnote from NPC
		else
		{
			// If shop UI is not up, talk to NPC
			if (!IsShopUIOn())
			{
				Common::BankUsingNPC(Magestrix->EntityId);
				return;
			}
			else
			{
				int tertarySlot = TaverlySummoning::GetShopTertiary();

				// If shop does have our item
				if (tertarySlot != -1 && tertarySlot > 7 && !Inventory::isInventoryFull())
				{
					// If TAB is not on 0 (aka on 1, aka on sell)
					if (Varpbit::GetVarp(SHOP_TAB) != 0)
					{
						// Click on BUY Tab
						Common::InteractWithEquipment(1, -1, 0x4F10029);
						return;
					}

					// Buy all
					Common::InteractWithEquipment(7, tertarySlot, 0x4F10014);
					extraDelay = 700;
					return;
				}

				// If shop does NOT have our item
				else
				{
					// If TAB is not on 1 (aka currently on 0, aka on buy)
					if (Varpbit::GetVarp(SHOP_TAB) != 1)
					{
						// Click on SELL Tab
						Common::InteractWithEquipment(1, -1, 0x4F10020);
						return;
					}

					int tertarySlot = Inventory::GetItemById(tertiary_product);
					// If we have item still in our inventory
					if (tertarySlot != -1)
					{
						// Sell 500
						Common::InteractWithEquipment(6, tertarySlot, 0x4F10014);
						extraDelay = 400;
						return;
					}
					// If we ran out, beep and shut down
					else
					{
						Beep(500, 500);
						delete taverlySummon;
						taverlySummon = 0;
						return;
					}
				}
			}

		}

	}

}

int TaverlySummoning::GetShopTertiary()
{
	auto ShopBuy = Inventory::GetContainerObj(static_cast<uint32_t>(ContainerType::ShopBuy));

	if (!ShopBuy)
		return -1;

	for (int i = 0; i < 20; i++)
	{
		auto forSale = ShopBuy->ContainerContent[i];
		
		// If have unnoted form in shop
		if (forSale.ItemId == tertiary_product - 1)
		{
			return i;
		}
	}

	return -1;
}

bool TaverlySummoning::IsShopUIOn()
{
	if (Widgets::GetWidgetUI(SHOP_WIDGET) && Widgets::GetUIType() == UIType::SHOP_STORE)
		return true;
	return false;
}

bool TaverlySummoning::HasSummoningPouches()
{
	for (int i = 12007; i <= 12107; i++)
	{
		if (Inventory::GetItemById(i) != -1)
			return true;
	}

	return false;
}
