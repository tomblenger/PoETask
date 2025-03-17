#include "stdafx.h"
#include "WorldItemData.h"

bool WorldItemData::Update(uintptr_t address)
{
	if (!Entity::Update(address))
		return false;

	if (components.count("Positioned") > 0)
	{
		positionedComp.Update(components["Positioned"]);
	}

	if (components.count("Targetable") > 0)
	{
		targetable.Update(components["Targetable"]);
	}

	if (components.count("WorldItem") > 0)
	{
		worlditem.Update(components["WorldItem"]);
		item.Update(worlditem.itemEntity.Address);
	}

	return true;
}