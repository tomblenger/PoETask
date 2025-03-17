#pragma once

#include "RemoteMemoryObject.h"
#include <vector>

class CharacterInfo : public RemoteMemoryObject
{
public:
	CharacterInfo();

	void	Update(uintptr_t addrPointer);

	std::wstring		Name;
	std::wstring		League;
	int					Level;
};

class SelectCharacterState : public RemoteMemoryObject
{
public:
	void	Update(uintptr_t addrPointer);
	std::vector<CharacterInfo>		GetCharacterInfo(uintptr_t addrPointer);

	std::vector<CharacterInfo>		lstCharInfo;
};