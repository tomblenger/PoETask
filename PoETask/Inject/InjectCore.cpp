#include "stdafx.h"
#include "InjectCore.h"

BOOL	ManualMap(
	DWORD dwProcessID, 
	LPVOID pModule, 
	size_t size, 
	eLoadFlags flags/* = NoFlags*/, 
	const std::wstring& moduleName/* = L""*/, 
	const char* funcName/* = ""*/, 
	CustomArgs_t *pArgs/* = nullptr*/)
{
	Process			target;

	if (target.Attach(dwProcessID) != STATUS_SUCCESS)
		return STATUS_PROCESS_NOT_IN_JOB;

	return ManualMap(target, pModule, size, flags, moduleName, funcName, pArgs);
}

BOOL	ManualMap(
	DWORD dwProcessID, 
	WCHAR *szDllPath, 
	eLoadFlags flags/* = NoFlags*/,
	const std::wstring& moduleName/* = L""*/,
	const char* funcName/* = ""*/,
	CustomArgs_t *pArgs/* = nullptr*/)
{
	Process			target;

	if (target.Attach(dwProcessID) != STATUS_SUCCESS)
		return STATUS_PROCESS_NOT_IN_JOB;

	return ManualMap(target, szDllPath, flags, moduleName, funcName, pArgs);
}

BOOL	ManualMap(
	WCHAR *szProcessName, 
	LPVOID pModule, 
	size_t size, 
	eLoadFlags flags/* = NoFlags*/,
	const std::wstring& moduleName/* = L""*/,
	const char* funcName/* = ""*/,
	CustomArgs_t *pArgs/* = nullptr*/)
{
	Process			target;

	if (target.Attach(szProcessName) != STATUS_SUCCESS)
		return STATUS_PROCESS_NOT_IN_JOB;

	return ManualMap(target, pModule, size, flags, moduleName, funcName, pArgs);
}

BOOL	ManualMap(
	HANDLE handle, 
	LPVOID pModule, 
	size_t size, 
	eLoadFlags flags/* = NoFlags*/,
	const std::wstring& moduleName/* = L""*/,
	const char* funcName/* = ""*/,
	CustomArgs_t *pArgs/* = nullptr*/)
{
	Process			target;

	if (target.Attach(handle) != STATUS_SUCCESS)
		return STATUS_PROCESS_NOT_IN_JOB;

	return ManualMap(target, pModule, size, flags, moduleName, funcName, pArgs);
}

BOOL	ManualMap(
	HANDLE handle, 
	WCHAR *szDllPath, 
	eLoadFlags flags/* = NoFlags*/,
	const std::wstring& moduleName/* = L""*/,
	const char* funcName/* = ""*/,
	CustomArgs_t *pArgs/* = nullptr*/)
{
	Process			target;

	if (target.Attach(handle) != STATUS_SUCCESS)
		return STATUS_PROCESS_NOT_IN_JOB;

	return ManualMap(target, szDllPath, flags, moduleName, funcName, pArgs);
}

BOOL	ManualMap(
	Process &target, 
	LPVOID pModule, 
	size_t size, 
	eLoadFlags flags/* = NoFlags*/,
	const std::wstring& moduleName/* = L""*/,
	const char* funcName/* = ""*/,
	CustomArgs_t *pArgs/* = nullptr*/)
{
	ModuleDataPtr	mod;
	NTSTATUS		status = ERROR_SUCCESS;

	// Actual injection
	auto injectedMod = target.mmap().MapImage(size, pModule, false, flags, nullptr, nullptr, pArgs, moduleName, funcName);
	if (!injectedMod)
		status = injectedMod.status;
	else
		mod = injectedMod.result();

	// Fix error code
	if (mod == nullptr && NT_SUCCESS(status))
		status = STATUS_DLL_NOT_FOUND;

	return status == STATUS_SUCCESS;
}

BOOL	ManualMap(
	Process &target,
	WCHAR *szPath, 
	eLoadFlags flags/* = NoFlags*/,
	const std::wstring& moduleName/* = L""*/,
	const char* funcName/* = ""*/,
	CustomArgs_t *pArgs/* = nullptr*/)
{
	ModuleDataPtr	mod;
	NTSTATUS		status = ERROR_SUCCESS;

	// Actual injection
	auto injectedMod = target.mmap().MapImage(std::wstring(szPath), flags, nullptr, nullptr, pArgs, moduleName, funcName);
	if (!injectedMod)
		status = injectedMod.status;
	else
		mod = injectedMod.result();

	// Fix error code
	if (mod == nullptr && NT_SUCCESS(status))
		status = STATUS_DLL_NOT_FOUND;

	return status == STATUS_SUCCESS;
}

BOOL	NativeInject(
	DWORD dwProcessID,
	const wchar_t *szPath)
{
	Process			target;

	if (target.Attach(dwProcessID) != STATUS_SUCCESS)
		return STATUS_PROCESS_NOT_IN_JOB;

	auto injectedMod = target.modules().Inject(szPath);
	if (!injectedMod)
		return FALSE;
	return TRUE;
}
