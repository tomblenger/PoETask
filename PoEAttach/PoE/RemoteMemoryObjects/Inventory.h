#pragma once
#include "RemoteMemoryObject.h"
#include "Element.h"

enum InventoryType
{
	InvalidInventory, //Incase inventory isn't opened.
	PlayerInventory,
	NormalStash,
	QuadStash,
	CurrencyStash,
	EssenceStash,
	DivinationStash,
	MapStash,
	FragmentStash
};

class Inventory : public RemoteMemoryObject
{
protected:
	InventoryType GetInventoryType();

public:
	Inventory();
	void	Update(uintptr_t addrPointer);	

	int			index;
	uintptr_t	itemCount;
	uintptr_t	totalBoxesInInventoryRow;
	InventoryType	invType;
};