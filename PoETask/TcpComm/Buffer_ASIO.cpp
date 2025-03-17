#include "stdafx.h"
#include "Buffer.h"
#include "Encrypt.hpp"

Syncable::~Syncable()
{
}

Buffer::Buffer()
{
	bufferexternal = NULL;
	used = 0;
	readoffset = REAL_BUFFER_OFFSET;
}


Buffer::~Buffer()
{
	used = 0;
}

void Buffer::Encrypt()
{
	//char szKey[100];
	//int nKeyLen = GetRandomKey(szKey);
	//int nEncBufLen = mEncryptBuffer((char *)bufferinternal + REAL_BUFFER_OFFSET, szKey, *(DWORD *)(bufferinternal + 1));
	//*(DWORD *)(bufferinternal + 1) = nEncBufLen;
	//bufferinternal[REAL_BUFFER_OFFSET - 1] = (BYTE)nKeyLen;
	//used = REAL_BUFFER_OFFSET + nEncBufLen;

	DWORD i;
	srand((DWORD)time(0));
	bufferinternal[REAL_BUFFER_OFFSET - 1] = (BYTE)rand() % 0x100;
	for (i = REAL_BUFFER_OFFSET; i < used; i++)
	{
		bufferinternal[i] = (BYTE)(255 - bufferinternal[i] + bufferinternal[REAL_BUFFER_OFFSET - 1]);
	}
}

void Buffer::Decrypt()
{
	//int nKeyLen = (int)bufferinternal[REAL_BUFFER_OFFSET - 1];
	//int nDecBufLen = mDecryptBuffer((char *)bufferinternal + REAL_BUFFER_OFFSET, nKeyLen, *(DWORD *)(bufferinternal + 1));
	//*(DWORD *)(bufferinternal + 1) = nDecBufLen;
	//used = REAL_BUFFER_OFFSET + nDecBufLen;

	DWORD i;
	for (i = REAL_BUFFER_OFFSET; i < used; i++)
	{
		bufferinternal[i] = (BYTE)(255 - bufferinternal[i] + bufferinternal[REAL_BUFFER_OFFSET - 1]);
	}
}

void Buffer::AddData(const BYTE *const pData, DWORD dwSize, BOOL bDontChangeLength)
{
	memcpy(bufferinternal + used, pData, dwSize);
	used += dwSize;
	if (!bDontChangeLength)
		*(DWORD *)(bufferinternal + 1) += dwSize;
}

void Buffer::AddData(CStringA &str, DWORD dwSize)
{
	if (!dwSize)
	{
		AddData((DWORD)str.GetLength());
	}
	dwSize = str.GetLength();
	memcpy(bufferinternal + used, (const char *)str, dwSize);
	used += dwSize;
	*(DWORD *)(bufferinternal + 1) += dwSize;
}

void Buffer::AddData(CStringW &str, DWORD dwSize)
{
	if (!dwSize)
	{
		AddData((DWORD)str.GetLength());
	}
	dwSize = str.GetLength();
	memcpy(bufferinternal + used, (const WCHAR *)str, dwSize * 2);
	used += dwSize * 2;
	*(DWORD *)(bufferinternal + 1) += dwSize * 2;
}

void Buffer::AddData(Syncable *pSyncable)
{
	pSyncable->Write(this);
}

void Buffer::EmptyUsed()
{
	readoffset = REAL_BUFFER_OFFSET;
	used = 0;
}

void Buffer::CreatePackage(BYTE bOpcode)
{
	EmptyUsed();

	used = REAL_BUFFER_OFFSET;
	bufferinternal[0] = bOpcode;
	*(DWORD *)(bufferinternal + 1) = 0;
}

void Buffer::Copy(Buffer *buffer)
{
	used = buffer->used;
	readoffset = buffer->readoffset;
	memcpy(bufferinternal, buffer->bufferinternal, used);
}

void Buffer::ReadStart()
{
	readoffset = REAL_BUFFER_OFFSET;
}

void Buffer::Read(BYTE *pData, DWORD dwSize)
{
	if (readoffset + dwSize > used)
		return;
	memcpy(pData, bufferinternal + readoffset, dwSize);
	readoffset += dwSize;
}

void Buffer::ReadMultiString(CStringA &str, DWORD dwSize)
{
	str = "";
	if (!dwSize)
	{
		dwSize = Read<DWORD>();
	}
	if (readoffset + dwSize > used)
		return;
	str.Append((char *)(bufferinternal + readoffset), dwSize);
	readoffset += dwSize;
}

void Buffer::ReadWideString(CStringW &str, DWORD dwSize)
{
	str = L"";
	if (!dwSize)
	{
		dwSize = Read<DWORD>();
		dwSize = min(MAX_PATH, dwSize);
	}
	if (readoffset + dwSize * 2 > used)
		return;
	str.Append((WCHAR *)(bufferinternal + readoffset), dwSize);
	readoffset += dwSize * 2;
}

void Buffer::ReadWideString(std::wstring &str, DWORD dwSize)
{
	str = L"";
	if (!dwSize)
	{
		dwSize = Read<DWORD>();
		dwSize = min(MAX_PATH, dwSize);
	}
	if (readoffset + dwSize * 2 > used)
		return;
	str.append((WCHAR *)(bufferinternal + readoffset), dwSize);
	readoffset += dwSize * 2;
}

void	Buffer::Read(Buffer *buffer)
{
	DWORD	dwSize;

	if (buffer->readoffset + REAL_BUFFER_OFFSET > buffer->used)
		return;
	dwSize = *(DWORD *)(buffer->bufferinternal + buffer->readoffset + 1);
	EmptyUsed();
	if (buffer->readoffset + dwSize + REAL_BUFFER_OFFSET > buffer->used)
		return;
	CreatePackage(*(BYTE *)(buffer->bufferinternal + buffer->readoffset));
	AddData(buffer->bufferinternal + buffer->readoffset + REAL_BUFFER_OFFSET, dwSize);
	buffer->readoffset += dwSize + REAL_BUFFER_OFFSET;
}

void	Buffer::Write(Buffer *buffer)
{
	buffer->AddData(bufferinternal, used);
}

#ifndef _NO_SQL_

SQLResult *SQLFirst(const char *queryformat, ...)
{
	SQLResult	*sql;
	char		query[1000];
	va_list		argList;
	int			query_state;

	va_start(argList, queryformat);
	vsprintf_s(query, 1000, queryformat, argList);
	va_end(argList);

	sql = Allocate<SQLResult>();
	if (!sql->error)
	{
		query_state = mysql_query(&sql->mysql, query);
		if (!query_state)
		{
			sql->res = mysql_store_result(&sql->mysql);
		}
		else
		{
			sql->error = -2;
		}

	}
	return sql;
}

BOOL	SQLNext(SQLResult *sql)
{
	sql->row = mysql_fetch_row(sql->res);
	if (!sql->row)
		return FALSE;
	return TRUE;
}

void	SQLClose(SQLResult *sql)
{
	Release<SQLResult>(sql);
}
#endif
