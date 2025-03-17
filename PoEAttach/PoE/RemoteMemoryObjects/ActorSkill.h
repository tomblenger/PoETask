#pragma once
#include "RemoteMemoryObject.h"

class ActorSkill : public RemoteMemoryObject
{
public:
	ActorSkill();
	bool	Update(uintptr_t addrPointer);

	std::wstring	name;
	USHORT			id;
	bool			canBeUsedWithWeapon;
	bool			canBeUsed;
	int				cost;
	int				totalUses;
	int				coolDown;
	int				soulsPerUse;
	int				totalVaalUses;
	BYTE			skillUseStage;
	bool			isUsing;
	BYTE			skillStage;
	bool			isUsingPressed;
	int				slotIdentifier;
	int				socketIndex;
	bool			isUserSkill;
	bool			allowedToCast;
	bool			isVaalSkill;
};