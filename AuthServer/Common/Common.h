#pragma once

#define BOOST_CONFIG_SUPPRESS_OUTDATED_MESSAGE

#define MSG_LOG		WM_USER + 0x1700

extern WCHAR		g_pszLogPath[MAX_PATH];

void LogFile(WCHAR *szFormat, ...);