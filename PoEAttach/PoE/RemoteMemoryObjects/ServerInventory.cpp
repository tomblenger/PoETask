#include "stdafx.h"
#include <stack>
#include <map>
#include "ServerInventory.h"
#include "Common.h"


ItemLocation::ItemLocation()
{
	xMin = 0;
	yMin = 0;
	xMax = 0;
	yMax = 0;
}

void ItemLocation::Update(uintptr_t addrPointer)
{
	Address = addrPointer;
	if (!Address)
		return;

	xMin = *(int *)Address;
	yMin = *(int *)(Address + 4);
	xMax = *(int *)(Address + 8);
	yMax = *(int *)(Address + 8);
}

void InvenSlotItem::Update(uintptr_t addrPointer)
{
	Address = addrPointer;
	if (!Address)
		return;

	item.Update(*(uintptr_t *)Address);
	location.Update(Address + 8);
}

ServerInventory::ServerInventory()
{
	invenType = 0;
	invenSlot = 0;
	columns = 0;
	rows = 0;
	totalItemCounts = 0;
	serverRequestCounter = 0;
}

void ServerInventory::Update(uintptr_t addrPointer)
{
	Address = addrPointer;
	if (!Address)
		return;

	invenType = *(int *)Address;
	invenSlot = *(int *)(Address + 0x04);
	columns = *(int *)(Address + 0x0C);
	rows = *(int *)(Address + 0x10);
	slotItems = ReadHashMap(Address + 0x48);
	totalItemCounts = *(int *)(Address + 0x50);
	serverRequestCounter = *(int *)(Address + 0xA8);
}

std::vector<InvenSlotItem> ServerInventory::ReadHashMap(uintptr_t pointer)
{
	std::map<int, InvenSlotItem> result;
	std::stack<InvenSlotItemHashNode *> stack;
	std::vector<InvenSlotItem> resultList;

	if (!pointer)
		return resultList;

	auto startNode = RemoteMemoryObject::ReadObject<InvenSlotItemHashNode>(pointer);
	auto item = startNode->Root();
	stack.push(item);

	int stackCounter = 500;
	while (stack.size() != 0 && stackCounter-- > 0)
	{
		auto node = stack.top();
		stack.pop();

		if (!node->IsNull())
			result[node->key] = node->value1;

		auto prev = node->Previous();
		if (!prev->IsNull())
			stack.push(prev);

		auto next = node->Next();
		if (!next->IsNull())
			stack.push(next);

		delete node;
	}

	delete startNode;

	for each (auto item in result)
	{
		resultList.push_back(item.second);
	}

	return resultList;
}
