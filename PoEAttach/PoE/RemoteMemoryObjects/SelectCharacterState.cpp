#include "stdafx.h"
#include "SelectCharacterState.h"
#include "GameStateController.h"
#include "Common.h"

CharacterInfo::CharacterInfo()
{

}

void CharacterInfo::Update(uintptr_t addrPointer)
{
	Address = addrPointer;

	if (!Address)
	{
		return;
	}

	Name = ReadNativeString(Address);
	League = ReadNativeString(Address + 0x20);
	Level = *(BYTE*)(Address + 0x40);
}

void SelectCharacterState::Update(uintptr_t addrPointer)
{
	Address = addrPointer;

	if (!Address || !GameStateController::IsSelectCharacterState())
	{
		return;
	}

	lstCharInfo.clear();

	lstCharInfo = GetCharacterInfo(Address);
}

std::vector<CharacterInfo>	SelectCharacterState::GetCharacterInfo(uintptr_t addrPointer)
{
	std::vector<CharacterInfo>	resultList;
	CharacterInfo				charInfo;
	if (!Address)
	{
		return resultList;
	}

	uintptr_t	startAddress = *(uintptr_t*)(Address + 0x80);
	uintptr_t	endAddress = *(uintptr_t*)(Address + 0x90);

	for (uintptr_t addr = startAddress; addr < endAddress; addr += 0x50)
	{
		charInfo.Update(addr);
		resultList.push_back(charInfo);
	}

	return resultList;
}