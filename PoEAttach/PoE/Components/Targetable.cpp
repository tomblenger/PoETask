#include "stdafx.h"
#include "Targetable.h"

Targetable::Targetable()
{
	isTargetable = false;
	isTargeted = false;
}

void Targetable::Update(uintptr_t addrPointer)
{
	Address = addrPointer;
	if (!Address)
		return;

	isTargetable = *(BYTE *)(Address + 0x30) == 1;
	isTargeted = *(BYTE *)(Address + 0x32) == 1;
}