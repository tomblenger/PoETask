#pragma once

#define STATS_RES_COUNT				9978

struct PoEResStats
{
	DWORD dwID;
	WCHAR *szName;
	WCHAR *szDisplayName;
};

extern PoEResStats	g_pResStats[STATS_RES_COUNT];

WCHAR *   GetStatsDisplayNameByID(DWORD dwStatsID);
WCHAR *   GetStatsNameByID(DWORD dwStatsID);
