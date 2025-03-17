#include "stdafx.h"
#include "MonsterData.h"
#include "ObjectResource.h"

bool MonsterData::Update(uintptr_t address)
{
	WCHAR szName[MAX_NAME];

	if (!Entity::Update(address))
		return false;

	GetMonsterResource(path, szName);
	name = szName;

	if (components.count("Actor") > 0)
	{
		actor.Update(components["Actor"]);
	}

	if (components.count("Life") > 0)
	{
		life.Update(components["Life"]);
	}

	if (components.count("Positioned") > 0)
	{
		positionedComp.Update(components["Positioned"]);
	}

	stats.clear();
	if (components.count("Stats") > 0)
	{
		uintptr_t statPtrStart = *(uintptr_t *)(components["Stats"] + 0x98);
		uintptr_t statPtrEnd = *(uintptr_t *)(components["Stats"] + 0xA0);
		for (uintptr_t addr = statPtrStart; addr < statPtrEnd; addr += 0x08)
		{
			Stats stat;
			stat.Update(addr);
			stats.push_back(stat);
		}
	}

	if (components.count("Targetable") > 0)
	{
		targetable.Update(components["Targetable"]);
	}

	return true;
}