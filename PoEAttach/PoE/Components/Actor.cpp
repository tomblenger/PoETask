#include "stdafx.h"
#include "Actor.h"

Actor::Actor()
{
	actionID = 1;
	isMoving = false;
	isAttacking = false;
}

void Actor::Update(uintptr_t addrPointer)
{
	Address = addrPointer;
	if (!Address)
		return;

	actionID = *(WORD *)(Address + 0xF8);
	isMoving = (actionID & ActionFlags::Moving) > 0;
	isAttacking = (actionID & ActionFlags::UsingAbility) > 0;

	actorSkills = ReadActorSkills(*(uintptr_t *)(Address + 0x3E8) + 0x08, *(uintptr_t *)(Address + 0x3F0));
}

std::vector<ActorSkill>	Actor::ReadActorSkills(uintptr_t startAddress, uintptr_t endAddress)
{
	std::vector<ActorSkill> result;
	if (!startAddress || !endAddress)
		return result;
 	if ((int)(endAddress - startAddress) / 16 > 50)
 	{
 		return result;
 	}
	for (uintptr_t addr = startAddress; addr < endAddress; addr += 0x10)
	{
		ActorSkill actorSkill;
		if(actorSkill.Update(*(uintptr_t *)addr))
			result.push_back(actorSkill);
	}
	return result;
}