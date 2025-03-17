#include "stdafx.h"
#include "Common.h"

WCHAR		g_szAppPath[MAX_PATH];
WCHAR		g_szGamePath[MAX_PATH];
GAMESETTING	*g_pGameSetting[MAX_ACCOUNT_NUM];
GAMEINFO	*g_pGameInfo[MAX_ACCOUNT_NUM];
HANDLE		g_hEventNotify[MAX_ACCOUNT_NUM], g_hEventAnswer[MAX_ACCOUNT_NUM];

DWORD				g_dwAppCount;
AppSettingInfo		g_pAppSetting[MAX_ACCOUNT_NUM];
AuthInfo		g_pAuth[MAX_ACCOUNT_NUM];
BYTE			g_bLogin;
WCHAR			g_szDeviceKey[MAX_PATH];

void LoadAppPath()
{
	int			i;

	GetModuleFileName(NULL, g_szAppPath, MAX_PATH);
	i = (int)wcslen(g_szAppPath);
	while (g_szAppPath[i] != L'\\')
		i--;
	g_szAppPath[i] = 0;
}
