#pragma once

#include <Windows.h>

#define  ITEM_MODS_COUNT	16425
#define  NOINDEX			0xFFFFFFFF

struct PoEResItemMods
{
	WCHAR * szModsId;
	DWORD dwItemModsHashKey;
	DWORD dwItemModTypeKey;
	DWORD dwItemStatsKey[4];
};

extern PoEResItemMods g_pResItemMods[ITEM_MODS_COUNT];

PoEResItemMods * GetItemMods(DWORD dwItemModsHashKey);
