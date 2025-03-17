#include "stdafx.h"
#include "Quest.h"

Quest::Quest()
{
	act = 0;
	questState = 0;
	questID = 0;
	step = -1;
	isCompleted = false;
	key = QUEST_RES_COUNT + 1;
}

bool Quest::Update(uintptr_t addrPointer)
{
	Address = addrPointer;
	if (!Address)
		return false;

	auto addr = *(uintptr_t *)(addrPointer + 0x18);
	id = ReadStringU(*(uintptr_t *)addr);
	act = *(int *)(addr + 0x08);
	name = ReadStringU(*(uintptr_t *)(addr + 0x0C));
	questState = *(int *)(addr + 0x14);
	questID = *(int *)(addr + 0x20);
	step = *(int *)(Address + 0x20);
	isCompleted = step == 0;

	key = (int)GetKeyByIDFromResource(id);

	return true;
}