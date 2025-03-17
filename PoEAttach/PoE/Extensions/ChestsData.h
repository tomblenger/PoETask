#pragma once

#include <vector>
#include "Entity.h"
#include "Targetable.h"
#include "Positioned.h"
#include "Stats.h"
#include "Chest.h"

class ChestsData : public Entity
{
public:
	bool		Update(uintptr_t address);

	Chest		chest;
	Targetable	targetable;
	Positioned	positionedComp;
	std::vector<Stats> stats;
	std::wstring name;
};