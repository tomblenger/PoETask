// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <intrin.h>
#include "Common.h"
#include "XorStr.h"
#include "defs.h"
#include "PoEAttach.h"
#include "Positioned.h"
#include "GameLogic.h"
#include "Patch.h"

// Macros
#define		MAX_BREAK					100

#define		HARDWAREBREAK(x, y)			*(&pContext->Dr0+x) = y; pContext->Dr7 |= 0x03<<(x*2)
#define		MOVREG(x,y)					pContext->x = pContext->y; pContext->Rip += 3
#define		PUSHREG(x)					pContext->Rsp -= 8; *(DWORD64 *)pContext->Rsp = pContext->x; pContext->Rip++
#define		PUSHREG64(x)				pContext->Rsp -= 8; *(DWORD64 *)pContext->Rsp = pContext->x; pContext->Rip += 2
#define		PUSHNEAR(x)					pContext->Rsp -= 8; *(DWORD64 *)pContext->Rsp = x; pContext->Rip += 2
#define		PUSHFAR(x)					pContext->Rsp -= 8; *(DWORD64 *)pContext->Rsp = x; pContext->Rip += 5
#define		POP(x)						pContext->x = *(DWORD64 *)pContext->Rsp; pContext->Rsp += 8; pContext->Rip++
#define		POP64(x)					pContext->x = *(DWORD64 *)pContext->Rsp; pContext->Rsp += 8; pContext->Rip += 2
#define		ADD(x, y)					pContext->x += y; pContext->Rip += 3
#define		NOP(x)						pContext->Rip += x
#define		RETN(x)						pContext->Rip = *(DWORD64 *)pContext->Rsp; pContext->Rsp += 8+x
#define		PARAM(x)					(*(DWORD64 *)(pContext->Rsp+x*8))

DWORD64		g_pdwBreakAddr[MAX_BREAK];
BYTE   		g_pbRestoreCode[MAX_BREAK];

void		OnAttach();
void		ErasePEHeader(HMODULE hModule);
void		DumpStack(DWORD64 dwRSP, DWORD64 dwDepth);
LONG		VectoredHandler(struct _EXCEPTION_POINTERS *ExceptionInfo);
DWORD64		SetBreak(DWORD dwIndex, DWORD64 dwAddr);
void		RestoreBreak(DWORD dwIndex);
DWORD64		SetBreak(DWORD dwIndex, WCHAR *szDLL, char *aszFunc, DWORD64 dwOffset);
int			StringToBytes(BYTE *pbBuffer, WCHAR *szBuffer);
void		BytesToString(WCHAR *szBuffer, BYTE *pbBuffer, int nLen);


typedef LRESULT
(CALLBACK * _WindowProcW)(
	_In_ HWND hWnd,
	_In_ UINT Msg,
	_In_ WPARAM wParam,
	_In_ LPARAM lParam);

_WindowProcW orgWindowProc;

void OnDetach()
{
	Log(L"Detach");
	if (g_vectoredHandler)
	{
		for (int i = 0; i < MAX_BREAK; i++)
		{
			if (g_pdwBreakAddr[i])
			{
				RestoreBreak(i);
			}
		}
		RemoveVectoredExceptionHandler(g_vectoredHandler);
	}

	if (orgWindowProc)
	{
		SetWindowLongPtr(g_hWndMain, GWLP_WNDPROC, (LONG_PTR)orgWindowProc);
		orgWindowProc = 0;
	}

	KillTimer(g_hWndMain, 0x20190831);
}

BYTE g_pbAllocCode[100] =
{
	0x65, 0x48, 0x8B ,0x04 ,0x25  ,0x58 ,0x00 ,0x00 ,0x00,
	0xC3,
};

LPVOID g_pFuncAlloc;

void InitAlloc()
{
	g_pFuncAlloc = VirtualAlloc(NULL, 0x1000, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (!g_pFuncAlloc)
	{
		Log(L"VirtualAlloc failed!");
		return;
	}
	//Log(L"1 %p", g_pFuncAlloc);
	memcpy(g_pFuncAlloc, g_pbAllocCode, 10);
	//Log(L"2 %p", *(DWORD64 *)g_pFuncAlloc);
	*(DWORD64 *)g_pFuncAlloc = 0x5825048B4865;
	*(WORD *)((BYTE *)g_pFuncAlloc + 8) = 0xC300;
	//Log(L"3 %p", *(DWORD64 *)g_pFuncAlloc);
	DWORD64 gs58 = ((DWORD64(*)())g_pFuncAlloc)();
	//Log(L"   %p", gs58);
}

DWORD64 GetGS58()
{
	if (!g_pFuncAlloc)
		return 0;
	return ((DWORD64(*)())g_pFuncAlloc)();
}

DWORD64 g_dwTempAddr;
DWORD g_dwNumTest, g_dwFirst;
DWORD64 qwRcx, qwRdx, qwR8, qwR9;

LRESULT CALLBACK  OnWndEvent(HWND	hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_KEYUP:
	{
		switch (wParam)
		{
		case VK_NUMPAD0:
		{
			//Log(L"Num0");
			DumpStatus();
		}
		break;
		case VK_NUMPAD1:
		{
			g_bTest1 = !g_bTest1;
			Log(L"g_bTest1, %d", g_bTest1);
		}
		break;
		case VK_NUMPAD2:
		{
			g_bTest2 = !g_bTest2;
			Log(L"g_bTest2, %d", g_bTest2);
		}
		break;
		case VK_NUMPAD3:
		{
			Test3();
		}
		break;
		case VK_NUMPAD4:
		{
			g_bTestLog = !g_bTestLog;
			Log(L"TestLog = %d", g_bTestLog);
		}
		break;
		case VK_NUMPAD5:
		{
			Test5();
		}
		break;
		case VK_NUMPAD6:
		{
			Test6();
		}
		break;
		case VK_NUMPAD7:
		{
			Test7();
		}
		break;
		case VK_NUMPAD8:
		{
			Test8();
		}
		break;
		case VK_NUMPAD9:
		{
#ifdef POE_TEST
			OnDetach();
#endif
		}
		break;
		case VK_INSERT:
		{
			GameLogic::Instance->ToggleRun(1);
		}
		break;
		case VK_DELETE:
		{
			GameLogic::Instance->ToggleRun(0);
		}
		break;
		}
	}
	break;
	}
	if (orgWindowProc)
		return orgWindowProc(hWnd, uMsg, wParam, lParam);
	return 0;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
	{
		//MessageBox(NULL, L"Success", L"", 0);
		MyBase = (uintptr_t)hModule;
		g_szBuffer = (WCHAR *)malloc(0x10000);
		g_aszBuffer = (char *)malloc(0x10000);
		DisableThreadLibraryCalls(hModule);
		g_pParam = new GAMEPARAM;
#ifdef POE_TEST
		g_pParam->nConnection = 0;
#else
		memcpy(g_pParam, (BYTE*)lpReserved + 8, sizeof(GAMEPARAM));
#endif
		ErasePEHeader(hModule);
		OnAttach();
	}
	break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

void WaitThreadForWindow()
{
	g_hWndMain = NULL;

	while (true)
	{
#ifndef POE_TEST
		Sleep(3000);
#endif
		
		HWND hCurWnd = NULL;
		do
		{
			hCurWnd = FindWindowEx(NULL, hCurWnd, NULL, L"Path of Exile");
			DWORD dwProcessID = 0;
			GetWindowThreadProcessId(hCurWnd, &dwProcessID);
			if (dwProcessID == GetCurrentProcessId())
			{
				g_hWndMain = hCurWnd;
				break;
			}
		} while (hCurWnd != NULL);

		if (g_hWndMain)
		{
			orgWindowProc = (_WindowProcW)GetWindowLongPtr(g_hWndMain, GWLP_WNDPROC);
			POE_TRACE(L"org %p", FROM_POE((DWORD64)orgWindowProc));
			SetWindowLongPtr(g_hWndMain, GWLP_WNDPROC, (LONG_PTR)OnWndEvent);
			SetTimer(g_hWndMain, 0x20190831, 100, (TIMERPROC)MainTimer);
			break;
		}
	}
	
#ifndef POE_TEST
	Sleep(2000);
#endif

	BasePOE = (uintptr_t)GetModuleHandle(NULL);
	Initialize();

	//SetUnhandledExceptionFilter(NULL);
	g_vectoredHandler = AddVectoredExceptionHandler(999, (PVECTORED_EXCEPTION_HANDLER)VectoredHandler);
	RaiseException(0x20181213, 0, 0, 0);
}

void InitPCI()
{
	SYSTEMTIME	tm;
	HANDLE	hParam;
	WCHAR	szMapParam[MAX_PATH];

	hParam = OpenFileMapping(FILE_MAP_ALL_ACCESS, 0, MAINSETTING_MAPPING_NAME);
	g_pMainSettingApp = (GAMEMAINSETTING *)MapViewOfFile(hParam, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	g_pMainSettingDLL = g_pMainSettingApp + 1;
	memcpy(g_pMainSettingDLL, g_pMainSettingApp, sizeof(GAMEMAINSETTING));
	swprintf(szMapParam, L"%s%d", GAMESETTING_MAPPING_NAME, g_pParam->nConnection);
	hParam = OpenFileMapping(FILE_MAP_ALL_ACCESS, 0, szMapParam);
	g_pGameSettingApp = (GAMESETTING *)MapViewOfFile(hParam, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	g_pGameSettingDLL = g_pGameSettingApp + 1;
	memcpy(g_pGameSettingDLL, g_pGameSettingApp, sizeof(GAMESETTING));
	swprintf(szMapParam, L"%s%d", GAMEINFO_MAPPING_NAME, g_pParam->nConnection);
	hParam = OpenFileMapping(FILE_MAP_ALL_ACCESS, 0, szMapParam);
	g_pGameInfoDLL = (GAMEINFO *)MapViewOfFile(hParam, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	memset(g_pGameInfoDLL, 0, sizeof(GAMEINFO));
	swprintf(szMapParam, L"%s%d", EVENT_NOTIFY_NAME, g_pParam->nConnection);
	g_hEventNotifyDLL = OpenEvent(EVENT_ALL_ACCESS, NULL, szMapParam);
	swprintf(szMapParam, L"%s%d", EVENT_ANSWER_NAME, g_pParam->nConnection);
	g_hEventAnswerDLL = OpenEvent(EVENT_ALL_ACCESS, NULL, szMapParam);
	//swprintf(szMapParam, L"%s", EVENT_WINDOW_NAME);
	//g_hWindowEvent = CreateEvent(NULL, NULL, NULL, szMapParam);
	GetLocalTime(&tm);
	swprintf(g_pszLogPath, L"%s\\Log\\%d\\%02d월%02d일%02d시%02d분%02d초.log", g_pGameSettingDLL->szAppPath, g_pParam->nConnection, tm.wMonth, tm.wDay, tm.wHour, tm.wMinute, tm.wSecond);
	Log(L"Init");
	Log(L"*****");

	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)WaitThreadForWindow, NULL, 0, NULL);
}

void OnAttach()
{
	InitPCI();
	//InitAlloc();
}

void MyZeroMemory(void * ptr, size_t size)
{
	DWORD dwOld;
	VirtualProtect(ptr, size, PAGE_READWRITE, &dwOld);
	ZeroMemory(ptr, size);
	//VirtualProtect(ptr, size, dwOld, &dwOld);
}

void WipePEGarbage(uintptr_t base)
{
	auto dosHeaders = (IMAGE_DOS_HEADER*)(base);
	auto pINH = (IMAGE_NT_HEADERS*)(base + dosHeaders->e_lfanew);

	auto pSectionHeader = (IMAGE_SECTION_HEADER*)(pINH + 1);

	//dprintf(E("[censored]ing the PE, base: 0x%p, size: 0x%X"), base, pINH->OptionalHeader.SizeOfImage);

	auto dir = pINH->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
	auto iat = (IMAGE_IMPORT_DESCRIPTOR*)(base + dir.VirtualAddress);

	for (; iat->Name; ++iat)
	{
		auto modName = (char*)(base + (uintptr_t)iat->Name);
		auto entry = (IMAGE_THUNK_DATA64*)(base + iat->OriginalFirstThunk);

		for (uintptr_t index = 0; entry->u1.AddressOfData; index += sizeof(uintptr_t), ++entry)
		{
			auto pImport = (IMAGE_IMPORT_BY_NAME*)(base + entry->u1.AddressOfData);
			auto importName = pImport->Name;
			//auto x = E("Wiping import %s");
			//dprintf(x, importName);
			auto len = strlen(importName);
			MyZeroMemory(importName, len);
		}

		//auto x = E("Wiping import module %s");
		//dprintf(x, modName);

		auto len = strlen(modName);
		MyZeroMemory(modName, len);
	}

	for (int i = 0; i < pINH->FileHeader.NumberOfSections; i++)
	{
		auto section = pSectionHeader[i];
		auto name = section.Name;
		auto rva = section.VirtualAddress;
		auto size = section.SizeOfRawData;

		auto secBase = (uintptr_t)base + rva;
		auto secEnd = secBase + size;

		if (strstr((const char*)name, ".rdata"))
		{
			uintptr_t shitBase = 0;

			for (uintptr_t ptr = secBase; ptr < secEnd - 4; ptr++)
			{
				auto str = (char*)ptr;
				if (str[0] == 'G' && str[1] == 'C' && str[2] == 'T' && str[3] == 'L') // whatever that "GCTL" shit is, we gotta clean it up
					shitBase = ptr;
			}
			auto shitSize = 676; // magic number. Change if not enough
			if (shitBase)
			{
				MyZeroMemory((void*)shitBase, shitSize);
				//dprintf(E("Cleaned GCTL"));
			}
			else
			{
				//dprintf(E("Couldn't find GCTL shit"));
			}
		}
		else if (
			strstr((const char*)name, ".rsrc")
			|| strstr((const char*)name, ".reloc")
			/*|| strstr((const char*)name, ".pdata")*/) // assuming we need exception support.
		{
			//dprintf(E("Wiping section %s"), name);
			MyZeroMemory((void*)secBase, size);
		}
		else if (strstr((const char*)name, ".data")) // this particular meme can be unstable
		{
			//dprintf(E("Wiping C++ exception data"));
			MyZeroMemory((void*)(secEnd - 0x1B7), 0x1B7);
			//dprintf(E("Wiped."));
		}
	}

	MyZeroMemory((void*)base, pINH->OptionalHeader.SizeOfHeaders);
	//dprintf(E("Wiped the headers. Done!"));
}

void ErasePEHeader(HMODULE hModule)
{
	//PIMAGE_DOS_HEADER	pDOSHdr;
	//PIMAGE_NT_HEADERS	pNTHdr;
	//DWORD				dwOld;
	//
	//if (hModule)
	//{
	//	// Erase PE header
	//	pDOSHdr = (PIMAGE_DOS_HEADER)hModule;
	//	if (pDOSHdr->e_magic == IMAGE_DOS_SIGNATURE)
	//	{
	//		pNTHdr = (PIMAGE_NT_HEADERS)((BYTE*)hModule + pDOSHdr->e_lfanew);
	//		if (pNTHdr)
	//		{
	//			VirtualProtect((LPVOID)hModule, pNTHdr->OptionalHeader.SizeOfHeaders, PAGE_EXECUTE_READWRITE, &dwOld);
	//			memset(hModule, 0, pNTHdr->OptionalHeader.SizeOfHeaders);
	//			VirtualProtect((LPVOID)hModule, pNTHdr->OptionalHeader.SizeOfHeaders, dwOld, &dwOld);
	//		}
	//	}
	//}

	//WipePEGarbage((uintptr_t)hModule);
}

DWORD64 g_fileHandle, g_bDataBreak;

LONG	WINAPI VectoredHandler(struct _EXCEPTION_POINTERS *ExceptionInfo)
{
	int				nBreak;
	//DWORD			dwAddr;
	PCONTEXT		pContext = (PCONTEXT)ExceptionInfo->ContextRecord;

	if (ExceptionInfo->ExceptionRecord->ExceptionCode == 0x20181213)
	{
		POE_TRACE(L"BasePOE : %p, %p", BasePOE, MyBase);

		//SetBreak(0, TO_POE(ADDR_REF_MAP_BLIGHT));
		//SetBreak(1, TO_POE(0x000000000109F629));
		//SetBreak(2, TO_POE(0x0000000000596A00));
		//SetBreak(3, TO_POE(0x00000000003C4A40));

		// Map
		//SetBreak(4, TO_POE(0x00000000003C13B0));	// MapUI draw
		//SetBreak(5, TO_POE(0x0000000000C7BFC0));	// UI all text draw ? 
		//SetBreak(6, TO_POE(0x00000000003C1D1F));	// NPC iterate
		//SetBreak(7, TO_POE(0x00000000003C191B));
		//SetBreak(8, TO_POE(0x00000000003C1636));
		//SetBreak(9, TO_POE(0x000000000109D4F0));	// Map UI - drawinternal
		//SetBreak(10, TO_POE(0x000000000109E6DB));
		//SetBreak(11, TO_POE(0x000000000109DE02));
		//SetBreak(12, TO_POE(0x000000000109E330));
		//SetBreak(13, TO_POE(0x000000000109E5FE));
		//SetBreak(14, TO_POE(0x0000000001009780));
		//SetBreak(15, TO_POE(0x0000000001007BA0));
		//SetBreak(16, TO_POE(0x0000000001007B76));
		//SetBreak(17, TO_POE(0x0000000001005570));	// All Draw ??
		//SetBreak(18, TO_POE(0x00000000003C10D0));	// Not called
		//SetBreak(19, TO_POE(0x00000000003C24EC));
		//SetBreak(20, TO_POE(0x00000000003C1488));

		//SetBreak(21, TO_POE(0x000000000109F658));
		//SetBreak(22, TO_POE(0x0000000001006194));
		//SetBreak(23, TO_POE(0x00000000010061D6));
		//SetBreak(24, TO_POE(0x000000000109F660));
		//SetBreak(25, TO_POE(0x0000000001007609));

		//SetBreak(26, L"kernel32.dll", "CreateFileW", 0);
		//SetBreak(28, L"kernel32.dll", "CloseHandle", 0);
		//SetBreak(30, L"kernel32.dll", "ReadFile", 0);
		//SetBreak(32, TO_POE(0x0000000001348730));
		//SetBreak(33, TO_POE(0x0000000001346A0F));
		//SetBreak(34, TO_POE(0x00000000013459B7));
		//SetBreak(35, TO_POE(0x0000000000507D2F));
		//SetBreak(36, TO_POE(0x0000000000507CC0));
		//SetBreak(37, TO_POE(0x0000000001098D40));
		//SetBreak(38, TO_POE(0x0000000000DB7C50));
		//SetBreak(39, TO_POE(0x0000000000A808B0));
		//SetBreak(40, TO_POE(0x0000000000A7E6F0));

		// Quest
		//SetBreak(41, TO_POE(0x000000000045D370));
		//SetBreak(42, TO_POE(0x000000000090C190));
		//SetBreak(43, TO_POE(0x000000000044EEE0));

		return EXCEPTION_CONTINUE_EXECUTION;
	}

	for (nBreak = 0; nBreak < MAX_BREAK; nBreak++)
	{
		if (pContext->Rip == g_pdwBreakAddr[nBreak])
			break;
	}
	if (nBreak >= MAX_BREAK)
	{
		if (ExceptionInfo->ExceptionRecord->ExceptionCode == EXCEPTION_ACCESS_VIOLATION)
		{
			Log(L"EX: %08X %p %p %p Rcx=0x%p", ExceptionInfo->ExceptionRecord->ExceptionCode, pContext->Rip, FROM_POE(pContext->Rip), pContext->Rip - MyBase, pContext->Rcx);
#ifdef POE_RELEASE
			TerminateProcess(GetCurrentProcess(), 0);
#endif
		}
#ifndef POE_RELEASE
		else if (g_bDataBreak)
			//if (ExceptionInfo->ExceptionRecord->ExceptionCode == EXCEPTION_BREAKPOINT)
		{
			if ((pContext->Dr6 >> 2) & 0x01)			//write break
			{
				Log(L"HardWareWrite %p %p %08X", pContext->Rip, FROM_POE(pContext->Rip));
				if (!g_dwFirst)
				{
					g_dwFirst = 1;
					//DumpStack(pContext->Rsp, 0x100);
				}
				return EXCEPTION_CONTINUE_EXECUTION;
			}
		}
#endif
		
		return EXCEPTION_CONTINUE_SEARCH;
	}
#ifndef POE_RELEASE
	switch (nBreak)
	{
	case 43:
	{
		Log(L"43: %p %p %p", FROM_POE(PARAM(0)), pContext->Rcx, pContext->Rdx);
		PUSHREG64(Rdi);
	}
	break;
	case 42:
	{
		Log(L"42: %p %p %p %p %p", FROM_POE(PARAM(0)), pContext->Rcx, pContext->Rdx, pContext->R8, pContext->R9);
		*(DWORD64 *)(pContext->Rsp + 0x10) = pContext->Rdx;
		NOP(5);
	}
	break;
	case 41:
	{
		Log(L"41: %p", FROM_POE(PARAM(0)));
		*(DWORD64 *)(pContext->Rsp + 0x10) = pContext->Rdx;
		NOP(5);
	}
	break;
	case 40:
	{
		Log(L"40: %p %p %p %p %p %p %p", FROM_POE(PARAM(0)), pContext->Rcx, pContext->Rdx, pContext->R8, pContext->R9, PARAM(5), PARAM(6));
		PUSHREG64(R14);
	}
	break;
	case 39:
	{
		Log(L"39: %p %p %p", FROM_POE(PARAM(0)), pContext->Rcx, pContext->Rdx);
		PUSHREG64(Rbx);
	}
	break;
	case 38:
	{
		WCHAR g_szTemp[MAX_PATH];
		DWORD64 dwlen = *(DWORD64 *)(pContext->Rcx + 0x180) - *(DWORD64 *)(pContext->Rcx + 0x188);
		DWORD64 dwBuf = *(DWORD64 *)(pContext->Rcx + 0x1A8) + *(DWORD64 *)(pContext->Rcx + 0x188);
		BytesToString(g_szTemp, (BYTE *)dwBuf, (int)dwlen);
		if (dwlen && *(DWORD64 *)(pContext->Rcx + 0xF0))
		{
			if (wcsncmp(g_szTemp, L"0001", 4) && wcsncmp(g_szTemp, L"000E", 4) && g_bTest2)
				Log(L"send1 %p %04X %s", FROM_POE(PARAM(0)), dwlen, g_szTemp);
		}
		else
		{
			if (g_bTest2)
				Log(L"send2 %p %04X %s", FROM_POE(PARAM(0)), dwlen, g_szTemp);
		}
		MOVREG(Rax, Rsp);
	}
	break;
	case 37:
	{
		Log(L"37: %p %p %p %p %p", FROM_POE(PARAM(0)), pContext->Rcx, pContext->Rdx, pContext->R8, pContext->R9);
		PUSHREG64(Rbp);
	}
	break;
	case 36:
	{
		Log(L"36: %p %p", FROM_POE(PARAM(0)), pContext->Rcx);
		//if (g_bTest1)
		//{
		//	pContext->Rax = 0;
		//	RETN(0);
		//	break;
		//}
		*(DWORD64 *)(pContext->Rsp + 8) = pContext->Rcx;
		NOP(3);
	}
	break;
	case 35:
	{
		Log(L"35 %p %p", pContext->Rax, *(DWORD64 *)pContext->Rax);
		pContext->Rdx = *(DWORD64 *)pContext->Rax;
		NOP(3);
	}
	break;
	case 34:
	{
		Log(L"34 %p", pContext->R15);
		//if (!g_bDataBreak)
		//{
		//	pContext->Dr2 = pContext->R15;
		//	Log(L"Break Dr %p", pContext->Dr2);
		//	pContext->Dr7 = 0x0B000010;
		//	g_bDataBreak = 1;
		//}
		pContext->Rdi += *(DWORD *)(pContext->Rsp + 0xB8);
		NOP(7);
	}
	break;
	case 33:
	{
		Log(L"33");
		Log(L"33 %s", (WCHAR *)pContext->R14);
		pContext->R14 = *(DWORD *)(pContext->Rbp - 0x55);
		NOP(4);
	}
	break;
	case 32:
	{
		Log(L"32:");
		if (!pContext->Dr2)
		{
			pContext->Dr2 = *(DWORD64 *)(pContext->Rax + 0x08);
			Log(L"Break Dr %p", pContext->Dr2);
			pContext->Dr7 = 0x0B000010;
		}
		pContext->Rdx = *(DWORD *)(pContext->Rax + 0x10);
		NOP(3);
	}
	break;
	case 31:
	{
		RestoreBreak(31);
		SetBreak(30, L"kernel32.dll", "ReadFile", 0);
	}
	break;
	case 30:
	{
		if (PARAM(0) < MyBase || PARAM(0) > MyBase + 0x100000)
		{
			if (g_fileHandle && pContext->Rcx == g_fileHandle)
			{
				Log(L"ReadFile %p %p", FROM_POE(PARAM(0)), pContext->Rcx);
				//DumpStack(pContext->Rsp, 0x80);
			}
		}
		RestoreBreak(30);
		SetBreak(31, PARAM(0));
	}
	break;
	case 29:
	{
		RestoreBreak(29);
		SetBreak(28, L"kernel32.dll", "CloseHandle", 0);
	}
	break;
	case 28:
	{
		if (PARAM(0) < MyBase || PARAM(0) > MyBase + 0x100000)
		{
			if (g_fileHandle && pContext->Rcx == g_fileHandle)
			{
				Log(L"CloseHandle %p %p", FROM_POE(PARAM(0)), pContext->Rcx);
				g_fileHandle = 0;
			}
		}
		RestoreBreak(28);
		SetBreak(29, PARAM(0));
	}
	break;
	case 27:
	{
		RestoreBreak(27);
		SetBreak(26, L"kernel32.dll", "CreateFileW", 0);
		if (g_fileHandle == 1)
		{
			g_fileHandle = pContext->Rax;
			Log(L"file %p", g_fileHandle);
		} 
	}
	break;
	case 26:
	{
		if (PARAM(0) < MyBase || PARAM(0) > MyBase + 0x100000)
		{
			Log(L"CreateFileW %p %s", FROM_POE(PARAM(0)), (WCHAR *)pContext->Rcx);
			if (wcsstr((WCHAR *)pContext->Rcx, L"Minimap/"))
			{
				g_fileHandle = 1;
			}
		}
		RestoreBreak(26);
		SetBreak(27, PARAM(0));
	}
	break;
	case 25:
	{
		if (g_dwFirst)
		{
			Log(L"25: %d", *(BYTE *)(pContext->R10 + 0x8C));
			g_dwFirst = 0;
		}
		pContext->Rsi++;
		NOP(2);
	}
	break;
	case 24:
	{
		qwRcx = **(DWORD64 **)(pContext->Rbx + 0x2E0);
		qwRdx = *(BYTE *)(*(DWORD64 *)(pContext->Rbx + 0x300) + 4);
		qwR8 = *(DWORD *)(*(DWORD64 *)(pContext->Rbx + 0x300) + 8);
		DWORD64 a5 = *(DWORD *)(*(DWORD64 *)(pContext->Rbx + 0x300) + 0x0C);

		if (!g_dwNumTest)
		{
			Log(L"24: %p %p %p %p %p", qwRcx, qwRdx, qwR8, qwR9, a5);
			DWORD64 v8 = 0x1380;
			if (!*(DWORD64 *)(qwRcx + 0xC8))
				v8 = 0x10480;
			DWORD64 v9 = *(DWORD64 *)GetGS58() + v8;
			pContext->Dr2 = (qwR8 + (qwRdx << 12) + v9);
			Log(L"Break Dr %p", pContext->Dr2);
			pContext->Dr7 = 0x0B000010;
			g_dwNumTest = 1;
		}
		//Log(L"24: ");
		//RestoreBreak(24);
		pContext->Rcx = *(DWORD64 *)(pContext->Rbx + 0x2E0);
		NOP(7);
	}
	break;
	case 23:
	{
		Log(L"23: ");
		//RestoreBreak(23);
		pContext->Rcx += pContext->R8;
		NOP(3);
	}
	break;
	case 22:
	{
		Log(L"22: ");
		//RestoreBreak(22);
		pContext->Rsp += 0x28;
		NOP(4);
	}
	break;
	case 21:
	{
		pContext->Rcx = *(DWORD64 *)pContext->Rcx;
		if (g_dwNumTest < 5)
		{
			Log(L"21: %f %d %p %p", *(float *)pContext->R9, *(BYTE *)(pContext->Rcx + 0xC1), pContext->Rdx, pContext->R8);
			g_dwNumTest++;
			qwRcx = pContext->Rcx;
			qwRdx = pContext->Rdx;
			qwR8 = pContext->R8;
			qwR9 = pContext->R9;
		}

		//if (g_bTest1)
		//	*(float *)pContext->R9 = 0.5001f;
		
		//RestoreBreak(21);
		NOP(3);
	}
	break;
	case 20:
	{
		Log(L"20:");
		pContext->Rcx = *(DWORD64 *)pContext->Rcx;
		NOP(3);
	}
	break;
	case 19:
	{
		Log(L"19: %p %p", pContext->Rbx, *(DWORD64 *)(pContext->Rbp + 0x10));
		MOVREG(Rcx, Rbx);
		if (g_bTest1)
		{
			pContext->Rip = TO_POE(0x003C2661);
			break;
		}
	}
	break;
	case 18:
	{
		Log(L"18: %p", FROM_POE(PARAM(0)));
		if (g_bTest1)
		{
			RETN(0);
			break;
		}
		PUSHREG64(Rdi);
	}
	break;
	case 17:
	{
		if (g_bTest1)
		{
			if (FROM_POE(PARAM(0)) == 0x00000000003C1490)
			{
				Log(L"17: %p", FROM_POE(PARAM(0)));
				RETN(0);
				break;
			}
		}
		PUSHREG64(Rdi);
	}
	break;
	case 16:
	{
		if (g_bTest1)
		{
			//if (FROM_POE(PARAM(0)) < 0x000000000109E63B && FROM_POE(PARAM(0)) > 0x000000000109DF65)
			//if (FROM_POE(PARAM(0)) == 0x000000000109E1D5)
			{
				Log(L"16: %p", FROM_POE(PARAM(0)));
				RETN(0);
				break;
			}
		}
		pContext->Rsp -= 0x38;
		NOP(4);
	}
	break;
	case 15:
	{
		//if (g_bTest1 && FROM_POE(PARAM(0)) < 0x000000000109DACB)
		if (g_bTest1)
		{
			//if (FROM_POE(PARAM(0)) == 0x000000000109D83B || FROM_POE(PARAM(0)) == 0x000000000109D7DC || FROM_POE(PARAM(0)) == 0x000000000109D7AF)
			if (FROM_POE(PARAM(0)) == 0x000000000109D7DC)
			//if (pContext->Rcx == 0x000001D868AADE00)
			{
				//Log(L"15: %p %p", FROM_POE(PARAM(0)), pContext->Rcx);
				RETN(0);
				break;
			}
		}
		pContext->Rsp -= 0x38;
		NOP(4);
	}
	break;
	case 14:
	{
		Log(L"14:");
		if (g_bTest1)
		{
			RETN(0);
			break;
		}
		*(DWORD64 *)(pContext->Rsp + 8) = pContext->Rbx;
		NOP(5);
	}
	break;
	case 13:
	{
		Log(L"13:");
		pContext->Rdx = *(DWORD64 *)(pContext->Rdi + 0x2D8);
		NOP(7);
		if (g_bTest1)
		{
			pContext->Rip = TO_POE(0x109F092);
		}
	}
	break;
	case 12:
	{
		Log(L"12:");
		pContext->Rdx = 1;
		NOP(5);
		if (g_bTest1)
		{
			pContext->Rip = TO_POE(0x109F092);
		}
	}
	break;
	case 11:
	{
		Log(L"11: %p", *(DWORD64 *)(pContext->Rdi + 0x288));
		pContext->Rax = *(DWORD64 *)(pContext->Rdi + 0x288);
		NOP(7);
		if (g_bTest1)
		{
			pContext->Rip = TO_POE(0x109F092);
		}
	}
	break;
	case 10:
	{
		Log(L"10: %p %p", *(DWORD64 *)(pContext->Rbp + 0x4A0), *(DWORD64 *)(pContext->Rbp + 0x4A8));
		pContext->R14 = *(DWORD64 *)(pContext->Rbp + 0x4A0);
		NOP(7);
		if (g_bTest1)
		{
			pContext->Rip = TO_POE(0x109F092);
			//pContext->R14 = *(DWORD64 *)(pContext->Rbp + 0x4A8);
		}
	}
	break;
	case 9:
	{
		//Log(L"9: %p %p %p %p %p", FROM_POE(PARAM(0)), pContext->Rcx, pContext->Rdx, pContext->R8, pContext->R9);
		//Log(L"    (%f %f), (%f %f), %f", *(float *)pContext->Rdx, *(float *)(pContext->Rdx + 4), *(float *)pContext->R8, *(float *)(pContext->R8 + 4), (float)pContext->R9);
		g_dwTempAddr = *(DWORD64 *)(pContext->Rcx + 384);
		if (g_bTest1)
		{
			//*(float *)pContext->Rdx = 3657.608643;
			//*(float *)(pContext->Rdx + 4) = 2081.521729;
			*(float *)pContext->R8 = 0;
			*(float *)(pContext->R8 + 4) = 0;
			//RETN(0);
			//break;
		}
		MOVREG(Rax, Rsp);
	}
	break;
	case 8:
	{
		Log(L"8: %08X", *(DWORD *)(pContext->Rcx + 0xCB));
		pContext->R15 = *(DWORD *)(pContext->Rcx + 0xCB);
		NOP(7);
		if (g_bTest1)
		{
			//pContext->R15 = 0;
		}
	}
	break;
	case 7:
	{
		Log(L"7 : %p %p", pContext->Rbx, pContext->R8);
		pContext->Rax = *(BYTE *)(pContext->Rbp + 0x640);
		NOP(6);
		if (g_bTest1)
		{
			//pContext->Rbx = pContext->R8;
			pContext->Rip = TO_POE(0x003C1D18);
		}
	}
	break;
	case 6:
	{
		Log(L"6 : %p %p", *(DWORD64 *)(pContext->R12 + 0x66B8), *(DWORD64 *)(pContext->R12 + 0x66C0));
		pContext->Rbx = *(DWORD64 *)(pContext->R12 + 0x66B8);
		NOP(8);
		if (g_bTest1)
		{
			pContext->Rip = TO_POE(0x003C21C0);
			//pContext->Rip = TO_POE(0x0000000000003C3488);
		}
	}
	break;
	case 5:
	{
		//Log(L"5: %p %p", FROM_POE(PARAM(0)), pContext->Rcx);
		if (g_bTest1)
		{
			RETN(0);
			break;
		}
		PUSHREG64(Rbp);
	}
	break;
	case 4:
	{
		Log(L"4: %p %p %p", FROM_POE(PARAM(0)), pContext->Rcx, pContext->Rdx);
		Log(L"    %f", *(float *)(pContext->Rcx + 0x204));
		if (g_bTest1)
		{
			//FVector playerPos, uiShiftSize;
			//playerPos.x = 4135.869629f;
			//playerPos.y = 2385.869629f;
			//uiShiftSize.x = -32.580608f;
			//uiShiftSize.y = -20.000000f;
			//reinterpret_cast<DWORD64(*)(DWORD64, FVector *, FVector *, float)>(TO_POE(0x109D4F0))(*(DWORD64 *)(pContext->Rcx + 0x1A8), &playerPos, &uiShiftSize, *(float *)(pContext->Rcx + 0x204));
			RETN(0);
			break;
		}
		*(DWORD64 *)(pContext->Rsp + 0x10) = pContext->Rdx;
		NOP(5);
	}
	break;
	case 3:
	{
		Log(L"3: %p %p", FROM_POE(PARAM(0)), pContext->Rcx);
		MOVREG(Rax, Rsp);
	}
	break;
	case 2:
	{
		Log(L"2: %p %p", FROM_POE(PARAM(0)), pContext->Rcx);
		PUSHREG64(Rbx);
	}
	break;
	case 1:
	{
		Log(L"1:");
		NOP(12);
	}
	break;
	case 0:
	{
		//Log(L"0: %d", *(WORD *)(pContext->Rbx + 0x78));
		*(WORD *)(pContext->Rbx + 0x78) = 0x100;
		//if (!pContext->Dr2)
		//{
		//	pContext->Dr2 = pContext->Rbx + 0x78;
		//	pContext->Dr7 = 0x07000010;
		//}
		NOP(6);
	}
	break;
	default:
		break;
	}

#endif


	return EXCEPTION_CONTINUE_EXECUTION;
}

void	DumpStack(DWORD64 dwRSP, DWORD64 dwDepth)
{
	DWORD	i;
	DWORD64 dwTemp = 0;

	for (i = 0; i < dwDepth; i++)
	{
		dwTemp = *(DWORD64 *)(dwRSP + i * 8);
		if (dwTemp > BasePOE)
		{
			dwTemp = dwTemp - BasePOE;
		}
		//else if (dwTemp > BaseLostark)
		//{
		//	dwTemp = dwTemp - BaseLostark;
		//}
		else
			dwTemp = 0;
		if (dwTemp == 0)
			Log(L"Stack %08X: %016llX", i * 8, *(DWORD64 *)(dwRSP + i * 8));
		else
			Log(L"Stack %08X: %016llX %016llX", i * 8, *(DWORD64 *)(dwRSP + i * 8), dwTemp);
	}
}

int	StringToBytes(BYTE *pbBuffer, WCHAR *szBuffer)
{
	int		i, nLen;
	WCHAR	szTemp[3], *szEnd;

	nLen = (int)wcslen(szBuffer);
	nLen /= 2;
	for (i = 0; i < nLen; i++)
	{
		wcsncpy(szTemp, szBuffer + i * 2, 2);
		szTemp[2] = 0;
		pbBuffer[i] = (BYTE)wcstol(szTemp, &szEnd, 16);
	}
	return nLen;
}

void	BytesToString(WCHAR *szBuffer, BYTE *pbBuffer, int nLen)
{
	int			i;

	for (i = 0; i < nLen; i++)
	{
		swprintf(szBuffer + i * 2, L"%02X", pbBuffer[i]);
	}
	szBuffer[i * 2] = 0;
}

DWORD64	SetBreak(DWORD dwIndex, DWORD64 dwAddr)
{
	DWORD		dwOldProtect = 0;
	//char hkBytes[] = { 0x8B, 0x04, 0x25, 0x00, 0x00, 0x00, 0x00 };//{ 0x31, 0xC0, 0xF7, 0xF0 };

	if (dwIndex >= MAX_BREAK)
		return 0;
	if (g_pdwBreakAddr[dwIndex])
		return 0;
	VirtualProtect((LPVOID)dwAddr, 0x01, PAGE_EXECUTE_READWRITE, &dwOldProtect);
	g_pbRestoreCode[dwIndex] = *(BYTE *)dwAddr;
	*(BYTE *)dwAddr = 0xCC;
	//memcpy((void *)dwAddr, hkBytes, sizeof(hkBytes));
	g_pdwBreakAddr[dwIndex] = dwAddr;
	VirtualProtect((LPVOID)dwAddr, 0x01, dwOldProtect, &dwOldProtect);
	return dwAddr;
}

void	RestoreBreak(DWORD dwIndex)
{
	DWORD		dwOldProtect = 0;

	if (dwIndex >= MAX_BREAK)
		return;
	VirtualProtect((LPVOID)g_pdwBreakAddr[dwIndex], 0x01, PAGE_EXECUTE_READWRITE, &dwOldProtect);
	*(BYTE *)g_pdwBreakAddr[dwIndex] = g_pbRestoreCode[dwIndex];
	g_pdwBreakAddr[dwIndex] = 0;
	g_pbRestoreCode[dwIndex] = 0;
	VirtualProtect((LPVOID)g_pdwBreakAddr[dwIndex], 0x01, dwOldProtect, &dwOldProtect);
}

DWORD64	SetBreak(DWORD dwIndex, WCHAR *szDLL, char *aszFunc, DWORD64 dwOffset)
{
	HMODULE		hMod = LoadLibrary(szDLL);
	DWORD64		dwAddr;

	if (!hMod)
		return 0;
	dwAddr = (DWORD64)GetProcAddress(hMod, aszFunc);
	if (!dwAddr)
		return 0;
	SetBreak(dwIndex, dwAddr + dwOffset);
	return dwAddr + dwOffset;
}
