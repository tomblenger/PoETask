#include "stdafx.h"
#include "Charges.h"
#include "Common.h"

Charges::Charges()
{
	CurrentCharges = 0;
	ChargesPerUse = 0;
	ChargesMax = 0;
}

void Charges::Update(uintptr_t addrPointer)
{
	Address = addrPointer;
	if (!Address)
	{
		return;
	}

	CurrentCharges = *(DWORD*)(Address + 0x18);
	ChargesPerUse = *(DWORD*)(*(DWORD64*)(Address + 0x10) + 0x14);
	ChargesMax = *(DWORD*)(*(DWORD64*)(Address + 0x10) + 0x10);
}