#pragma once

#include <vector>
#include "Entity.h"
#include "Life.h"
#include "Player.h"
#include "Actor.h"
#include "Targetable.h"
#include "Positioned.h"
#include "Stats.h"

class PlayerData : public Entity
{
public:
	bool	Update(uintptr_t address);

	Life	life;	
	Player	player;
	Actor	actor;
	Targetable	targetable;
	Positioned	positionedComp;
	std::vector<Stats> stats;
};