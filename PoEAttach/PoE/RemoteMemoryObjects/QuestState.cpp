#include "stdafx.h"
#include "QuestState.h"


QuestState::QuestState()
{
	questPtr = 0;
	questStateId = 0;
	testOffset = 0;
}

bool QuestState::Update(uintptr_t addr)
{
	Address = addr;
	if (!Address)
		return false;

	questPtr = *(uintptr_t *)(Address + 0x8);
	questStateId = *(int *)(Address + 0x10);
	testOffset = *(int *)(Address + 0x14);
	questStateText = ReadStringU(*(uintptr_t *)(Address + 0x2C));
	questProgressText = ReadStringU(*(uintptr_t *)(Address + 0x34));

	return true;
}