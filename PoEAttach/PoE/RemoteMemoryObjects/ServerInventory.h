#pragma once

#include <vector>
#include "RemoteMemoryObject.h"
#include "ItemData.h"

class ItemLocation : public RemoteMemoryObject
{
public:
	ItemLocation();
	void Update(uintptr_t addrPointer);

	int xMin;
	int yMin;
	int xMax;
	int yMax;
};

class InvenSlotItem : public RemoteMemoryObject
{
public:
	InvenSlotItem() {}
	void Update(uintptr_t addrPointer);

	ItemData		item;
	ItemLocation	location;
	DWORD			itemKey;
};

class InvenSlotItemHashNode : public RemoteMemoryObject
{
public:
	static InvenSlotItemHashNode * ReadObject(uintptr_t address)
	{
		auto result = RemoteMemoryObject::ReadObject<InvenSlotItemHashNode>(address);
		if (!result->IsNull())
			result->Update();
		return result;
	}

	inline InvenSlotItemHashNode * Previous() { return ReadObject(Address); }
	inline InvenSlotItemHashNode * Root() { return ReadObject(Address + 0x8); }
	inline InvenSlotItemHashNode * Next() { return ReadObject(Address + 0x10); }
	inline bool IsNull() { return Address != 0 && *(BYTE *)(Address + 0x19) != 0; }

	void Update()
	{
		key = *(int *)(Address + 0x20);
		value1.Update(*(uintptr_t *)(Address + 0x28));
		value1.itemKey = key;
	}

	int key;
	InvenSlotItem value1;
};

class ServerInventory : public RemoteMemoryObject
{
public:
	ServerInventory();
	void Update(uintptr_t addrPointer);
	std::vector<InvenSlotItem> ReadHashMap(uintptr_t pointer);

	int invenType;
	int invenSlot;
	int columns;
	int rows;
	std::vector<InvenSlotItem> slotItems;
	int totalItemCounts;
	int serverRequestCounter;
};