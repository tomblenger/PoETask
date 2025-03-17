#pragma once

#include "RemoteMemoryObject.h"
#include <vector>


class ItemStatsDetailInfo
{
public:
	ItemStatsDetailInfo();

	std::wstring	statsName;
	int				statsValue;

};

class ItemMod : public RemoteMemoryObject
{
public:
	ItemMod();
	void									Update(uintptr_t	addrPointer);
	void									ParseName(uintptr_t addrPointer);
	std::wstring							ReplaceAll(std::wstring &str, const std::wstring& from, const std::wstring& to);
	int										IndexofAny(std::wstring & str);
	std::vector<ItemStatsDetailInfo>		GetItemStatsDetailInfo(DWORD dwItemModsHashKey);

	std::wstring							name;
	std::wstring							rawname;
	std::wstring							displayname;

	int										StatsValue[4];
	std::vector<ItemStatsDetailInfo>		lstStatsDetailInfo;

};

