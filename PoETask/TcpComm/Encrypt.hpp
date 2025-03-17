#ifndef __ENCRYPT_H__
#define __ENCRYPT_H__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#ifdef _PROTECT
#include "ThemidaSDK/Include/C/ThemidaSDK.h"
#endif

int GetRandomKey(char *szKey)
{
#ifdef _PROTECT
	VM_DOLPHIN_RED_START
#endif

	srand((int)time(0));
	int nKeyLen = rand() % 4 + 4;
	for (int i = 0; i < nKeyLen; i++)
	{
		szKey[i] = rand() % 26 + 65;
	}
	szKey[nKeyLen] = 0;

#ifdef _PROTECT
	VM_DOLPHIN_RED_END
#endif

	return nKeyLen;
}

int mEncryptBuffer(char *pBuffer, char *szKey, int bufferLen)
{
	int		nTotalLen = 0, nLen = 0;

#ifdef _PROTECT
	VM_DOLPHIN_RED_START
#endif

	int nLength = (int)strlen(szKey);
	nTotalLen = bufferLen;
	nLen = nTotalLen % nLength;

	char	szTemp[] = { '+', '-', '[', ']', '^', '%', '&', '#', '$', '!', '/', '*', '(', ')', '{', '}' };

	for (int i = 0; i < nLength - nLen + nLength; i++)
	{
		if (i == 0)
		{
			pBuffer[nTotalLen + i] = '@';
		}
		else
		{
			pBuffer[nTotalLen + i] = szTemp[rand() % 16];
		}
	}

	nTotalLen = bufferLen + nLength * 2 - nLen;

	if (nTotalLen % 2 == 1)
	{
		pBuffer[nTotalLen] = szTemp[rand() % 10];
		nTotalLen++;
	}

	nLen = nTotalLen / 2;

	char	temp[2];

	for (int i = 0; i < nLen; i++)
	{
		for (int j = 0; j < nLen; j++)
		{
			temp[0] = pBuffer[j];
			pBuffer[j] = pBuffer[nTotalLen - 1 - j];
			pBuffer[nTotalLen - 1 - j] = temp[0];
		}

		temp[0] = pBuffer[nLen];
		pBuffer[nLen] = pBuffer[i];
		pBuffer[i] = temp[0];

		temp[0] = pBuffer[nLen - 1];
		pBuffer[nLen - 1] = pBuffer[nTotalLen - 1 - i];
		pBuffer[nTotalLen - 1 - i] = temp[0];
	}

	memcpy(pBuffer + nTotalLen, szKey, nLength);

	nTotalLen = nTotalLen + nLength;
	nLen = nTotalLen % nLength;

	if (nLen > 0)
	{
		for (int i = 0; i < nLength - nLen; i++)
		{
			pBuffer[nTotalLen + i] = szTemp[rand() % 10];
		}
	}
	else
	{
		nLen = nLength;
	}

	nTotalLen = nTotalLen + nLength - nLen;
	pBuffer[nTotalLen] = 0;

	nLen = nTotalLen / nLength;
	for (int i = 0; i < nLength; i++)
	{
		for (int j = 0; j < nLen; j++)
		{
			if (j == 0)
				pBuffer[i + j * nLength] = pBuffer[i + j * nLength] ^ szKey[i];
			else
				pBuffer[i + j * nLength] = pBuffer[i + j * nLength] ^ pBuffer[i + j * nLength - nLength];
		}
	}

	pBuffer[nTotalLen] = 0;

#ifdef _PROTECT
	VM_DOLPHIN_RED_END
#endif

	return nTotalLen;
}

int mDecryptBuffer(char *pBuffer, int nKeyLen, int nDataLen)
{
	char	temp[2];
	char	szKey[MAX_PATH];

#ifdef _PROTECT
	VM_DOLPHIN_RED_START
#endif

	int nBufferLen = nDataLen;
	int nLen = nBufferLen / nKeyLen;

	for (int i = 0; i < nKeyLen; i++)
	{
		for (int j = nLen - 1; j > 0; j--)
		{
			pBuffer[i + j * nKeyLen] = pBuffer[i + j * nKeyLen] ^ pBuffer[i + j * nKeyLen - nKeyLen];
		}
	}

	int i = nDataLen;
	while (!isalpha(pBuffer[i]) && !isdigit(pBuffer[i]))
		i--;
	i++;
	pBuffer[i] = 0;

	nDataLen = i;

	memcpy((void *)szKey, (void *)(pBuffer + nDataLen - nKeyLen), nKeyLen);
	szKey[nKeyLen] = 0;

	for (int i = 0; i < nKeyLen; i++)
	{
		pBuffer[i] = pBuffer[i] ^ szKey[i];
	}
	pBuffer[nDataLen - nKeyLen] = 0;

	nBufferLen = nDataLen - nKeyLen;

	nLen = nBufferLen / 2;

	for (int idx = nLen - 1; idx >= 0; idx--)
	{
		temp[0] = pBuffer[nLen - 1];
		pBuffer[nLen - 1] = pBuffer[nBufferLen - 1 - idx];
		pBuffer[nBufferLen - 1 - idx] = temp[0];

		temp[0] = pBuffer[nLen];
		pBuffer[nLen] = pBuffer[idx];
		pBuffer[idx] = temp[0];

		for (int j = 0; j < nLen; j++)
		{
			temp[0] = pBuffer[j];
			pBuffer[j] = pBuffer[nBufferLen - 1 - j];
			pBuffer[nBufferLen - 1 - j] = temp[0];
		}
	}

	i = nBufferLen;
	while (pBuffer[i] != 0x40 && pBuffer[i] < 0x80 && !isalpha(pBuffer[i]) && !isdigit(pBuffer[i]))
		i--;
	pBuffer[i] = 0;
	nBufferLen = i;

	pBuffer[nBufferLen] = 0;

#ifdef _PROTECT
	VM_DOLPHIN_RED_END
#endif

	return nBufferLen;
}

#endif	//__ENCRYPT_H__