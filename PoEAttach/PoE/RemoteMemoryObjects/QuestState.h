#pragma once

#include "RemoteMemoryObject.h"

class QuestState : public RemoteMemoryObject
{
public:
	QuestState();
	bool Update(uintptr_t addr);

	uintptr_t questPtr;
	int	questStateId;
	int testOffset;
	std::wstring questStateText;
	std::wstring questProgressText;
};