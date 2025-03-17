#include "stdafx.h"
#include "Base.h"
#include "Common.h"

Base::Base()
{
	ItemCellsSizeX = 0;
	ItemCellsSizeY = 0;
	IsCorrupted = false;
	IsShaper = false;
	IsElder = false;
}

void Base::update(uintptr_t addrPointer)
{
	Address = addrPointer;

	if (!Address)
	{
		return;
	}
	
	if (*(DWORD64*)(Address + 0x10))
	{
		ItemName = ReadNativeString((*(DWORD64*)(Address + 0x10) + 0x18));
	}

	ItemCellsSizeX = *(DWORD*)(*(DWORD64*)(Address + 0x10) + 0x10);
	ItemCellsSizeY = *(DWORD*)(*(DWORD64*)(Address + 0x10) + 0x14);
	IsCorrupted = *(BYTE*)(Address + 0xD8);
	IsShaper = *(BYTE*)(Address + 0xD9);
	IsElder = *(BYTE*)(Address + 0xDA);
}