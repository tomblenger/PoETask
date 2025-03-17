#pragma once

#include "Global.h"

#ifdef _PROTECT
#include "ThemidaSDK/Include/C/ThemidaSDK.h"
#endif


#define MAX_NAME			100

#define MSG_LOGINACK		WM_USER + 0x1700
#define MSG_FORCECLOSE		WM_USER + 0x1701
#define MSG_LOG				WM_USER + 0x1702
#define MSG_DISCONNECT		WM_USER + 0x1703

struct AccountInfo
{
	DWORD dwUse;
	WCHAR szAutoId[MAX_NAME];
	WCHAR szAutoPw[MAX_NAME];
	WCHAR szGameId[MAX_NAME];
	WCHAR szGamePw[MAX_NAME];
	DWORD dwServerNo;
	DWORD dwCharacterNo;
	BYTE pReserved[400];
};

struct AppSettingInfo
{
	AccountInfo account;
};

struct AuthInfo
{
	DWORD dwLoginResult;
	DWORD dwID;
	WCHAR szExpiredTime[MAX_NAME];
	WCHAR szToken[MAX_NAME];
};

class CAuth;

extern WCHAR		g_szAppPath[MAX_PATH];
extern WCHAR		g_szGamePath[MAX_PATH];
extern GAMESETTING	*g_pGameSetting[MAX_ACCOUNT_NUM];
extern GAMEINFO		*g_pGameInfo[MAX_ACCOUNT_NUM];
extern HANDLE		g_hEventNotify[MAX_ACCOUNT_NUM], g_hEventAnswer[MAX_ACCOUNT_NUM];

extern DWORD				g_dwAppCount;
extern AppSettingInfo		g_pAppSetting[MAX_ACCOUNT_NUM];
extern AuthInfo		g_pAuth[MAX_ACCOUNT_NUM];
extern BYTE			g_bLogin;
extern WCHAR		g_szDeviceKey[MAX_PATH];

void LoadAppPath();
