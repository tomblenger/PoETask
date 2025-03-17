#include "stdafx.h"
#include "Npc.h"

Npc::Npc()
{
	hasIconOverhead = false;
	isIgnoreHidden = false;
	isMinimapLabelVisible = false;
}

void Npc::Update(uintptr_t addrPointer)
{
	Address = addrPointer;
	if (!Address)
		return;

	hasIconOverhead = *(uintptr_t *)(Address + 0x48) != 0;
	isIgnoreHidden = *(BYTE *)(Address + 0x20) == 1;
	isMinimapLabelVisible = *(BYTE *)(Address + 0x21) == 1;
}