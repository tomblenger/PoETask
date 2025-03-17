#pragma once

#include <vector>
#include "Entity.h"
#include "Actor.h"
#include "Life.h"
#include "Targetable.h"
#include "Positioned.h"
#include "Stats.h"

class MonsterData : public Entity
{
public:
	bool		Update(uintptr_t address);

	Actor	actor;
	Life	life;
	Positioned	positionedComp;
	std::vector<Stats> stats;
	Targetable	targetable;
	std::wstring	name;
};