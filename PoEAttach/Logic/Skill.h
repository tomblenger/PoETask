#pragma once

#include "Common.h"

struct DoSkillInfo
{
	DWORD generalNum;

	struct
	{
		WORD id;
		DWORD delay;
		int manaCost;
		WCHAR *name;
		BYTE isBuff;
		WCHAR *buffName;
		DWORD64 coolTime;
	} pSkillGeneral[MAX_COUNT];
};

struct DoForwardSkillInfo
{
	DWORD generalNum;

	struct
	{
		DWORD delay;
		int manaCost;
		WCHAR *name;
		DWORD64 coolTime;
	} pSkillGeneral[MAX_COUNT];
};

extern DoSkillInfo g_pSkillList_Witch;
extern DoForwardSkillInfo g_pForwardSkillList_Witch;
