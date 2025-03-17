#include "stdafx.h"
#include "Map.h"

void Map::Update(uintptr_t addrPointer)
{
	if (!Element::Update(addrPointer))
		return;
}