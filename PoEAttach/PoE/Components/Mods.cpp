#include "stdafx.h"
#include "Mods.h"
#include "Common.h"

Mods::Mods()
{
	bIdentified = false;
	bIsUsable = false;
	bIsMirrored = false;
	struniqueName = L"";
	implicitMods.clear();
	explicitMods.clear();
}

void Mods::Update(uintptr_t addrPointer)
{
	Address = addrPointer;
	if (!Address)
	{
		return;
	}
	
	if (*(DWORD64*)(Address + 0x30))
	{
		if (*(DWORD64*)(*(DWORD64*)(Address + 0x30) + 0x08))
		{
			if (*(DWORD64*)(*(DWORD64*)(*(DWORD64*)(Address + 0x30) + 0x08) + 0x30))
			{
				struniqueName = ReadStringU(*(DWORD64*)(*(DWORD64*)(*(DWORD64*)(Address + 0x30) + 0x08) + 0x30));
			}
		}
	}

 	bIdentified = *(BYTE *)(Address + 0x88) == 1;
 	nItemRarity = (int)*(DWORD*)(Address + 0x8C);
	nItemLevel = (int)*(DWORD*)(Address + 0x434);
	nItemRequireLevel = (int)*(DWORD*)(Address + 0x438);
	bIsUsable = *(BYTE*)(Address + 0x370) != 0;
	bIsMirrored = *(BYTE*)(Address + 0x371) != 0;

	implicitMods.clear();
	explicitMods.clear();

	implicitMods = GetMods(0x90, 0x98);
	explicitMods = GetMods(0xA8, 0xB0);
	implicitMods.insert(implicitMods.end(), explicitMods.begin(), explicitMods.end());
}

std::vector<ItemMod> Mods::GetMods(DWORD dwStartOffset, DWORD dwEndOffset)
{
	ItemMod		itemMod;
	std::vector<ItemMod>		resultList;

	if (Address == 0)
	{
		return resultList;
	}

	uintptr_t	begin = *(uintptr_t*)(Address + dwStartOffset);
	uintptr_t	end = *(uintptr_t*)(Address + dwEndOffset);

	uintptr_t	count = (end - begin) / 0x28;

	if (count > 12)
	{
		return resultList;
	}

	for (uintptr_t i = begin; i < end; i+= 0x28)
	{
		itemMod.Update(i);
		resultList.push_back(itemMod);
	}

	return resultList;

}