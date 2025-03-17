#include "stdafx.h"
#include <stdio.h>
#include <sstream>
#include <iostream>
#include "Common.h"

HANDLE g_hWindowEvent;
PVOID g_vectoredHandler = nullptr;
uintptr_t	BasePOE = 0, MyBase = 0;
DWORD		g_dwTick = 0;
BYTE		g_bTest1 = 0, g_bTest2 = 0, g_bTest3 = 0, g_bTestLog = 0;
BYTE		g_bMonSkill = 0;
BYTE		g_bAddException = 0;
HWND		g_hWndMain;
WCHAR		*g_szBuffer;
char		*g_aszBuffer;
WCHAR		g_pszLogPath[MAX_PATH] = L"D:\\1.txt";
HANDLE		g_hEventNotifyDLL, g_hEventAnswerDLL;
GAMEPARAM	*g_pParam;
GAMEINFO	*g_pGameInfoDLL;
GAMESETTING	*g_pGameSettingDLL;
GAMESETTING	*g_pGameSettingApp;
void * jmp_rsi_0 = nullptr;

void	Log(WCHAR *szFormat, ...)
{
	va_list			arg;

	// 	if(g_pGameSettingDLL->bSetting == 3)
	// 		return;

	va_start(arg, szFormat);
	vswprintf(g_szBuffer, szFormat, arg);
	va_end(arg);

#ifndef USE_TARGET
	SYSTEMTIME		time;
	FILE			*fp;

	GetLocalTime(&time);

	if (wcscmp(g_szBuffer, L"Init"))
	{
		WaitForSingleObject(g_hEventAnswerDLL, INFINITE);
		if (g_szBuffer[0])
		{
			swprintf(g_pGameInfoDLL->szLog, L"%02d:%02d:%02d:%03d   %s", time.wHour, time.wMinute, time.wSecond, time.wMilliseconds, g_szBuffer);
			//swprintf(g_pGameInfoDLL->szLog, L"%s", g_szBuffer);
		}
		else
		{
			g_pGameInfoDLL->szLog[0] = 0;
		}
		SetEvent(g_hEventNotifyDLL);
	}

	if (g_szBuffer[0])
	{
		if (!wcscmp(g_szBuffer, L"Init"))
		{
			fp = _wfopen(g_pszLogPath, L"w");
			if (!fp)
				return;
			fclose(fp);
			return;
		}
		fp = _wfopen(g_pszLogPath, L"a+");
		if (!fp)
			return;
		sprintf(g_aszBuffer, "%02d:%02d:%02d:%03d   ", time.wHour, time.wMinute, time.wSecond, time.wMilliseconds);
		WideCharToMultiByte(CP_ACP, 0, (WCHAR *)g_szBuffer, (int)wcslen(g_szBuffer) + 1, g_aszBuffer + 15, 0x10000, 0, 0);
		fprintf(fp, "%s", g_aszBuffer);
		fprintf(fp, "\n");
		fclose(fp);
	}
#else

	if (g_pParam->pTParam->procLog)
	{
		reinterpret_cast<void(__fastcall*)(WCHAR *)>(g_pParam->pTParam->procLog)(g_szBuffer);
	}

#endif
}

void	LogMy(WCHAR *szFormat, ...)
{
	va_list			arg;
	SYSTEMTIME		time;
	FILE			*fp;

	GetLocalTime(&time);

	// 	if(g_pGameSettingDLL->bSetting == 3)
	// 		return;

	va_start(arg, szFormat);
	vswprintf(g_szBuffer, szFormat, arg);
	va_end(arg);

	if (g_szBuffer[0])
	{
		if (!wcscmp(g_szBuffer, L"Init"))
		{
			fp = _wfopen(g_pszLogPath, L"w");
			if (!fp)
				return;
			fclose(fp);
			return;
		}
		fp = _wfopen(g_pszLogPath, L"a+");
		if (!fp)
			return;
		sprintf(g_aszBuffer, "%02d:%02d:%02d:%03d   ", time.wHour, time.wMinute, time.wSecond, time.wMilliseconds);
		WideCharToMultiByte(CP_ACP, 0, (WCHAR *)g_szBuffer, (int)wcslen(g_szBuffer) + 1, g_aszBuffer + 15, 0x10000, 0, 0);
		fprintf(fp, g_aszBuffer);
		fprintf(fp, "\n");
		fclose(fp);
	}
}

std::vector<std::wstring> SplitWideString(std::wstring source, wchar_t separator)
{
	std::vector<std::wstring> strings;

	std::wistringstream f(source);
	std::wstring s;
	while (std::getline(f, s, separator)) {
		strings.push_back(s);
	}

	return strings;
}

bool operator == (const struct IVector &a, const struct IVector &b)
{
	if (a.x == b.x && a.y == b.y)
		return true;
	return false;
}

bool operator != (const struct IVector &a, const struct IVector &b)
{
	if (a.x == b.x && a.y == b.y)
		return false;
	return true;
}
