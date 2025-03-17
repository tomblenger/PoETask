#pragma once

#include "RemoteMemoryObject.h"
#include "StashElement.h"
#include "QuestData.h"
#include "SkillElement.h"

class IngameUIElements : public RemoteMemoryObject
{
public:
	void Update(uintptr_t addrPointer);
	void getAllChild(uintptr_t addr, int depth);

 	Element			skillBar;
	StashElement	stashElement;
	QuestData		questData;
	Element			interaction;
	Element			purchaseItem;
	Element			sellItem;
	Element			inventory;
	Element			reward;
	Element			passiveSkillPanel;
	Element			passiveSkillUp;
	Element			worldmap;
	Element			skipTutorial;
	Element			deadPanel;
	Element			areaInstancePanel;
	Element			specialProcMonsterPanel;

	std::vector<SkillElement>	skillBarSkills;
	std::vector<int>	teleportFieldIds;
};