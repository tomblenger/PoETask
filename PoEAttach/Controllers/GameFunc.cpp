#include "stdafx.h"
#include "GameFunc.h"
#include "Patch.h"
#include "Common.h"

void	ReleaseItem(uintptr_t ServerData, DWORD dwTargetIndex, DWORD dwItemCellX, DWORD dwItemCellY)
{
	DWORD64 dwBuffer1[100];
	DWORD	dwBuffer2[2];

	memset(dwBuffer1, 0, sizeof(dwBuffer1));
	memset(dwBuffer2, 0, sizeof(dwBuffer2));

	dwBuffer1[61] = ServerData;
	dwBuffer1[56] = dwTargetIndex;

	dwBuffer2[0] = dwItemCellX;
	dwBuffer2[1] = dwItemCellY;

	reinterpret_cast<DWORD64(*)(DWORD64*, DWORD*)>(TO_POE(ADDR_ITEM_RELEASE))(dwBuffer1, dwBuffer2);
}

void	HoldItem(uintptr_t ServerData, DWORD dwInvenIndex, DWORD dwItemKey)
{
	DWORD64		dwBuffer[0x10];
	memset(dwBuffer, 0, sizeof(dwBuffer));

	DWORD64		dwSendBuffer = *(DWORD64*)(ServerData + DATA_OFFSET_MAKEHOLDITEMBUFF);

	dwBuffer[1] = DATA_HOLDITEM_OPCODE;
	dwBuffer[2] = dwInvenIndex;
	*(DWORD*)((BYTE*)&dwBuffer + DATA_OFFSET_HOLD_GETITEMKEY) = dwItemKey;

	reinterpret_cast<DWORD64(*)(DWORD64*, DWORD64)>(TO_POE(ADDR_MAKE_HOLDITEMBUFF))(dwBuffer, dwSendBuffer);
	DWORD64 dwResult = reinterpret_cast<DWORD64(*)(DWORD64, DWORD)>(TO_POE(ADDR_SEND_PACKET))(dwSendBuffer, 0);
}

void	ReleaseTradeItem(uintptr_t ServerData, DWORD dwInvenIndex, DWORD dwItemKey, BYTE bTargetCellX, BYTE bTargetCellY)
{
	reinterpret_cast<DWORD64(*)(DWORD64, DWORD, DWORD, BYTE, BYTE)>(TO_POE(ADDR_TRADEITEM_RELEASE))(ServerData, dwInvenIndex, dwItemKey, bTargetCellX, bTargetCellY);
}

void	SellItem(uintptr_t ServerData, DWORD npcInvenLastItemKey)
{
	DWORD64 dwBuffer[0x10];
	memset(dwBuffer, 0, sizeof(dwBuffer));

	DWORD64	dwSendBuffer = *(DWORD64*)(ServerData + DATA_OFFSET_MAKEHOLDITEMBUFF);

	dwBuffer[1] = DATA_SELLITEM_OPCODE;
	dwBuffer[2] = npcInvenLastItemKey;

	reinterpret_cast<DWORD64(*)(DWORD64*, DWORD64)>(TO_POE(ADDR_MAKE_SELLITEMBUFF))(dwBuffer, dwSendBuffer);
	reinterpret_cast<DWORD64(*)(DWORD64, DWORD)>(TO_POE(ADDR_SEND_PACKET))(dwSendBuffer, 0);
}

void	NextStash(uintptr_t ServerData, DWORD StashSubIndex)
{
	DWORD64	dwBuffer[0x10];
	memset(dwBuffer, 0, sizeof(dwBuffer));

	DWORD64 dwSendBuffer = *(DWORD64*)(ServerData + DATA_OFFSET_MAKEHOLDITEMBUFF);

	dwBuffer[1] = DATA_OFFSET_NEXT_STASH;
	dwBuffer[2] = StashSubIndex;

	reinterpret_cast<DWORD64(*)(DWORD64*, DWORD64)>(TO_POE(ADDR_MAKE_NEXTSTASHBUFF))(dwBuffer, dwSendBuffer);
	reinterpret_cast<DWORD64(*)(DWORD64, DWORD)>(TO_POE(ADDR_SEND_PACKET))(dwSendBuffer, 0);

}

void	EquipGemToItem(uintptr_t ServerData, DWORD dwInvenID, DWORD dwItemKey, DWORD dwSocketIndex)
{
	DWORD64		dwBuffer1[100];

	memset(dwBuffer1, 0, sizeof(dwBuffer1));

	dwBuffer1[61] = ServerData;
	dwBuffer1[56] = dwInvenID;

	reinterpret_cast<DWORD64(*)(DWORD64*, DWORD, DWORD)>(TO_POE(ADDR_EQUIP_GEM))(dwBuffer1, dwItemKey, dwSocketIndex);

}

void	DisarmingGem(uintptr_t ServerData, DWORD dwInvenID, DWORD dwItemKey, DWORD dwSocketIndex)
{
	DWORD64		dwBuffer1[100];

	memset(dwBuffer1, 0, sizeof(dwBuffer1));

	dwBuffer1[61] = ServerData;
	dwBuffer1[56] = dwInvenID;

	reinterpret_cast<DWORD64(*)(DWORD64*, DWORD, DWORD)>(TO_POE(ADDR_DISARMING_GEM))(dwBuffer1, dwItemKey, dwSocketIndex);
}

void	GemLevelUp(uintptr_t ServerData, DWORD dwInvenID, DWORD dwItemKey, DWORD dwSocketIndex)
{
	reinterpret_cast<DWORD64(*)(DWORD64, DWORD, DWORD, DWORD)>(TO_POE(ADDR_GEM_LEVELUP))(ServerData, dwInvenID, dwItemKey, dwSocketIndex);
}

void	RecvReward(uintptr_t ServerData, DWORD dwNpcItemKey, DWORD dwTargetInvenId, DWORD dwTargetCellX, DWORD dwTargetCellY, BYTE npcInvenIndex/* = 0*/)
{
	DWORD64		dwBuffer[0x10];
	memset(dwBuffer, 0, sizeof(dwBuffer));

	DWORD64		dwSendBuffer = *(DWORD64*)(ServerData + DATA_OFFSET_MAKEHOLDITEMBUFF);

	dwBuffer[1] = DATA_REC_REWARD_OPCODE;
	dwBuffer[2] = dwNpcItemKey;
	*(DWORD*)((BYTE*)&dwBuffer + 0x14) = dwTargetInvenId;
	dwBuffer[3] = dwTargetCellX;
	*(DWORD*)((BYTE*)&dwBuffer + 0x1C) = dwTargetCellY;
	dwBuffer[4] = npcInvenIndex;
	*(DWORD*)((BYTE*)&dwBuffer + 0x21) = 0;
	*(DWORD*)((BYTE*)&dwBuffer + 0x22) = 0;

	Log(L"a1+8 = %04X,a1 + 16 = %08X,a1 + 20 = %08X,a1 + 24 = %08X, a1 + 28 = %08X, a1 + 32 = %02X, a1 + 33 = %02X a1 + 34 = %02X", DATA_REC_REWARD_OPCODE, dwNpcItemKey, dwTargetInvenId, dwTargetCellX, dwTargetCellY, 0, 0, 0);

	reinterpret_cast<DWORD64(*)(DWORD64*, DWORD64)>(TO_POE(ADDR_REC_REWARDBUFF))(dwBuffer, dwSendBuffer);
	reinterpret_cast<DWORD64(*)(DWORD64, DWORD)>(TO_POE(ADDR_SEND_PACKET))(dwSendBuffer, 0);

}

void	UsingOrbToItem(uintptr_t ServerData, DWORD dwIndex1, DWORD dwItemKey1, DWORD dwIndex2, DWORD dwItemKey2)
{
	reinterpret_cast<DWORD64(*)(DWORD64, DWORD, DWORD, DWORD, DWORD)>(TO_POE(ADDR_IDENTIFY_ITEM))(ServerData, dwIndex1, dwItemKey1, dwIndex2, dwItemKey2);
}

void	ClickNpcElement(uintptr_t ServerData, BYTE bNpcElementId)
{
	reinterpret_cast<DWORD64(*)(DWORD64, BYTE)>(TO_POE(ADDR_CLICK_NPC_ELEMENT))(ServerData, bNpcElementId);
}

void	PlayGame(uintptr_t SelectCharacterState, std::wstring charName, DWORD64 nCharIndex)
{
	DWORD64		dwPlayAddress;

	dwPlayAddress = *(DWORD64*)(SelectCharacterState + DATA_OFFSET_PLAYGAME);
	reinterpret_cast<DWORD64(*)(DWORD64, std::wstring, DWORD64)>(TO_POE(ADDR_PLAY_GAME))(dwPlayAddress, charName, 0);
}

void DoPlayerAction(uintptr_t serverDataPtr, WORD actionId, DWORD64 entityId, int *targetPos, WORD modifier, bool setActionFlag/* = true*/)
{
	BYTE prevAction = *(BYTE *)(*(uintptr_t *)(serverDataPtr + DATA_OFFSET_SERVERDATA_INGAMEDATA) + DATA_OFFSET_INGAMEDATA_ACTION_FLAG);
	*(BYTE *)(*(uintptr_t *)(serverDataPtr + DATA_OFFSET_SERVERDATA_INGAMEDATA) + DATA_OFFSET_INGAMEDATA_ACTION_FLAG) = 2;
	reinterpret_cast<DWORD64(*)(DWORD64, DWORD64, DWORD64, int *, WORD, BYTE)>(TO_POE(ADDR_PLAYER_ACTION))(serverDataPtr, actionId, entityId, targetPos, modifier, 0);
	reinterpret_cast<DWORD64(*)(DWORD64)>(TO_POE(ADDR_PLAYERACTION_END))(serverDataPtr);
	*(BYTE *)(*(uintptr_t *)(serverDataPtr + DATA_OFFSET_SERVERDATA_INGAMEDATA) + DATA_OFFSET_INGAMEDATA_ACTION_FLAG) = prevAction;
}

void SwitchSkillSlotIndex(uintptr_t serverDataPtr, BYTE	slotIdx, WORD skillId)
{
	reinterpret_cast<DWORD64(*)(DWORD64, BYTE, WORD)> (TO_POE(ADDR_SWITCHSKILLSLOTINDEX))(serverDataPtr, slotIdx, skillId);
}

void ChangeAreaByWaypoint(uintptr_t serverDataPtr, DWORD waypointId, DWORD targetAreaId, BYTE mod/* = 0*/)
{
	reinterpret_cast<DWORD64(*)(DWORD64, DWORD, DWORD, BYTE)> (TO_POE(ADDR_CHANGEAREABYWAYPOINT))(serverDataPtr, waypointId, targetAreaId, mod);
}

void	SkipTutorial(uintptr_t ServerData, uintptr_t dwSkipLayoutAddr)
{
	DWORD64 dwBuffer[0x10];
	memset(dwBuffer, 0, sizeof(dwBuffer));

	DWORD64 dwSendBuffer = *(DWORD64*)(ServerData + DATA_OFFSET_MAKEHOLDITEMBUFF);

	dwBuffer[1] = DATA_SKIPTUTORIAL_OPCODE;
	dwBuffer[2] = DATA_SKIPTUTORIAL_VALUE;

	reinterpret_cast<DWORD64(*)(DWORD64*, DWORD64)>(TO_POE(ADDR_MAKE_SKIPTUTORIALBUFF))(dwBuffer, dwSendBuffer);
	reinterpret_cast<DWORD64(*)(DWORD64, DWORD)>(TO_POE(ADDR_SEND_PACKET))(dwSendBuffer, 0);
	reinterpret_cast<DWORD64(*)(DWORD64, BYTE)>(TO_POE(ADDR_SKIPTUTORIAL_LAYOUT))(dwSkipLayoutAddr, 0);
}

void	InputGamePassword(uintptr_t LoginStatePtr, DWORD64 dwWordIndex, char chWord)
{
	DWORD64	dwPasswordTextBoxAddr = *(DWORD64*)(LoginStatePtr + DATA_OFFSET_PASSWORDBOX);
	DWORD64	dwPasswordValueAddr = dwPasswordTextBoxAddr + 0x2D8;

	reinterpret_cast<DWORD64(*)(DWORD64, DWORD64, DWORD64)>(TO_POE(ADDR_INPUT_GAMEACCOUNTINFO))(dwPasswordValueAddr, dwWordIndex, chWord);
}

void	InputGameAccount(uintptr_t LoginStatePtr, DWORD64 dwWordIndex, char chWord)
{
	DWORD64	dwPasswordTextBoxAddr = *(DWORD64*)(LoginStatePtr + DATA_OFFSET_USERNAMEBOX);
	DWORD64	dwPasswordValueAddr = dwPasswordTextBoxAddr + 0x2D8;

	reinterpret_cast<DWORD64(*)(DWORD64, DWORD64, DWORD64)>(TO_POE(ADDR_INPUT_GAMEACCOUNTINFO))(dwPasswordValueAddr, dwWordIndex, chWord);
}

void	SettingGameAccount(uintptr_t LoginStatePtr, std::wstring username)
{
	DWORD64 dwUserNameBoxAddr = *(DWORD64*)(LoginStatePtr + DATA_OFFSET_USERNAMEBOX);
	*(std::wstring *)(dwUserNameBoxAddr + 0x2E8) = username;
}

std::wstring GetLoginedAccount(uintptr_t LoginStatePtr)
{
	std::wstring LoginedAccount = L"";
	DWORD64 dwUserNameBoxAddr = *(DWORD64*)(LoginStatePtr + DATA_OFFSET_USERNAMEBOX);
	if (*(DWORD64 *)(dwUserNameBoxAddr + 0x2E8))
	{
		LoginedAccount = *(std::wstring *)(dwUserNameBoxAddr + 0x2E8);
	}
	return LoginedAccount;
}

void	ResurrectAtCheckPoint(uintptr_t ServerData)
{
	reinterpret_cast<DWORD64(*)(DWORD64, BYTE)>(TO_POE(ADDR_RESSURECTATCHECKPOINT))(ServerData, 0);
}

void	PointPassiveSkill(uintptr_t ServerData, DWORD dwPassiveSkillID)
{
	DWORD64 dwBuffer[0x10];
	memset(dwBuffer, 0, sizeof(dwBuffer));

	DWORD64 dwSendBuffer = *(DWORD64*)(ServerData + DATA_OFFSET_MAKEHOLDITEMBUFF);

	dwBuffer[1] = DATA_OPCODE_PASSIVESKILL;
	dwBuffer[2] = dwPassiveSkillID;

	reinterpret_cast<DWORD64(*)(DWORD64*, DWORD64)>(TO_POE(ADDR_MAKE_PASSIVESKILLBUFF))(dwBuffer, dwSendBuffer);
	reinterpret_cast<DWORD64(*)(DWORD64, DWORD)>(TO_POE(ADDR_SEND_PACKET))(dwSendBuffer, 0);
}

void	UseItem(uintptr_t ServerData, DWORD dwInvenId, DWORD dwItemKey)
{
	reinterpret_cast<DWORD64(*)(DWORD64, DWORD, DWORD)>(TO_POE(ADDR_USE_ITEM))(ServerData, dwInvenId, dwItemKey);
}

void	DropItem(uintptr_t ServerData)
{
	reinterpret_cast<DWORD64(*)(DWORD64)>(TO_POE(ADDR_DROP_ITEM))(ServerData);
}

void	MakeNewAreaInstance(uintptr_t areaInstanceUiPtr, BYTE	bValue)
{
	reinterpret_cast<DWORD64(*)(DWORD64, BYTE)>(TO_POE(ADDR_MAKE_NEWAREA_INSTANCE))(areaInstanceUiPtr, bValue);
}

void	KillSpecialMonster(uintptr_t ServerData, BYTE bValue)
{
	reinterpret_cast<DWORD64(*)(DWORD64, BYTE)>(TO_POE(ADDR_SPECIALPROC_MONSTER_DIALOG))(ServerData, bValue);
}

DWORD64 GetQuestStatePtr(uintptr_t playerCompPtr, uintptr_t questPtr)
{
	DWORD64 pdwBuf[0x10];

	if (!playerCompPtr || !questPtr)
		return 0;
	memset(pdwBuf, 0, sizeof(pdwBuf));
	reinterpret_cast<DWORD64(*)(DWORD64, DWORD64 *, DWORD64, DWORD64)>(TO_POE(ADDR_QUEST_GETSTATE))(playerCompPtr, pdwBuf, questPtr + 0x10, 0);
	return pdwBuf[1];
}

void DoPlayerForwardSkillAction(uintptr_t serverDataPtr, WORD actionId, int *targetPos)
{	
	uintptr_t	pBuff[0x10];
	pBuff[0] = TO_POE(DATA_OFFSET_FORWARDSKILL2);
	pBuff[1] = DATA_OFFSET_FORWARDSKILL1;
	pBuff[2] = *(uintptr_t *)targetPos;
	pBuff[3] = actionId;
	uintptr_t addr = *(uintptr_t *)(serverDataPtr + DATA_OFFSET_FORWARDSKILL3);
	reinterpret_cast<DWORD64(*)(DWORD64*, DWORD64)>(TO_POE(ADDR_USE_FORWARDSKILL))(pBuff, addr);
	reinterpret_cast<DWORD64(*)(DWORD64, DWORD64)>(TO_POE(ADDR_SEND_PACKET))(addr, 0);
}

void	DoPlayerBuffSkillAction(uintptr_t serverDataPtr, WORD actionId, DWORD playerID)
{
	reinterpret_cast<DWORD64(*)(DWORD64, WORD, DWORD)>(TO_POE(ADDR_USE_BUFFSKILL))(serverDataPtr, actionId, playerID);
}
