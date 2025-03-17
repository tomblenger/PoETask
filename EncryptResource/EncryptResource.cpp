// EncryptResource.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include <fstream>
#include <Windows.h>

using namespace std;

char * EncryptBlock(char *buffer, int nLen, int &nOutLen);
char *	DecryptBlock(char *pbBlock, int nLen, int &nOutLen);

int main()
{
	char *buffer = nullptr, *encryptedBuffer = nullptr;
	int nLen, i = 0;
	streampos size;
	char aszAppPath[MAX_PATH], aszPath[MAX_PATH];

	GetModuleFileNameA(NULL, aszAppPath, MAX_PATH);
	i = (int)strlen(aszAppPath);
	while (aszAppPath[i] != '\\')
		i--;
	aszAppPath[i] = 0;
	sprintf(aszPath, "%s\\..\\Release\\PoEAttach.dll", aszAppPath);
	ifstream file(aszPath, ios::in | ios::binary | ios::ate);
	if (file.is_open())
	{
		size = file.tellg();
		std::cout << "file size = " << size << endl;

		buffer = new char[size];
		file.seekg(0, ios::beg);
		file.read(buffer, size);
		file.close();

		encryptedBuffer = EncryptBlock(buffer, (int)size, nLen);
		cout << "Encrypted Buffer - size = " << nLen << endl;
		//encryptedBuffer = DecryptBlock(encryptedBuffer, (int)nLen, nLen);
		sprintf(aszPath, "%s\\..\\..\\PoETask\\res\\PA", aszAppPath);
		ofstream ofile(aszPath, ios::out | ios::binary);
		if (ofile.is_open())
		{
			encryptedBuffer = EncryptBlock(buffer, (int)size, nLen);
			ofile.write(encryptedBuffer, nLen);
			ofile.close();
			cout << "Successfully finished." << endl;
		}
		else
		{
			cout << "Unable to open file to write!" << endl;
		}

		if (buffer)
			delete [] buffer;
		if (encryptedBuffer)
			delete [] encryptedBuffer;
	}
	else
	{
		cout << "Unable to open file to read !" << endl;
	}

	return 0;
}

char * EncryptBlock(char *buffer, int nLen, int &nOutLen)
{
	char *outputbuffer, *temp;
	int i = 0;

	nOutLen = nLen + 4 + 40;
	outputbuffer = new char[nOutLen];
	temp = new char[nLen + 4];
	*(int*)temp = nLen;
	memcpy(temp + 4, buffer, nLen);
	for (i = 0; i < 40; i++)
	{
		outputbuffer[i] = rand() % 0x80;
	}
	outputbuffer[40] = temp[0];
	for (i = 41; i < nLen + 4 + 40; i++)
	{
		outputbuffer[i] = temp[i - 40] ^ outputbuffer[i - 1];
	}

	delete[] temp;

	return outputbuffer;
}

char *	DecryptBlock(char *pbBlock, int nLen, int &nOutLen)
{
	char *pbBuffer;
	int	 i;

	pbBuffer = new char[nLen];
	pbBuffer[0] = pbBlock[40];
	for (i = 1; i < nLen; i++)
	{
		pbBuffer[i] = pbBlock[40 + i] ^ pbBlock[40 + i - 1];
	}
	nOutLen = *(int *)pbBuffer;
	memmove(pbBuffer, pbBuffer + 4, nOutLen);
	return pbBuffer;
}