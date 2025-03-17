#include "stdafx.h"
#include "WorldItem.h"

void WorldItem::Update(uintptr_t addrPointer)
{
	Address = addrPointer;
	if (!Address)
		return;

	itemEntity.Update(*(uintptr_t *)(Address + 0x28), true);
}