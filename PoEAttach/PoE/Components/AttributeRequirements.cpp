#include "stdafx.h"
#include "AttributeRequirements.h"

AttributeRequirements::AttributeRequirements()
{
	strength = 0;
	dexterity = 0;
	intelligence = 0;
}

void AttributeRequirements::Update(uintptr_t addrPointer)
{
	Address = addrPointer;
	if (!Address)
	{
		return;
	}

	if (*(DWORD64*)(Address + 0x10))
	{
		if (*(DWORD*)(*(DWORD64*)(Address + 0x10) + 0x10))
		{
			strength = *(DWORD*)(*(DWORD64*)(Address + 0x10) + 0x10);
		}
	}

	if (*(DWORD64*)(Address + 0x10))
	{
		if (*(DWORD*)(*(DWORD64*)(Address + 0x10) + 0x14))
		{
			dexterity = *(DWORD*)(*(DWORD64*)(Address + 0x10) + 0x14);
		}
	}

	if (*(DWORD64*)(Address + 0x10))
	{
		if (*(DWORD*)(*(DWORD64*)(Address + 0x10) + 0x18))
		{
			intelligence = *(DWORD*)(*(DWORD64*)(Address + 0x10) + 0x18);
		}
	}
}
