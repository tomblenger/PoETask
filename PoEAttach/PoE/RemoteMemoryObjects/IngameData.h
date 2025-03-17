#pragma once

#include "RemoteMemoryObject.h"
#include "WorldArea.h"

class IngameData : public RemoteMemoryObject
{
public:
	IngameData();
	void Update(uintptr_t addrPointer);

	uintptr_t currentAreaPtr;
	WorldArea	currentWorldArea;
	BYTE currentAreaLevel;
	UINT currentAreaHash;
	uintptr_t localPlayerPtr;
	uintptr_t entityListPtr;
	uintptr_t labyrinthDataPtr;
};