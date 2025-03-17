#include "stdafx.h"
#include "QuestData.h"

bool CompareQuestResource(Quest a, Quest b)
{
	if (a.key < b.key)
		return true;
	return false;
}

void QuestData::Update(uintptr_t addrPointer)
{
	Address = addrPointer;
	if (!Address)
		return;

	quests.clear();
	uintptr_t addr = *(uintptr_t *)Address;

	while (addr)
	{
		Quest	quest;
		if (quest.Update(addr))
		{
			quests.push_back(quest);
		}
		if(addr == *(uintptr_t *)(Address + 0x08))
			break;
		addr = *(uintptr_t *)addr;
	}

	std::sort(quests.begin(), quests.end(), CompareQuestResource);
}