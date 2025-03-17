#pragma once

#include <string>

void	ReleaseItem(uintptr_t ServerData, DWORD dwTargetIndex, DWORD dwItemCellX, DWORD dwItemCellY);
void	HoldItem(uintptr_t ServerData, DWORD dwInvenIndex, DWORD dwItemKey);
void	ReleaseTradeItem(uintptr_t ServerData, DWORD dwInvenIndex, DWORD dwItemKey, BYTE bTargetCellX, BYTE bTargetCellY);
void	SellItem(uintptr_t ServerData, DWORD npcInvenLastItemKey);
void	NextStash(uintptr_t ServerData, DWORD StashSubIndex);
void	EquipGemToItem(uintptr_t ServerData, DWORD dwInvenID, DWORD dwItemKey, DWORD dwSocketIndex);
void	DisarmingGem(uintptr_t ServerData, DWORD dwInvenID, DWORD dwItemKey, DWORD dwSocketIndex);
void	GemLevelUp(uintptr_t ServerData, DWORD dwInvenID, DWORD dwItemKey, DWORD dwSocketIndex);
void	RecvReward(uintptr_t ServerData, DWORD dwNpcItemKey, DWORD dwTargetInvenId, DWORD dwTargetCellX, DWORD dwTargetCellY, BYTE npcInvenIndex = 0);
void	UsingOrbToItem(uintptr_t ServerData, DWORD dwIndex1, DWORD dwItemKey1, DWORD dwIndex2, DWORD dwItemKey2);
void	ClickNpcElement(uintptr_t ServerData, BYTE bNpcElementId);
void	PlayGame(uintptr_t SelectCharacterState, std::wstring charName, DWORD64 nCharIndex);
void	DoPlayerAction(uintptr_t serverDataPtr, WORD actionId, DWORD64 entityId, int *targetPos, WORD modifier, bool setActionFlag = true);
void	SwitchSkillSlotIndex(uintptr_t serverDataPtr, BYTE	slotIdx, WORD skillId);
void	ChangeAreaByWaypoint(uintptr_t serverDataPtr, DWORD	waypointId, DWORD targetAreaId, BYTE mod = 0);
void	SkipTutorial(uintptr_t ServerData, uintptr_t dwSkipLayoutAddr);
void	InputGamePassword(uintptr_t LoginStatePtr, DWORD64 dwWordIndex, char chWord);
void	InputGameAccount(uintptr_t LoginStatePtr, DWORD64 dwWordIndex, char chWord);
void	SettingGameAccount(uintptr_t LoginStatePtr, std::wstring username);
std::wstring GetLoginedAccount(uintptr_t LoginStatePtr);
void	ResurrectAtCheckPoint(uintptr_t ServerData);
void	PointPassiveSkill(uintptr_t ServerData, DWORD dwPassiveSkillID);
void	UseItem(uintptr_t ServerData, DWORD dwInvenId, DWORD dwItemKey);
void	DropItem(uintptr_t ServerData);
void	MakeNewAreaInstance(uintptr_t areaInstanceUiPtr, BYTE	bValue);
void	KillSpecialMonster(uintptr_t ServerData, BYTE bValue);	//0:kill, 1:help
DWORD64 GetQuestStatePtr(uintptr_t playerCompPtr, uintptr_t questPtr);
void	DoPlayerForwardSkillAction(uintptr_t serverDataPtr, WORD actionId, int *targetPos);
void	DoPlayerBuffSkillAction(uintptr_t serverDataPtr, WORD actionId, DWORD playerID);
