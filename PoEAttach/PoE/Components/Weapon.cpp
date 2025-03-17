#include "stdafx.h"
#include "Weapon.h"
#include "Common.h"

Weapon::Weapon()
{
	DamageMin = 0;
	DamageMax = 0;
	CritChance = 0;
}

void Weapon::Update(uintptr_t addrPointer)
{
	Address = addrPointer;

	if (!Address)
	{
		return;
	}

	DamageMin = (int)*(DWORD*)(*(DWORD64*)(Address + 0x28) + 0x14);
	DamageMax = (int)*(DWORD*)(*(DWORD64*)(Address + 0x28) + 0x18);
	CritChance = (int)*(DWORD*)(*(DWORD64*)(Address + 0x28) + 0x20);
}