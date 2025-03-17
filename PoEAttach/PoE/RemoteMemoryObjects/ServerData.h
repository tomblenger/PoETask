#pragma once

#include <vector>
#include "RemoteMemoryObject.h"
#include "PassiveSkill.h"
#include "MinimapIcon.h"

enum NetworkStateE
{
	None,
	Disconnected,
	Connecting,
	NS_Connected
};

enum PartyStatus
{
	PartyLeader,
	Invited,
	PartyMember,
	PS_None,
};

enum PartyAllocation
{
	FreeForAll,
	ShortAllocation,
	PermanentAllocation,
	PA_None,
	NotInParty = 160
};

enum CharacterClass
{
	Scion,
	Marauder,
	Ranger,
	Witch,
	Duelist,
	Templar,
	Shadow,
	CC_None
};

class ServerData : public RemoteMemoryObject
{
public:
	ServerData();
	void Update(uintptr_t addrPointer);

	static std::vector<uintptr_t> ReadStructsArray(uintptr_t startAddress, uintptr_t endAddress, int structSize, int maxCountLimit);
	static std::vector<USHORT> ReadPassiveSkillIdArray(uintptr_t startAddress, uintptr_t endAddress);
	static std::vector<MinimapIcon> ReadMinimapIconArray(uintptr_t startAddress, uintptr_t endAddress, int structSize, int maxCountLimit);

	BYTE playerClass;
	std::vector<USHORT> passiveSkillIds;
	std::vector<PassiveSkill> passiveSkills;
	int characterLevel;
	int passiveRefundPointsLeft;
	int questPassiveSkillPoints;
	int freePassiveSkillPointsLeft;
	int totalAscendencyPoints;
	int spentAscendencyPoints;
	int timeInGame;
	BYTE networkState;
	std::wstring league;
	BYTE partyAllocationType;
	int latency;
	//public List<ServerStashTab> PlayerStashTabs = > GetStashTabs(0x6480, 0x6488);
	//public List<ServerStashTab> GuildStashTabs = > GetStashTabs(0x6498, 0x64A0);
	BYTE partyStatus;
	std::wstring guildName;
	std::vector<USHORT> skillBarIds;
	std::vector<uintptr_t> nearestPlayers;
	std::vector<uintptr_t> playerInventories;
	std::vector<uintptr_t> npcInventories;
	std::vector<uintptr_t> guildInventories;
	USHORT tradeChatChannel;
	USHORT globalChatChannel;
	USHORT lastActionId;
	BYTE monserLevel;
	BYTE monsterRemaining;
	std::vector<MinimapIcon>	minimapIcons;
};