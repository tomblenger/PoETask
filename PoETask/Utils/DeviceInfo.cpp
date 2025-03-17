#include "stdafx.h"
#include "DeviceInfo.h"

#include <Wincrypt.h>
#include <winsock2.h>
#include <iphlpapi.h>

#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "IPHLPAPI.lib")
#pragma warning(disable:4996)
  
void	PrintDebugString(WCHAR *szFormat, ...)
{
	//WCHAR			szBuffer[1024];
	//va_list			arg;
	//
	//
	//va_start(arg, szFormat);
	//vswprintf(szBuffer, szFormat, arg);
	//va_end(arg);
	//
	//OutputDebugString(szBuffer);
	//OutputDebugString(L"\n");
}

BOOL	GetCPUInfo(WCHAR *szCPU)
{
	int		CPUInfo[4] = { -1 };
	DWORD	i, nIds, nExIds;
	DWORD	dwCPUID1 = 0, dwCPUID2 = 0;

	szCPU[0] = 0;

	__cpuid(CPUInfo, 0);
	nIds = CPUInfo[0];

	for (i = 0; i <= nIds; ++i)
	{
		__cpuid(CPUInfo, i);
		if (i == 1)
		{
			dwCPUID1 = CPUInfo[0];
			dwCPUID2 = CPUInfo[3];
		}
	}

	char CPUBrandString[0x40];

	__cpuid(CPUInfo, 0x80000000);
	nExIds = CPUInfo[0];
	memset(CPUBrandString, 0, sizeof(CPUBrandString));

	// Get the information associated with each extended ID.
	for (i = 0x80000000; i <= nExIds; ++i)
	{
		__cpuid(CPUInfo, i);

		// Interpret CPU brand string and cache information.
		if (i == 0x80000002)
			memcpy(CPUBrandString, CPUInfo, sizeof(CPUInfo));
		else if (i == 0x80000003)
			memcpy(CPUBrandString + 16, CPUInfo, sizeof(CPUInfo));
		else if (i == 0x80000004)
			memcpy(CPUBrandString + 32, CPUInfo, sizeof(CPUInfo));
	}

	swprintf(szCPU, L"%08X%08X %S", dwCPUID1, dwCPUID2, CPUBrandString);

	return TRUE;
}

BOOL	GetNetworkInfo(WCHAR *szNetwork)
{
	PIP_ADAPTER_INFO pAdapterInfo = NULL;
	PIP_ADAPTER_INFO pAdapter = NULL;
	BOOL bRet = FALSE;

	szNetwork[0] = 0;
	ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);
	pAdapterInfo = new IP_ADAPTER_INFO;

	if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW)
	{
		delete pAdapterInfo;
		pAdapterInfo = (IP_ADAPTER_INFO *) malloc(ulOutBufLen);
		if (pAdapterInfo == NULL)
			return FALSE;
	}

	if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == NO_ERROR)
	{
		pAdapter = pAdapterInfo;
		while (pAdapter)
		{
			if (pAdapter->AddressLength == 6 && pAdapter->Type == MIB_IF_TYPE_ETHERNET)
			{
				//if (!strstr(pAdapter->Description, "TAP-Win") && !strstr(pAdapter->Description, "Virtual"))
				if(strstr(pAdapter->Description, "PCI"))
				{
					swprintf(szNetwork, L"%S %02X-%02X-%02X-%02X-%02X-%02X", pAdapter->Description
						, pAdapter->Address[0], pAdapter->Address[1], pAdapter->Address[2]
						, pAdapter->Address[3], pAdapter->Address[4], pAdapter->Address[5]);
					bRet = TRUE;								
				}
			}
			pAdapter = pAdapter->Next;
		}
	}
	if (pAdapterInfo)
		delete pAdapterInfo;

	return bRet;
}

BOOL SHA1PasswordHash(LPCTSTR lpszPassword, LPTSTR lpszHash)
{
	HCRYPTPROV hCryptProv; // Handle to our context
	HCRYPTHASH hCryptHash; // Handle to our hash
	BYTE       bHashValue[SHA1_HASH_LEN]; // This will hold our SHA-1 hash
	DWORD     dwSize = SHA1_HASH_LEN, dwCount; // Size of output, and a count variable for a for loop
	BOOL       bSuccess = FALSE; // We change this to TRUE if we complete the operations
	// Declare all the variables at the start of our code for C89 compatability

	if (CryptAcquireContext(&hCryptProv, NULL, 0, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT))
	{ // Initiate usage of the functions
		if (CryptCreateHash(hCryptProv, CALG_SHA1, 0, 0, &hCryptHash))
		{ // Create a SHA1 hash
			if (CryptHashData(hCryptHash, (PBYTE)lpszPassword, lstrlen(lpszPassword) * sizeof(TCHAR), 0))
			{ // Update the hash, (process our password)
				if (CryptGetHashParam(hCryptHash, HP_HASHVAL, bHashValue, &dwSize, 0))
				{ // Extract the hash
					for (dwCount = 0, *lpszHash = 0; dwCount < SHA1_HASH_LEN; dwCount++)
					{ // Format the hash into a big endian, hexadecimal string
						wsprintf(
							lpszHash + (dwCount * 2),
							TEXT("%02X"),
							bHashValue[dwCount]
							);
					}
					bSuccess = TRUE;
				}
			}
			CryptDestroyHash(hCryptHash);
		}
		CryptReleaseContext(hCryptProv, 0);
	}

	return bSuccess;
}

BOOL	GetDeviceKey(WCHAR *szDeviceKey)
{
	WCHAR	szDevieInfo[1024];
	szDeviceKey[0] = 0;

	if (!GetCPUInfo(szDevieInfo))
		return FALSE;
	GetNetworkInfo(szDevieInfo + wcslen(szDevieInfo));
	//if (!GetNetworkInfo(szDevieInfo + wcslen(szDevieInfo)))
	//	return FALSE;
	if (!SHA1PasswordHash(szDevieInfo, szDeviceKey))
		return FALSE;

	PrintDebugString(szDevieInfo);
	PrintDebugString(szDeviceKey);

	return TRUE;
}