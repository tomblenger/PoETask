#pragma once

#include "RemoteMemoryObject.h"
#include "ServerInventory.h"

class InventoryHolder : public RemoteMemoryObject
{
public:
	InventoryHolder();
	void Update(uintptr_t addrPointer);

	static int StructSize;

	int id;
	ServerInventory inventory;
};
