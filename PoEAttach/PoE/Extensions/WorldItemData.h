#pragma once

#include "Entity.h"
#include "Targetable.h"
#include "Positioned.h"
#include "WorldItem.h"
#include "ItemData.h"

class WorldItemData : public Entity
{
public:
	bool		Update(uintptr_t address);

	Targetable	targetable;
	Positioned	positionedComp;
	WorldItem	worlditem;
	ItemData	item;
};