#include "stdafx.h"
#include "ActorSkill.h"

ActorSkill::ActorSkill()
{
	id = 0;
	canBeUsedWithWeapon = false;
	canBeUsed = false;
	cost = 0;
	totalUses = 0;
	coolDown = 0;
	soulsPerUse = 0;
	totalVaalUses = 0;
	skillUseStage = 2;
	isUsing = false;
	skillStage = 0;
	isUsingPressed = false;
	slotIdentifier = 0;
	socketIndex = 0;
	isUserSkill = false;
	allowedToCast = false;
}

bool ActorSkill::Update(uintptr_t addrPointer)
{
	Address = addrPointer;
	if (!Address)
		return false;

	id = *(USHORT *)(Address + 0x10);
	if (*(uintptr_t *)(Address + 0x20))
	{
		name = ReadStringU(**(uintptr_t **)(*(uintptr_t *)(Address + 0x20) + 0x08));
	}
	else
	{
		switch (id)
		{
		case 0x266:
			name = L"Interaction";
			break;
		case 0x2909:
			name = L"Move";
			break;
		case 0x37D9:
			name = L"WashedUp";
			break;
		default:
			break;
		}
	}
	canBeUsedWithWeapon = *(BYTE *)(Address + 0x46) > 0;
	canBeUsed = *(BYTE *)(Address + 0x47) == 0;
	cost = *(BYTE *)(Address + 0x4C);
	totalUses = *(int *)(Address + 0x50);
	coolDown = *(int *)(Address + 0x58);	//milliseconds
	soulsPerUse = *(int *)(Address + 0x68);
	totalVaalUses = *(int *)(Address + 0x6C);
	skillUseStage = *(BYTE *)(Address + 0x08);	//Default value is 2, but increasing while skill casting (for repeating skills)
	isUsing = skillUseStage > 2;
	skillStage = *(BYTE *)(Address + 0x0C);
	isUsingPressed = (skillStage & 8) > 0;
	slotIdentifier = (id >> 8) & 0xff;
	socketIndex = (slotIdentifier >> 2) & 15;
	isUserSkill = (slotIdentifier & 0x80) > 0;
	allowedToCast = canBeUsedWithWeapon && canBeUsed;
	isVaalSkill = (soulsPerUse >= 1) && (totalVaalUses >= 1);

	return true;
}