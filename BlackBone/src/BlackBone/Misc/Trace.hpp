#pragma once

#include <cstdio>
#include <cstdlib>

#pragma warning(push)
#pragma warning(disable : 4091)
#include <DbgHelp.h>
#pragma warning(pop)

namespace blackbone
{

#ifndef BLACKBONE_NO_TRACE
#define 	LOGFILE_PATH	 L"D:\\Blackbone.log"

inline void DoTraceV( const char* fmt, va_list va_args )
{
    char buf[2048], userbuf[1024];
    vsprintf_s( userbuf, fmt, va_args );
    sprintf_s( buf, "BlackBone: %s\r\n", userbuf );
//     OutputDebugStringA( buf );
// 
// #ifdef CONSOLE_TRACE
//     printf_s( buf );
// #endif

	FILE	*fp;
	WCHAR	szFile[MAX_PATH];

	swprintf(szFile, LOGFILE_PATH);
	fp = _wfopen(szFile, L"a+");
	if (!fp)
		return;
	fprintf(fp, buf);
	fclose(fp);
}

inline void DoTraceV( const wchar_t* fmt, va_list va_args )
{
    wchar_t buf[2048], userbuf[1024];
	char aszBuf[4096];
    vswprintf_s( userbuf, fmt, va_args );
    swprintf_s( buf, L"BlackBone: %ls\r\n", userbuf );
//     OutputDebugStringW( buf );
// 
// #ifdef CONSOLE_TRACE
//     wprintf_s( buf );
// #endif

	FILE	*fp;
	WCHAR	szFile[MAX_PATH];

	swprintf(szFile, LOGFILE_PATH);
	fp = _wfopen(szFile, L"a+");
	if (!fp)
		return;
	WideCharToMultiByte(CP_ACP, 0, buf, (int)wcslen(buf) + 1, aszBuf, (int)wcslen(buf) * 2 + 2, 0, 0);
	fprintf(fp, aszBuf);
	fclose(fp);
}

template<typename Ch>
inline void DoTrace( const Ch* fmt, ... )
{
    va_list va_args;
    va_start( va_args, fmt );
    DoTraceV( fmt, va_args );
    va_end( va_args );
}

#define BLACKBONE_TRACE(fmt, ...) DoTrace(fmt, ##__VA_ARGS__)

#else
#define BLACKBONE_TRACE(fmt, ...) 
#endif

}