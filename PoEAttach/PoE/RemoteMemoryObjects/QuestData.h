#pragma once

#include <algorithm>
#include <vector>
#include "RemoteMemoryObject.h"
#include "Quest.h"

class QuestData : public RemoteMemoryObject
{
public:
	void	Update(uintptr_t addrPointer);

	std::vector<Quest>	quests;
};
