#include "stdafx.h"
#include "SkillGem.h"

SkillGem::SkillGem()
{
	GemLevel = 0;
}

void SkillGem::Update(uintptr_t addrPointer)
{
	Address = addrPointer;

	if (!Address)
	{
		return;
	}

	GemLevel = (int)*(BYTE*)(Address + 0x2C);
	GemExpPerlevel = (int)*(DWORD*)(Address + 0x30);
	CurrentGemExp = (int)*(DWORD*)(Address + 0x28);
	MaxGemExp = (int)*(DWORD*)(Address + 0x34);
}
