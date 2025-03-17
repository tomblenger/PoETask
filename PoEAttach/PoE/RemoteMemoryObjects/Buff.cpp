#include "stdafx.h"
#include "Buff.h"

Buff::Buff()
{
	charges = 0;
	maxTime = 0;
	timer = 0;
}

void Buff::Update(uintptr_t addrPointer)
{
	Address = addrPointer;
	if (!Address)
		return;

	name = ReadStringU(**(uintptr_t **)(Address + 0x08));
	charges = *(BYTE *)(Address + 0x2C);
	maxTime = *(float *)(Address + 0x10);
	timer = *(float *)(Address + 0x14);
}