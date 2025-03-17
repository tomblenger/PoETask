#pragma once

#include <string>
#include <vector>
#include "Global.h"

#define TO_POE(x)				(BasePOE + x)
#define FROM_POE(x)				(x - BasePOE)

#define MAX_NAME				500
#define MAX_COUNT				200
#define	NOINDEX					0xFFFFFFFF
#define	DIST(u, v)				(sqrt(((float)(u).x-(float)(v).x)*((float)(u).x-(float)(v).x)+((float)(u).y-(float)(v).y)*((float)(u).y-(float)(v).y)) / 10.f)

#define WAYPOINT_NAME			L"Waypoint"
#define ENTRANCE_NAME			L"Entrance"
#define NPC_NAME				L"NPC"
#define QUESTOBJECT_NAME		L"QuestObject"
#define QUESTITEM_NAME			L"QuestItem"
#define PORTAL_NAME				L"Portal"
#define QUEST_COMPLETE_TEXT		L"Quest Complete"
#define STASH_ICON				L"StashPlayer"
#define STASH_NAME				L"Stash"
#define ITEM_WISDOM				L"Scroll of Wisdom"
#define ITEM_PORTAL				L"Portal Scroll"


#ifndef POE_RELEASE
#define POE_TRACE(fmt, ...) Log(fmt, ##__VA_ARGS__)
#else
#define POE_TRACE(...) 
#endif

#define TESTLOG(fmt, ...) if (g_bTestLog) {Log(fmt, ##__VA_ARGS__);}

extern HANDLE g_hWindowEvent;
extern PVOID g_vectoredHandler;
extern uintptr_t BasePOE, MyBase;
extern DWORD g_dwTick;
extern BYTE		g_bTest1, g_bTest2, g_bTest3, g_bTestLog;
extern BYTE		g_bAddException;
extern HWND g_hWndMain;
extern WCHAR *g_szBuffer;
extern char	*g_aszBuffer;
extern WCHAR g_pszLogPath[MAX_PATH];
extern HANDLE g_hEventNotifyDLL, g_hEventAnswerDLL;
extern GAMEPARAM *g_pParam;
extern GAMEINFO	*g_pGameInfoDLL;
extern GAMESETTING	*g_pGameSettingDLL;
extern GAMESETTING	*g_pGameSettingApp;

struct IVector
{
	int x;
	int y;
};

struct FVector
{
	float x;
	float y;
};



void Log(WCHAR *szFormat, ...);
void LogMy(WCHAR *szFormat, ...);
std::vector<std::wstring> SplitWideString(std::wstring source, wchar_t separator);
bool operator == (const struct IVector &a, const struct IVector &b);
bool operator != (const struct IVector &a, const struct IVector &b);