#include "stdafx.h"
#include "Armour.h"

Armour::Armour()
{
	EvasionScore = 0;
	AmourScore = 0;
	EnergyShieldScore = 0;
}

void Armour::Update(uintptr_t addrPointer)
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
			EvasionScore = *(DWORD*)(*(DWORD64*)(Address + 0x10) + 0x10);
		}
	}
	if (*(DWORD64*)(Address + 0x10))
	{
		if (*(DWORD*)(*(DWORD64*)(Address + 0x10) + 0x14))
		{
			AmourScore = *(DWORD*)(*(DWORD64*)(Address + 0x10) + 0x14);
		}
	}
	if (*(DWORD64*)(Address + 0x10))
	{
		if (*(DWORD*)(*(DWORD64*)(Address + 0x10) + 0x18))
		{
			EnergyShieldScore = *(DWORD*)(*(DWORD64*)(Address + 0x10) + 0x18);
		}
	}
}