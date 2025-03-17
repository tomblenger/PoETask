#pragma once

#include "Component.h"
#include <vector>

class ItemSocketInfo
{
public:
	ItemSocketInfo();
	~ItemSocketInfo();
	void InitSocketItem();
	std::wstring	SocketedName;
	int				SocketIndex;
	int				SocketColor;
	int				CurrentGemExp;
	int				MaxGemExp;
	int				gemLevel;
	BYTE			IsSocketEquiped;
	uintptr_t		Address;
};

class SocketedGems
{
public:
	SocketedGems();
	int				SocketIndex;
	uintptr_t		SocketItemPtr;
};

class Sockets : Component
{
public:
	Sockets();
	void							Update(uintptr_t addrPointer);
	std::vector<int>				List(uintptr_t pointer);
	std::vector<SocketedGems>		SocketedGemsList(uintptr_t pointer);
	std::vector<std::vector<int>>	LinkList(uintptr_t pointer);
	std::vector<ItemSocketInfo>		GetSocketInfo(std::vector<int> socketcolorList, std::vector<SocketedGems> socketedGemsList);

	std::vector<int>				lstSocketColor;
	std::vector<SocketedGems>		lstSocketedGem;
	std::vector<ItemSocketInfo>		lstItemSocketInfo;
	std::vector<std::vector<int>>	lstSocketLinkInfo;
};

// red	 R	1
// green G	2
// blue	 B	3
// white W  ?