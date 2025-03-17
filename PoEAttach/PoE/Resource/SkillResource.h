#pragma once

#define PASSIVESKILL_RES_COUNT				2271
#define	PASSIVESKILL_LEVEL_RES_COUNT		16

struct PoEResPassiveSkill
{
	DWORD dwKey;
	WCHAR *szId;
	DWORD dwGraphID;
	WCHAR *szName;
};

struct PoEResPSKillByCharLevel
{
	BYTE	bCharLevel;
	WCHAR	*szPSkillName;
};


extern PoEResPassiveSkill	g_pResPassiveSkill[PASSIVESKILL_RES_COUNT];

PoEResPassiveSkill *   GetPassiveSkillNameByID(WORD wSkillID);
