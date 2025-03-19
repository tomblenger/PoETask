#pragma once

#include <Windows.h>


#define	AUTO_VERSION		L"3.8.0d.02"
#define	APP_MARK			"GlobalERA"

//#define		POE_RELEASE
#ifndef POE_RELEASE
#define		POE_TEST
#endif
//#define		DUMP_PROCESS
//#define		SDK_DUMP
//#define		AUTH
//#define		_PROTECT

#define		PI							3.1415926537f
#define		MAX_ACCOUNT_NUM				30
#define		MAINSETTING_MAPPING_NAME	textwonce(L"POE_GAMEMAINSETTING")
#define		GAMESETTING_MAPPING_NAME	textwonce(L"H264.VIDEO.FILE.MAP(POE)")
#define		GAMEINFO_MAPPING_NAME		textwonce(L"H264.VIDEO.FILE.MAPINFO(POE)")
#define		EVENT_NOTIFY_NAME			textwonce(L"H264.VIDEO.NOTIFY(POE)")
#define		EVENT_ANSWER_NAME			textwonce(L"H264.VIDEO.ANSWER(POE)")
#define		EVENT_WINDOW_NAME			textwonce(L"H264.VIDEO.WINDOW(POE)")

struct GAMEMAINSETTING
{
	DWORD	dwTemp;
	DWORD	dwTest;
	BYTE	pbReserve[4000];
};

struct GAMESETTING
{
	DWORD	dwCommand;
	WCHAR	szGameID[MAX_PATH];
	WCHAR	szGamePW[MAX_PATH];
	DWORD	dwServerNo;
	DWORD	dwCharNo;
	WCHAR	szAppPath[MAX_PATH];
	char	pad01[3];
};

struct GAMEINFO
{
	WCHAR	szLog[0x10000];
};

struct GAMEPARAM
{
	int nConnection;
	char pad00[4];
};

extern	HANDLE				g_hLauncherEvent;
extern	GAMEMAINSETTING		*g_pMainSettingApp;
extern	GAMEMAINSETTING		*g_pMainSettingDLL;
