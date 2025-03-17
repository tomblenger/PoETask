#include "stdafx.h"
#include "Sockets.h"
#include "Common.h"
#include "ItemResource.h"
#include "ItemData.h"

ItemSocketInfo::ItemSocketInfo()
{
	SocketIndex = 0;
	SocketColor = 0;
	IsSocketEquiped = 0;
	InitSocketItem();
}

ItemSocketInfo::~ItemSocketInfo()
{
	
}

void ItemSocketInfo::InitSocketItem()
{
	CurrentGemExp = 0;
	MaxGemExp = 0;
	SocketedName = L"";
	Address = 0;
	gemLevel = 0;
}

Sockets::Sockets()
{
	lstSocketColor.clear();
	lstSocketedGem.clear();
	lstItemSocketInfo.clear();
	lstSocketLinkInfo.clear();
}

void Sockets::Update(uintptr_t addrPointer)
{
	Address = addrPointer;

	if (!Address)
	{
		return;
	}

	lstSocketColor.clear();
	lstSocketedGem.clear();
	lstItemSocketInfo.clear();
	lstSocketLinkInfo.clear();

	lstSocketColor = List(Address);
	lstSocketedGem = SocketedGemsList(Address);
	lstItemSocketInfo = GetSocketInfo(lstSocketColor, lstSocketedGem);
	lstSocketLinkInfo = LinkList(Address);
}

std::vector<ItemSocketInfo> Sockets::GetSocketInfo(std::vector<int> socketcolorList, std::vector<SocketedGems> socketedGemsList)
{
	ItemSocketInfo				socketInfo;
	std::vector<ItemSocketInfo> resultList;

	if (socketcolorList.size() == 0)
	{
		return resultList;
	}

	for (int i = 0; i < socketcolorList.size(); i++)
	{
		socketInfo.IsSocketEquiped = 0;
		socketInfo.SocketColor = socketcolorList[i];
		socketInfo.SocketIndex = i;
		socketInfo.InitSocketItem();

		for (int j = 0; j < socketedGemsList.size(); j++)
		{
			if (socketedGemsList[j].SocketIndex == i)
			{
				if (socketedGemsList[j].SocketItemPtr)
				{
					ItemData	itemData;

					if (itemData.Update(socketedGemsList[j].SocketItemPtr))
					{
						socketInfo.IsSocketEquiped = 1;
						socketInfo.SocketColor = socketcolorList[i];
						socketInfo.SocketIndex = i;
						socketInfo.SocketedName = itemData.displayName;
						socketInfo.CurrentGemExp = itemData.skillGem.CurrentGemExp;
						socketInfo.MaxGemExp = itemData.skillGem.MaxGemExp;
						socketInfo.gemLevel = itemData.skillGem.GemLevel;
						socketInfo.Address = socketedGemsList[j].SocketItemPtr;
						break;
					}
				}
			}
		}

		resultList.push_back(socketInfo);
	}

	return resultList;
}

std::vector<int> Sockets::List(uintptr_t pointer)
{
	std::vector<int> resultList;

	if (!pointer)
	{
		return resultList;
	}

	uintptr_t	num = Address + 0x18;

	for (int i = 0; i < 6; i++)
	{
		int num2 = (int)*(DWORD*)(num);

		if (num2 >= 1 && num2 <= 6)
		{
			resultList.push_back((int)*(DWORD*)(num));
		}
		num += 4;
	}

	return resultList;
}

std::vector<SocketedGems> Sockets::SocketedGemsList(uintptr_t pointer)
{
	SocketedGems				socketedGemInfo;
	std::vector<SocketedGems>	resultList;


	if (!pointer)
	{
		return resultList;
	}

	uintptr_t	startAddress = Address + 0x30;

	for (int i = 0; i < 6; i++)
	{
		uintptr_t	objectAddress = *(uintptr_t*)(startAddress);

		if (objectAddress != 0)
		{
			socketedGemInfo.SocketIndex = i;
			socketedGemInfo.SocketItemPtr = objectAddress;
			resultList.push_back(socketedGemInfo);
		}

		startAddress += 8;
	}

	return resultList;
}

std::vector<std::vector<int>> Sockets::LinkList(uintptr_t pointer)
{
	std::vector<std::vector<int>> resultList;

	if (!pointer)
	{
		return resultList;
	}

	uintptr_t	linkStart = *(uintptr_t*)(pointer + 0x60);
	uintptr_t	linkEnd = *(uintptr_t*)(pointer + 0x68);
	uintptr_t	linkGroupingCount = linkEnd - linkStart;

	if (linkGroupingCount <= 0 || linkGroupingCount > 6)
	{
		return resultList;
	}

	int LinkCounter = 0;

	for (int i = 0; i < linkGroupingCount; i++)
	{
		std::vector<int> array;
		int LinkGroupSize = (int)*(BYTE*)(linkStart + i);

		for (int j = 0; j < LinkGroupSize; j++)
		{
			array.push_back(j + LinkCounter);
		}
		resultList.push_back(array);
		LinkCounter += LinkGroupSize;
	}

	return resultList;
}


SocketedGems::SocketedGems()
{
	SocketIndex = 0;
	SocketItemPtr = 0;
}
