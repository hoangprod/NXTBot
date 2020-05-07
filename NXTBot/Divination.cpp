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
#include "Divination.h"

extern int extraDelay;

extern Divination* divination;

void Divination::FSM()
{
	Player player = RS::GetLocalPlayer();

	if (!player._base)
		return;

	if (!runOnce)
	{
		auto wisp = RS::GetClosestMonster();

		if (!wisp)
			return;

		TargetName = wisp->Name;

		printf("Targeting Wisp type: %s\n", TargetName.data());

		runOnce = !runOnce;
	}



	if (player.isMoving() || player.IsInAnimation() || player.CurrentUsefulAni() != -1)
	{
		//printf("is moving\n");
		return;
	}

	auto haveMemory = HaveMemoryInInventory();

	auto EnergyRift = Static::GetClosestStaticObjectByName("Energy Rift");

	if (!EnergyRift.Definition)
		EnergyRift = Static::GetClosestStaticObjectByName("Energy rift", true);


	if (EnergyRift.Definition && ((Inventory::isInventoryFull() && haveMemory) || (haveMemory && RS::GetDistance(player.GetTilePosition(), Tile2D(EnergyRift.TileX, EnergyRift.TileY)) < 3.0f )))
	{

		if (EnergyRift.Definition)
		{
			//printf("%d %d %d\n", EnergyRift.Definition->Id, EnergyRift.TileX, EnergyRift.TileY);

			printf("Depositing in rift\n");
			Common::StaticInteract(EnergyRift);
			extraDelay = 5000;
		}

		return;
	}

	auto Wisp = RS::GetClosestEntityNPCByNameStrStr("Enriched");

	if(!Wisp)
		Wisp = RS::GetClosestEntityNPCByName(TargetName.data());

	if (Wisp)
	{
		printf("interacting with wisp: %p\n", Wisp);

		// I don't know why it is named banking lmao
		Common::BankUsingNPC(Wisp->EntityId);

		extraDelay = 5000;
	}
}

bool Divination::HaveMemoryInInventory()
{
	//29385-29406  29199-29208

	for (uint32_t i = 29385; i <= 29406; i++)
	{
		if (Inventory::GetItemById(i) != -1)
			return true;
	}

	for (uint32_t i = 29199; i <= 29208; i++)
	{
		if (Inventory::GetItemById(i) != -1)
			return true;
	}

	if (Inventory::GetItemById(37946) != -1)
		return true;

	if (Inventory::GetItemById(29384) != -1)
		return true;

	return false;
}
