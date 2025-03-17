#include "stdafx.h"
#include "Stack.h"

ItemStack::ItemStack()
{
	Size = 0;
}

void ItemStack::Update(uintptr_t addrPointer)
{
	Address = addrPointer;

	if (!Address)
	{
		return;
	}

	Size = *(int*)(Address + 0x18);
	if (*(DWORD64*)(Address + 0x10))
	{
		MaxStackSize = *(int *)(*(DWORD64*)(Address + 0x10) + 0x28);
	}

}