#pragma once

#include "Common.h"

#define GEMINFO_COUNT	11

struct GemLevelDetailInfo
{
	int		gemLv;
	int		playerLv;
	int		intelligence;
	int		strength;
	int		dexterity;
};

struct GemLevelInfo
{
	WCHAR	*gemName;
	WCHAR	*gemKrName;
	std::vector<GemLevelDetailInfo>	gemLeveldetailInfos;
};

extern GemLevelInfo g_pGemInfoList[GEMINFO_COUNT];
