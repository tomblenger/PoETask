#pragma once

#include <vector>
#include <map>
#include "QuestState.h"
#include "FileInMemory.h"

class QuestStates : public FileInMemory
{
public:
	QuestStates();
	QuestState GetQuestState(std::wstring questId, int stateId);

	std::vector<QuestState> entriesList;
	std::map<std::wstring, std::map<int, QuestState>> questStatesDictionary;
};