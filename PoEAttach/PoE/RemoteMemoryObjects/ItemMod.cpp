#include "stdafx.h"
#include "ItemMod.h"
#include "Common.h"
#include "ItemMods.h"
#include "StatsResource.h"

ItemMod::ItemMod()
{
	memset(StatsValue, 0, sizeof(StatsValue));
	lstStatsDetailInfo.clear();
}

std::wstring ItemMod::ReplaceAll(std::wstring &str, const std::wstring& from, const std::wstring& to) 
{
	size_t start_pos = 0;
	while ((start_pos = str.find(from, start_pos)) != std::wstring::npos)
	{
		str.replace(start_pos, from.length(), to);
		start_pos += to.length();
	}
	return str;
}

int	ItemMod::IndexofAny(std::wstring & str)
{
	int i = 0;
	int nDigit = -1;

	for (i = 0; i < str.length(); i++)
	{
		if (iswdigit(str[i]))
		{
			nDigit = i;
			break;
		}
	}

	return nDigit;
}

void ItemMod::Update(uintptr_t addrPointer)
{
	Address = addrPointer;
	if (!Address)
	{
		return;
	}

	memset(StatsValue, 0, sizeof(StatsValue));

	if (*(DWORD64*)(Address))
	{
		if (*(DWORD*)(*(DWORD64*)(Address)))
		{
			StatsValue[0] = (int)**(DWORD**)(Address);
		}
	}
	if (*(DWORD64*)(Address))
	{
		if (*(DWORD*)(*(DWORD64*)(Address) + 0x04))
		{
			StatsValue[1] = (int)*(DWORD*)(*(DWORD64*)(Address) + 0x04);
		}
	}
	if (*(DWORD64*)(Address))
	{
		if (*(DWORD*)(*(DWORD64*)(Address) + 0x08))
		{
			StatsValue[2] = (int)*(DWORD*)(*(DWORD64*)(Address) + 0x08);
		}
	}
	if (*(DWORD64*)(Address))
	{
		if (*(DWORD*)(*(DWORD64*)(Address) + 0x0C))
		{
			StatsValue[3] = (int)*(DWORD*)(*(DWORD64*)(Address) + 0x0C);
		}
	}

 	lstStatsDetailInfo.clear();
 	ParseName(Address);
}

void ItemMod::ParseName(uintptr_t addrPointer)
{
	uintptr_t	addr = **(uintptr_t**)(Address + 0x20);
	DWORD		dwItemModsHashKey = *(DWORD*)(*(uintptr_t*)(Address + 0x20) + 0x08);

	rawname = ReadStringU(addr);

	uintptr_t	addrDisplay = addr + (rawname.length() + 2) * 2;
	displayname = ReadStringU(addrDisplay);

	name = ReplaceAll(rawname, L"_", L"");
	
	int ixDigits = IndexofAny(name);

	if (ixDigits < 0 || !std::stoi(name.substr(ixDigits)))
	{
				
	}
	else
	{
		name = name.substr(0, ixDigits);
	}
	
	lstStatsDetailInfo = GetItemStatsDetailInfo(dwItemModsHashKey);
}

std::vector<ItemStatsDetailInfo> ItemMod::GetItemStatsDetailInfo(DWORD dwItemModsHashKey)
{
	int	i = 0;
	ItemStatsDetailInfo					statsDetailInfo;
	std::vector<ItemStatsDetailInfo>	resultList;

	if (!dwItemModsHashKey)
	{
		return resultList;
	}

	auto ResItemMod = GetItemMods(dwItemModsHashKey);
	if (!ResItemMod)
		return resultList;
	for (i = 0; i < 4; i++)
	{
		if (ResItemMod->dwItemStatsKey[i] != NOINDEX)
		{
			WCHAR *szName = GetStatsNameByID(ResItemMod->dwItemStatsKey[i] + 1);
			if (szName)
				statsDetailInfo.statsName = szName;
			statsDetailInfo.statsValue = StatsValue[i];
			resultList.push_back(statsDetailInfo);
		}
	}

	return resultList;
}

ItemStatsDetailInfo::ItemStatsDetailInfo()
{
	statsValue = 0;
}