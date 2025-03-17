#include "stdafx.h"
#include "Stats.h"

Stats::Stats()
{
	id = 0;
	val = 0;
}

void Stats::Update(uintptr_t addrPointer)
{
	Address = addrPointer;
	if (!Address)
		return;

	id = *(int *)(Address);
	val = *(int *)(Address + 0x04);
	WCHAR *szName = GetStatsDisplayNameByID(id);
	if (szName)
		name = szName;
}