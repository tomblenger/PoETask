#include "stdafx.h"
#include "InventoryHolder.h"
#include "Common.h"


int InventoryHolder::StructSize = 0x20;

InventoryHolder::InventoryHolder()
{
	id = 0;
}

void InventoryHolder::Update(uintptr_t addrPointer)
{
	Address = addrPointer;
	if (!Address)
		return;

	id = *(int *)Address;
	inventory.Update(*(uintptr_t *)(Address + 8));
}