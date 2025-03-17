#include "stdafx.h"
#include "Inventory.h"
#include "GameStateController.h"

Inventory::Inventory()
{
	index = 0;
	itemCount = 0;
	totalBoxesInInventoryRow = 0;
}

InventoryType Inventory::GetInventoryType()
{
	//GameStateController::InGameState.ingameUi.inventoryPanel

	Element element;
	element.Update(Address);
	if (element.parent)
	{
		element.Update(element.parent);
		switch (element.childCount)
		{
		case 0x6f:
			return EssenceStash;
		case 0x38:
			return CurrencyStash;
		case 0x40:
			return FragmentStash;
		case 0x05:
			return DivinationStash;
		case 0x06:
			return MapStash;
		case 0x01:
			// Normal Stash and Quad Stash is same.
			if (totalBoxesInInventoryRow == 24)
			{
				return QuadStash;
			}
			return NormalStash;
		default:
			return InvalidInventory;
		}
	}
	return InvalidInventory;
	
}

void Inventory::Update(uintptr_t addrPointer)
{
	Address = addrPointer;// *(uintptr_t *)(addrPointer + 0x18);
	if (!Address)
		return;
	
	itemCount = *(uintptr_t *)(Address + 0x3B8);
	totalBoxesInInventoryRow = *(uintptr_t *)(Address + 0x4A0);
	invType = GetInventoryType();
}