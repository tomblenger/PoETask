#include "stdafx.h"
#include "OtherData.h"
#include "Common.h"

bool OtherData::Update(uintptr_t address)
{
	if (!Entity::Update(address))
		return false;

	if (components.count("Positioned") > 0)
	{
		positionedComp.Update(components["Positioned"]);
	}

	if (components.count("Render") > 0)
	{
		render.Update(components["Render"]);
	}

	if (components.count("Transitionable") > 0)
	{
		transitionable.Update(components["Transitionable"]);
	}

// 	if (components.count("Targetable") > 0)
// 	{
// 		targetable.Update(components["Targetable"]);
// 	}

	return true;
}