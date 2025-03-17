#include "stdafx.h"
#include "Common.h"

WCHAR		g_szBuffer[0x10000];
char		g_aszBuffer[0x10000];
WCHAR		g_pszLogPath[MAX_PATH];
WCHAR		g_szTemp[0x10000];
FILE		*fp;

void	LogFile(WCHAR *szFormat, ...)
{
	va_list			arg;
	SYSTEMTIME		time;

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
			//fclose(fp);
			return;
		}
		//fp = _wfopen(g_pszLogPath, L"a+");
		if (!fp)
			return;
		sprintf(g_aszBuffer, "[%04d/%02d/%02d %02d:%02d:%02d:%03d]   ", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond, time.wMilliseconds);
		WideCharToMultiByte(CP_ACP, 0, (WCHAR *)g_szBuffer, (int)wcslen(g_szBuffer) + 1, g_aszBuffer + 28, 0x10000, 0, 0);
		fprintf(fp, g_aszBuffer);
		fprintf(fp, "\n");
		//fclose(fp);
	}
}