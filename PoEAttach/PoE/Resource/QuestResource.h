#pragma once

#include <string>

#define			QUEST_RES_COUNT							173
#define			QUESTSTATES_RES_COUNT					1175
#define			QUESTSTATES_LIST_MAX_COUNT				10

struct PoEResQuest
{
	DWORD dwKey;
	WCHAR *id;
	BYTE bAct;
	WCHAR *szName;
};

struct PoEResQuestState
{
	DWORD dwQuestKey;
	BYTE bQuestStep;
	DWORD dwNumQuestStates;
	DWORD pdwQuestStatesList[QUESTSTATES_LIST_MAX_COUNT];
	DWORD dwNumData;
	DWORD pdwQuestDataList[QUESTSTATES_LIST_MAX_COUNT];
	WCHAR *szMessage;
	WCHAR *szMapPinsText;
};

extern	PoEResQuest	g_pResQuest[QUEST_RES_COUNT];
extern	PoEResQuestState g_pResQuestState[QUESTSTATES_RES_COUNT];

DWORD	GetKeyByIDFromResource(std::wstring id);