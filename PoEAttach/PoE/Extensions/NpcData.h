#pragma once

#include "Entity.h"
#include "Targetable.h"
#include "Positioned.h"
#include "Npc.h"

class NpcData : public Entity
{
public:
	bool			Update(uintptr_t address);

	Targetable		targetable;
	Positioned		positionedComp;
	Npc				npc;
	std::wstring	name;
};
