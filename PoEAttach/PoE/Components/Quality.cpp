#include "stdafx.h"
#include "Quality.h"

Quality::Quality()
{
	ItemQuality = 0;
}

void Quality::Update(uintptr_t addrPointer)
{
	Address = addrPointer;

	if (!Address)
	{
		return;
	}

	ItemQuality = (int)*(DWORD*)(Address + 0x18);
}