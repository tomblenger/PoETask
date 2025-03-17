#pragma once
#include "RemoteMemoryObject.h"
#include "QuestResource.h"
#include "QuestState.h"

class Quest : public RemoteMemoryObject
{
public:
	Quest();
	bool	Update(uintptr_t addrPointer);

	std::wstring	id;
	int				act;
	std::wstring	name;
	int				questState;
	int				questID;
	int				step;
	bool			isCompleted;

	int				key;
	QuestState		state;
};