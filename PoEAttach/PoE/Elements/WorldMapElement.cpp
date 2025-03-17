#include "stdafx.h"
#include "WorldMapElement.h"

void WorldMapElement::Update(uintptr_t addrPointer)
{
	if (!Element::Update(addrPointer))
		return;
}