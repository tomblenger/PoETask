#pragma once

#include <BlackBone/src/BlackBone/Config.h>
#include <BlackBone/src/BlackBone/Process/Process.h>
#include <BlackBone/src/BlackBone/PE/PEImage.h>
#include <BlackBone/src/BlackBone/Misc/Utils.h>

#include <iostream>

using namespace blackbone;

BOOL	ManualMap(
	DWORD dwProcessID, 
	LPVOID pModule, 
	size_t size, 
	eLoadFlags flags = NoFlags, 
	const std::wstring& moduleName = L"",
	const char* funcName = "", 
	CustomArgs_t *pArgs = nullptr);

BOOL	ManualMap(
	DWORD dwProcessID, 
	WCHAR	*szDllPath, 
	eLoadFlags flags = NoFlags, 
	const std::wstring& moduleName = L"", 
	const char* funcName = "", 
	CustomArgs_t *pArgs = nullptr);

BOOL	ManualMap(
	WCHAR *szProcessName, 
	LPVOID pModule, 
	size_t size, 
	eLoadFlags flags = NoFlags, 
	const std::wstring& moduleName = L"", 
	const char* funcName = "", 
	CustomArgs_t *pArgs = nullptr);

BOOL	ManualMap(
	WCHAR *szProcessName, 
	WCHAR *szDllPath, 
	eLoadFlags flags = NoFlags, 
	const std::wstring& moduleName = L"", 
	const char* funcName = "", 
	CustomArgs_t *pArgs = nullptr);

BOOL	ManualMap(
	HANDLE handle, 
	LPVOID pModule, 
	size_t size, 
	eLoadFlags flags = NoFlags, 
	const std::wstring& moduleName = L"", 
	const char* funcName = "", 
	CustomArgs_t *pArgs = nullptr);

BOOL	ManualMap(
	HANDLE handle, 
	WCHAR *szDllPath, 
	eLoadFlags flags = NoFlags, 
	const std::wstring& moduleName = L"", 
	const char* funcName = "", 
	CustomArgs_t *pArgs = nullptr);

BOOL	ManualMap(
	Process &target, 
	LPVOID pModule, 
	size_t size, 
	eLoadFlags flags = NoFlags, 
	const std::wstring& moduleName = L"", 
	const char* funcName = "", 
	CustomArgs_t *pArgs = nullptr);

BOOL	ManualMap(
	Process &target, 
	WCHAR *szPath, 
	eLoadFlags flags = NoFlags, 
	const std::wstring& moduleName = L"", 
	const char* funcName = "", 
	CustomArgs_t *pArgs = nullptr);

BOOL	NativeInject(
	DWORD dwProcessID,
	const wchar_t *szPath);
