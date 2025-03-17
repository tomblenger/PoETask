#include "stdafx.h"
#include "NpcData.h"
#include "ObjectResource.h"
#include "Common.h"

bool NpcData::Update(uintptr_t address)
{
	WCHAR szName[MAX_NAME];

	if (!Entity::Update(address))
		return false;

	GetNpcResource(path, szName);
	name = szName;

	if (components.count("NPC") > 0)
	{
		npc.Update(components["NPC"]);
	}

	if (components.count("Positioned") > 0)
	{
		positionedComp.Update(components["Positioned"]);
	}

	if (components.count("Targetable") > 0)
	{
		targetable.Update(components["Targetable"]);
	}

	return true;
}
