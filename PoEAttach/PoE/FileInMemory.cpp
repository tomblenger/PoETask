#include "stdafx.h"
#include "FileInMemory.h"


std::vector<uintptr_t> FileInMemory::RecordAddresses()
{
	std::vector<uintptr_t> records;

	if (!Address || !*(uintptr_t *)(Address + 0x40))
		return records;

	uintptr_t firstRec = *(uintptr_t *)(*(uintptr_t *)(Address + 0x40) + 0x0);
	uintptr_t lastRec = *(uintptr_t *)(*(uintptr_t *)(Address + 0x40) + 0x8);
	int cnt = *(int *)(*(uintptr_t *)(Address + 0x40) + 0x20);
	uintptr_t recLen = (lastRec - firstRec) / cnt;

	for (int i = 0; i < cnt; i++)
	{
		records.push_back(firstRec + i * recLen);
	}

	return records;
}