#include "stdafx.h"
#include <algorithm>
#include "ServerData.h"
#include "InventoryHolder.h"
#include "SkillResource.h"
#include "Patch.h"


bool ComparePassiveSkill(const PassiveSkill &a, const PassiveSkill &b)
{
	if (a.key < b.key)
		return true;
	return false;
}

ServerData::ServerData()
{
	playerClass = 0;
	characterLevel = 0;
	passiveRefundPointsLeft = 0;
	questPassiveSkillPoints = 0;
	freePassiveSkillPointsLeft = 0;
	totalAscendencyPoints = 0;
	spentAscendencyPoints = 0;
	timeInGame = 0;
	networkState = 0;
	partyAllocationType = 0;
	latency = 0;
	partyStatus = 0;
	tradeChatChannel = 0;
	globalChatChannel = 0;
	lastActionId = 0;
	monserLevel = 0;
	monsterRemaining = 0;
}

void ServerData::Update(uintptr_t addrPointer)
{
	Address = addrPointer;
	if (!Address)
		return;

	playerClass = *(BYTE *)(Address + DATA_OFFSET_SERVERDATA_CHARACTER_LEVEL - 4) & 0xF;
	characterLevel = *(int *)(Address + DATA_OFFSET_SERVERDATA_CHARACTER_LEVEL);
	passiveRefundPointsLeft = *(int *)(Address + DATA_OFFSET_SERVERDATA_CHARACTER_LEVEL + 4);
	questPassiveSkillPoints = *(int *)(Address + DATA_OFFSET_SERVERDATA_CHARACTER_LEVEL + 8);
	totalAscendencyPoints = *(int *)(Address + DATA_OFFSET_SERVERDATA_CHARACTER_LEVEL + 0x10);
	spentAscendencyPoints = *(int *)(Address + DATA_OFFSET_SERVERDATA_CHARACTER_LEVEL + 0x14);
	passiveSkillIds = ReadPassiveSkillIdArray(*(uintptr_t *)(Address + DATA_OFFSET_SERVERDATA_PASSIVESKILL), *(uintptr_t *)(Address + DATA_OFFSET_SERVERDATA_PASSIVESKILL + 8));
	passiveSkills.clear();
	for (int i = 0; i < passiveSkillIds.size(); i++)
	{
		PassiveSkill skill;
		skill.graphId = passiveSkillIds[i];
		auto skillInfo = GetPassiveSkillNameByID(skill.graphId);
		if (skillInfo)
		{
			skill.key = skillInfo->dwKey;
			skill.id = skillInfo->szId;
			skill.name = skillInfo->szName;
		}
		passiveSkills.push_back(skill);
	}
	std::sort(passiveSkills.begin(), passiveSkills.end(), ComparePassiveSkill);

	freePassiveSkillPointsLeft = (characterLevel - 1) + questPassiveSkillPoints - ((int)passiveSkillIds.size() - spentAscendencyPoints);
	timeInGame = *(int *)(Address + DATA_OFFSET_SERVERDATA_LATENCY - 8);
	networkState = *(BYTE *)(Address + DATA_OFFSET_SERVERDATA_NETWORKSTATE);
	league = ReadNativeString(Address + DATA_OFFSET_SERVERDATA_NETWORKSTATE + 0x28);
	latency = *(int *)(Address + DATA_OFFSET_SERVERDATA_LATENCY);
	partyAllocationType = *(BYTE *)(Address + DATA_OFFSET_SERVERDATA_PARTY_ALLOCATE_TYPE);
	partyStatus = *(BYTE *)(Address + DATA_OFFSET_SERVERDATA_PARTY_ALLOCATE_TYPE - 0x20);
	guildName = ReadNativeString(*(uintptr_t *)(Address + DATA_OFFSET_SERVERDATA_GUILD));
	playerInventories = ReadStructsArray(*(uintptr_t *)(Address + DATA_OFFSET_SERVERDATA_PLAYERINVENTORY), *(uintptr_t *)(Address + DATA_OFFSET_SERVERDATA_PLAYERINVENTORY + 8), InventoryHolder::StructSize, 400);
	npcInventories = ReadStructsArray(*(uintptr_t *)(Address + DATA_OFFSET_SERVERDATA_NPCINVENTORY), *(uintptr_t *)(Address + DATA_OFFSET_SERVERDATA_NPCINVENTORY + 8), InventoryHolder::StructSize, 100);
	tradeChatChannel = *(USHORT *)(Address + DATA_OFFSET_SERVERDATA_TRADECHAT);
	globalChatChannel = *(USHORT *)(Address + DATA_OFFSET_SERVERDATA_TRADECHAT + 8);
	lastActionId = *(USHORT *)(Address + DATA_OFFSET_SERVERDATA_LASTACTION);
	//Monster Info
	monserLevel = *(BYTE *)(Address + DATA_OFFSET_SERVERDATA_MONSTER_LEVEL);
	monsterRemaining = *(BYTE *)(Address + DATA_OFFSET_SERVERDATA_MONSTER_LEVEL + 1);	// 51 = 50+, 255 = N/A (Town, etc.)

	minimapIcons = ReadMinimapIconArray(*(uintptr_t *)(Address + DATA_MINIMAPICON_LIST), *(uintptr_t *)(Address + DATA_MINIMAPICON_LIST + 8), DATA_MINIMAPICON_STRUCT_SIZE, 50);
}

std::vector<uintptr_t> ServerData::ReadStructsArray(uintptr_t startAddress, uintptr_t endAddress, int structSize, int maxCountLimit)
{
	std::vector<uintptr_t> result;
	int range = (int)(endAddress - startAddress);
	if (range <= 0 || range > structSize * maxCountLimit)
	{
		//Log(L"Fixed possible memory leak");
		return result;
	}

	for (auto addr = startAddress; addr < endAddress; addr += structSize)
		result.push_back(addr);
	return result;
}

std::vector<USHORT> ServerData::ReadPassiveSkillIdArray(uintptr_t startAddress, uintptr_t endAddress)
{
	std::vector<USHORT> result;
	int range = (int)(endAddress - startAddress);
	if (range <= 0 || abs(range) > 500)
	{
		return result;
	}
	for (uintptr_t addr = startAddress; addr < endAddress; addr += 2)
		result.push_back(*(USHORT *)addr);
	return result;
}

std::vector<MinimapIcon> ServerData::ReadMinimapIconArray(uintptr_t startAddress, uintptr_t endAddress, int structSize, int maxCountLimit)
{
	std::vector<MinimapIcon> result;
	int range = (int)(endAddress - startAddress);
	if (range <= 0 || range > structSize * maxCountLimit)
	{
		return result;
	}

	for (auto addr = startAddress; addr < endAddress; addr += structSize)
	{
		MinimapIcon minimapIcon;
		if(minimapIcon.Update(addr))
			result.push_back(minimapIcon);
	}
	return result;
}
