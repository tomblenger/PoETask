// PoEAttach.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "PoEAttach.h"
#include "Common.h"
#include "GameController.h"
#include "GameStateController.h"
#include "Offsets.h"
#include "EntityListWrapper.h"
#include "Entity.h"
#include "GameLogic.h"
#include "Patch.h"
#include "GameFunc.h"
#include "ItemResource.h"
#include "QuestLogic.h"

void Initialize()
{
	Offsets *offs = new Offsets;
	offs->DoPatternScans();

	GameController *controller = new GameController;
	GameLogic *pLogic = new GameLogic;
	pLogic->SetStatus(MS_First);
#ifdef POE_TEST
	GameStateController::Instance->UpdateStates();
	if (GameStateController::IsSelectCharacterState())
		pLogic->SetStatus(MS_CharacterSelect);
	else if (GameStateController::IsInGameState())
		pLogic->SetStatus(MS_Main);
#else
	pLogic->_isRun = 1;
#endif
}

void __stdcall MainTimer()
{
	GameLogic::Instance->Main();
}

void PressKeyAlternate(WCHAR wKey)
{
	SendNotifyMessage(g_hWndMain, WM_SETCURSOR, (WPARAM)g_hWndMain, 0x02040001);
	SendNotifyMessage(g_hWndMain, WM_KEYDOWN, (WPARAM)wKey, 0x00240001);
	if (isalpha(wKey) && isupper(wKey))
		SendNotifyMessage(g_hWndMain, WM_CHAR, wKey + 0x20, 0x00240001);
	else
		SendNotifyMessage(g_hWndMain, WM_CHAR, wKey, 0x00240001);
	SendNotifyMessage(g_hWndMain, WM_KEYUP, (WPARAM)wKey, 0xC0240001);
	SendNotifyMessage(g_hWndMain, WM_SETCURSOR, (WPARAM)g_hWndMain, 0x02000001);
}

void PressKeyDwon(WCHAR wKey)
{
	SetForegroundWindow(g_hWndMain);
	keybd_event((BYTE)wKey, 0, 0, 0);
	//keybd_event('C', 0, KEYEVENTF_KEYUP, 0);
}

void SetMouseCursor(int nX, int nY)
{
	POINT	pt;

	pt.x = nX;
	pt.y = nY;
	ClientToScreen(g_hWndMain, &pt);
	SetCursorPos(pt.x, pt.y);
	SendNotifyMessage(g_hWndMain, WM_MOUSEMOVE, 0, MAKELPARAM(nX, nY));
}

void	ClickDone(int nX, int nY)
{
	//g_bClickStep = 1;
	SendNotifyMessage(g_hWndMain, WM_MOUSEMOVE, 0, MAKELPARAM(nX, nY));
	SendNotifyMessage(g_hWndMain, WM_LBUTTONDOWN, MK_LBUTTON, MAKELPARAM(nX, nY));
	SendNotifyMessage(g_hWndMain, WM_LBUTTONUP, MK_LBUTTON, MAKELPARAM(nX, nY));
}

void	Click(int nX, int nY, BOOL bMoveCursor, BOOL bDelayClick)
{
	if (bMoveCursor)
	{
		POINT	pt;

		pt.x = nX;
		pt.y = nY;
		ClientToScreen(g_hWndMain, &pt);
		SetCursorPos(pt.x, pt.y);
	}
	SendNotifyMessage(g_hWndMain, WM_MOUSEMOVE, 0, MAKELPARAM(nX, nY));
	if (bDelayClick)
	{
		//g_bClickState = 1;
		//g_nClickX = nX;
		//g_nClickY = nY;
	}
	else
	{
		ClickDone(nX, nY);
	}
}

void	RClickDone(int nX, int nY)
{
	//g_bClickStep = 1;
	SendNotifyMessage(g_hWndMain, WM_MOUSEMOVE, 0, MAKELPARAM(nX, nY));
	SendNotifyMessage(g_hWndMain, WM_RBUTTONDOWN, MK_RBUTTON, MAKELPARAM(nX, nY));
	SendNotifyMessage(g_hWndMain, WM_RBUTTONUP, MK_RBUTTON, MAKELPARAM(nX, nY));
}

void	RClick(int nX, int nY, BOOL bMoveCursor, BOOL bDelayClick)
{
	if (bMoveCursor)
	{
		POINT	pt;

		pt.x = nX;
		pt.y = nY;
		ClientToScreen(g_hWndMain, &pt);
		SetCursorPos(pt.x, pt.y);
	}
	SendNotifyMessage(g_hWndMain, WM_MOUSEMOVE, 0, MAKELPARAM(nX, nY));
	if (bDelayClick)
	{
		//g_bRClickState = 1;
		//g_nRClickX = nX;
		//g_nRClickY = nY;
	}
	else
	{
		RClickDone(nX, nY);
	}
}

void	MClickDone(int nX, int nY)
{
	//g_bClickStep = 1;
	SendNotifyMessage(g_hWndMain, WM_MOUSEMOVE, 0, MAKELPARAM(nX, nY));
	SendNotifyMessage(g_hWndMain, WM_MBUTTONDOWN, MK_MBUTTON, MAKELPARAM(nX, nY));
	SendNotifyMessage(g_hWndMain, WM_MBUTTONUP, MK_MBUTTON, MAKELPARAM(nX, nY));
}

void	MClick(int nX, int nY, BOOL bMoveCursor, BOOL bDelayClick)
{
	if (bMoveCursor)
	{
		POINT	pt;

		pt.x = nX;
		pt.y = nY;
		ClientToScreen(g_hWndMain, &pt);
		SetCursorPos(pt.x, pt.y);
	}
	SendNotifyMessage(g_hWndMain, WM_MOUSEMOVE, 0, MAKELPARAM(nX, nY));
	if (bDelayClick)
	{
	}
	else
	{
		MClickDone(nX, nY);
	}
}


void DumpStatus()
{
	Log(L"DumpStatus");

	GameController::Instance->Update();
	Log(L"------------------------- Server Data -------------------------");
	ServerData *serverData = &GameStateController::InGameState.serverData;
	Log(L"Address = %p, PlayerClass = %d, CharacterLevel = %d, PassiveRefund = %d, QuestPassive = %d, FreePassive = %d, TotalAscPoints = %d, SpentAscPoints = %d, TimeInGame = %d, NetworkState = %d, League = [%s], PartyAllocation = %d, "
		"Latency = %d, PartyStatus = %d, Guild = [%s], TradeChat = %d, GlobalChat = %d, LastActionID = %04X, MonsterLevel = %d, MonsterRemaining = %d",
		serverData->Address,
		serverData->playerClass,
		serverData->characterLevel,
		serverData->passiveRefundPointsLeft,
		serverData->questPassiveSkillPoints,
		serverData->freePassiveSkillPointsLeft,
		serverData->totalAscendencyPoints,
		serverData->spentAscendencyPoints,
		serverData->timeInGame,
		serverData->networkState,
		serverData->league.c_str(),
		serverData->partyAllocationType,
		serverData->latency,
		serverData->partyStatus,
		serverData->guildName.c_str(),
		serverData->tradeChatChannel,
		serverData->globalChatChannel,
		serverData->lastActionId,
		serverData->monserLevel,
		serverData->monsterRemaining);

	Log(L"------------------------- Player Inventory -------------------------");
	for (size_t i = 0; i < GameController::Instance->playerInventories.size(); i++)
	{
		InventoryHolder *pInven = &GameController::Instance->playerInventories[i];
		Log(L"--- Inventory ID = %d, Addr = %p, InvenAddr = %p, Count = %d, Size = (%d, %d), Type = %d, Slot = %d, TotalItems = %d, ServerRequest = %d",
			pInven->id,
			pInven->Address,
			pInven->inventory.Address, 
			(int)pInven->inventory.slotItems.size(), 
			pInven->inventory.columns, pInven->inventory.rows,
			pInven->inventory.invenType,
			pInven->inventory.invenSlot,
			pInven->inventory.totalItemCounts,
			pInven->inventory.serverRequestCounter);
		for (size_t j = 0; j < pInven->inventory.slotItems.size(); j++)
		{
			ItemData *pItem = &pInven->inventory.slotItems[j].item;
			Log(L"Key = %08X Addr = %p Cell = %d/%d "
				"Name = [%s] Stack = %d/%d Quality = %d InvenHolder = %p Level = %d RequireLevel = %d Rarity = %d Identify = %d Charge = %d/%d/%d Size = %d/%d "
				"SocketCount = %d GemLevel = %d CurGemExp = %d MaxGemExp = %d Evasion = %d ArmourScore = %d EnergyShield = %d STR = %d DEX = %d INT = %d Path = [%s]",
				pInven->inventory.slotItems[j].itemKey,
				pInven->inventory.slotItems[j].Address,
				pInven->inventory.slotItems[j].location.xMin,
				pInven->inventory.slotItems[j].location.yMin,
				pItem->displayName.c_str(),
				pItem->stack.Size, pItem->stack.MaxStackSize,
				pItem->quality.ItemQuality,
				pItem->Address,
				pItem->mods.nItemLevel,
				pItem->mods.nItemRequireLevel,
				pItem->mods.nItemRarity,
				pItem->mods.bIdentified,
				pItem->charges.CurrentCharges, pItem->charges.ChargesPerUse, pItem->charges.ChargesMax,
				pItem->itemWidth,
				pItem->itemHeight,
				(int)pItem->itemSocket.lstSocketColor.size(),
				pItem->skillGem.GemLevel,
				pItem->skillGem.CurrentGemExp,
				pItem->skillGem.MaxGemExp,
				pItem->armour.EvasionScore,
				pItem->armour.AmourScore,
				pItem->armour.EnergyShieldScore,
				pItem->attribute.strength,
				pItem->attribute.dexterity,
				pItem->attribute.intelligence, 
				pItem->path.c_str());
			for (int k = 0; k < pItem->itemSocket.lstItemSocketInfo.size(); k++)
			{
				Log(L"    Socket Index = %d Name = [%s] Color = %d IsEquiped = %d CurGemExp = %d MaxGemExp = %d, GemLv = %d, Addr = %p",
					pItem->itemSocket.lstItemSocketInfo[k].SocketIndex,
					pItem->itemSocket.lstItemSocketInfo[k].SocketedName.c_str(),
					pItem->itemSocket.lstItemSocketInfo[k].SocketColor,
					pItem->itemSocket.lstItemSocketInfo[k].IsSocketEquiped,
					pItem->itemSocket.lstItemSocketInfo[k].CurrentGemExp,
					pItem->itemSocket.lstItemSocketInfo[k].MaxGemExp,
					pItem->itemSocket.lstItemSocketInfo[k].gemLevel,
					pItem->itemSocket.lstItemSocketInfo[k].Address);
			}

			std::wstring	logSocketLinks;
			WCHAR szTemp[MAX_NAME];
			logSocketLinks = L"";
			for (int n = 0; n < pItem->itemSocket.lstSocketLinkInfo.size(); n++)
			{
				logSocketLinks += L"[";
				for (int p = 0; p < pItem->itemSocket.lstSocketLinkInfo[n].size(); p++)
				{
					if (p == 0)
					{
						swprintf(szTemp, L"%d", pItem->itemSocket.lstSocketLinkInfo[n][p]);
					}
					else
					{
						swprintf(szTemp, L",%d", pItem->itemSocket.lstSocketLinkInfo[n][p]);
					}
					logSocketLinks += szTemp;
				}
				logSocketLinks += L"], ";
			}
			if (!logSocketLinks.empty())
				Log(L"SocketLink %s", logSocketLinks.c_str());

			for (int l = 0; l < pItem->mods.implicitMods.size(); l++)
			{
				for (int m = 0; m < pItem->mods.implicitMods[l].lstStatsDetailInfo.size(); m++)
				{
					Log(L"    Stats Name = %s Value = %d",
						pItem->mods.implicitMods[l].lstStatsDetailInfo[m].statsName.c_str(),
						pItem->mods.implicitMods[l].lstStatsDetailInfo[m].statsValue);
				}
			}

		}
	}


	Log(L"------------------------- NPC Inventory -------------------------");
	for (size_t i = 0; i < GameController::Instance->npcInventories.size(); i++)
	{
		InventoryHolder *pInven = &GameController::Instance->npcInventories[i];
		for (size_t j = 0; j < pInven->inventory.slotItems.size(); j++)
		{
			ItemData *pItem = &pInven->inventory.slotItems[j].item;
			Log(L"InvenId = %d ItemName = %s ItemKey = %08X InvenHolder = %p InvenAddress = %p InvenSlotAddress = %p ItemDataAddress = %p",
				pInven->id,
				pItem->displayName.c_str(),
				pInven->inventory.slotItems[j].itemKey,
				pInven->Address,
				pInven->inventory.Address,
				pInven->inventory.slotItems[j].Address,
				pItem->Address);
		}
	}

	Log(L"------------------------- Local Player ------------------------");
	PlayerData *pPlayer = &GameController::Instance->entityList->localPlayer;
 	Log(L"Name = [%s], ID = %08X, Pos = (%d, %d), HP = %d/%d/%.2f, MP = %d/%d/%.2f, ES = %d/%d/%.2f, Lv = %d, ATTRIBUTE = %d/%d/%d, Attack = %d, Move = %d, Addr = %p", 
		pPlayer->player.name.c_str(),
		pPlayer->id, 
		pPlayer->positionedComp.gridPos.x, pPlayer->positionedComp.gridPos.y, 
		pPlayer->life.curHP, pPlayer->life.maxHP, pPlayer->life.hpPercentage,
		pPlayer->life.curMana, pPlayer->life.maxMana, pPlayer->life.mpPercentage,
		pPlayer->life.curES, pPlayer->life.maxES, pPlayer->life.esPercentage,
 		pPlayer->player.level,
		pPlayer->player.strength, pPlayer->player.dexterity, pPlayer->player.intelligence, 
 		pPlayer->actor.isMoving, pPlayer->actor.isAttacking,
		pPlayer->player.Address);
 	for (size_t i = 0; i < pPlayer->life.buffs.size(); i++)
 	{
 		Log(L"   Buff  Name = [%s], Charge = %d, Time = %f/%f", 
			pPlayer->life.buffs[i].name.c_str(), 
			pPlayer->life.buffs[i].charges,
			pPlayer->life.buffs[i].timer, pPlayer->life.buffs[i].maxTime);
 	}
 	//for (int i = 0; i < pPlayer->stats.size(); i++)
 	//{
 	//	Log(L"   Stat  Name = %s, ID = %d, Value = %d", 
	//		pPlayer->stats[i].name.c_str(), 
	//		pPlayer->stats[i].id, 
	//		pPlayer->stats[i].val);
 	//}
	Log(L"------------------------- Skill -----------------------");
	for (size_t i = 0; i < GameStateController::InGameState.ingameUi.skillBarSkills.size(); i++)
	{
		SkillElement *pSkill = &GameStateController::InGameState.ingameUi.skillBarSkills[i];
		Log(L"%d- Addr = %p, ID = %04X, Name = [%s]", 
			i, 
			pSkill->Address, 
			pSkill->skillid, 
			pSkill->skillName.c_str());
	}
	Log(L"------------------------- Actor Skill -----------------------");
	for (size_t i = 0; i < pPlayer->actor.actorSkills.size(); i++)
 	{
 		Log(L"%d-  ADDR = %p, ID = %04X, Name = %s, CoolDown = %d, Use = %d/%d", 
			i, 
			pPlayer->actor.actorSkills[i].Address, 
			pPlayer->actor.actorSkills[i].id, pPlayer->actor.actorSkills[i].name.c_str(), 
 			pPlayer->actor.actorSkills[i].coolDown, 
			pPlayer->actor.actorSkills[i].isUsing, pPlayer->actor.actorSkills[i].isUsingPressed);
 	}

	Log(L"------------------------- Area -----------------------");
	WorldArea *pArea = &GameStateController::InGameState.data.currentWorldArea;
	Log(L"Name = [%s], ID = %s, Act = %d, Town = %d, Waypoint = %d, Lv = %d, AreaID = %d, Atlas = %d, MapWorlds = %d, Corrupt = %d, Mission = %d, Daily = %d, MapTrial = %d, Labyrinth = %d, Abyss = %d, Map = %d, EndGameMap = %d",
		pArea->name.c_str(),
		pArea->id.c_str(),
		pArea->act,
		pArea->isTown,
		pArea->hasWaypoint,
		pArea->areaLevel,
		pArea->worldAreaID,
		pArea->isAtlasMap,
		pArea->isMapWorlds,
		pArea->isCorruptedArea,
		pArea->isMissionArea,
		pArea->isDailyArea,
		pArea->isMapTrialArea,
		pArea->isLabyrinthArea,
		pArea->isAbyssArea,
		pArea->isMap,
		pArea->isEndGameMap);

	Log(L"------------------------- Quest -----------------------");
	for (size_t i = 0; i < GameStateController::InGameState.ingameUi.questData.quests.size(); i++)
	{
		Quest *pQuest = &GameStateController::InGameState.ingameUi.questData.quests[i];
		if (pQuest->step <= 0)
			continue;
		Log(L"%d-  ID = %s, Name = [%s], QuestID = %d, State = %d, Step = %d, Complete = %d, Act = %d, Key = %d, Text = [%s], [%s], Addr = %p",
			i,
			pQuest->id.c_str(),
			pQuest->name.c_str(),
			pQuest->questID,
			pQuest->questState,
			pQuest->step,
			pQuest->isCompleted,
			pQuest->act,
			pQuest->key,
			pQuest->state.questProgressText.c_str(), pQuest->state.questStateText.c_str(),
			pQuest->Address);
	}

	Log(L"------------------------- NPC -----------------------");
	for (size_t i = 0; i < GameController::Instance->entityList->npcs.size(); i++)
	{
		NpcData *pNpc = &GameController::Instance->entityList->npcs[i];
		Log(L"ID = %08X, Name = [%s], Pos = (%d, %d), Target = %d/%d, MinimapLabelVisible = %d, IconHead = %d, IgnoreHidden = %d, Addr = %p, Path = [%s]", 
			pNpc->id,
			pNpc->name.c_str(),
			pNpc->positionedComp.gridPos.x, pNpc->positionedComp.gridPos.y, 
			pNpc->targetable.isTargetable, pNpc->targetable.isTargeted, 
			pNpc->npc.isMinimapLabelVisible,
			pNpc->npc.hasIconOverhead,
			pNpc->npc.isIgnoreHidden,
			pNpc->npc.Address,
			pNpc->path.c_str());
	}
	Log(L"------------------------- Monster -----------------------");
	for (size_t i = 0; i < GameController::Instance->entityList->monsters.size(); i++)
	{
		MonsterData *pMonster = &GameController::Instance->entityList->monsters[i];
		Log(L"ID = %08X, Name = [%s], HP = %d/%d, Pos = (%d, %d), Target = %d/%d, Attack = %d, Move = %d, ActionId = %d, Reaction = %d, Path = [%s], Addr = %p", 
			pMonster->id, 
			pMonster->name.c_str(),
			pMonster->life.curHP, pMonster->life.maxHP,
			pMonster->positionedComp.gridPos.x, pMonster->positionedComp.gridPos.y, 
			pMonster->targetable.isTargetable, pMonster->targetable.isTargeted, 
			pMonster->actor.isAttacking, pMonster->actor.isMoving, pMonster->actor.actionID,
			pMonster->positionedComp.reaction,
			pMonster->path.c_str(),
			pMonster->Address);
	}
	Log(L"------------------------- Player -----------------------");
	for (size_t i = 0; i < GameController::Instance->entityList->players.size(); i++)
	{
		pPlayer = &GameController::Instance->entityList->players[i];
		Log(L"Name = [%s], ID = %08X, Pos = (%d, %d), HP = %d/%d, MP = %d/%d, ES = %d/%d, Lv = %d, ATTRIBUTE = %d/%d/%d, Move = %d, Attack = %d",
			pPlayer->player.name.c_str(),
			pPlayer->id,
			pPlayer->positionedComp.gridPos.x, pPlayer->positionedComp.gridPos.y,
			pPlayer->life.curHP, pPlayer->life.maxHP,
			pPlayer->life.curMana, pPlayer->life.maxMana,
			pPlayer->life.curES, pPlayer->life.maxES,
			pPlayer->player.level,
			pPlayer->player.strength, pPlayer->player.dexterity, pPlayer->player.intelligence,
			pPlayer->actor.isMoving, pPlayer->actor.isAttacking);
	}
	Log(L"------------------------- WorldItem -----------------------");
	for (size_t i = 0; i < GameController::Instance->entityList->worldItems.size(); i++)
	{
		WorldItemData *pWorldItem = &GameController::Instance->entityList->worldItems[i];
		ItemData *pItem = &pWorldItem->item;
		Log(L"ID = %08X, Pos = (%d, %d) "
			"Name = [%s] StackSize = %d Quality = %d InvenHolder = %p Level = %d RequireLevel = %d Rarity = %d Identify = %d CurChage = %d Size = %d/%d "
			"SocketCount = %d GemLevel = %d CurGemExp = %d MaxGemExp = %d Evasion = %d ArmourScore = %d EnergyShield = %d STR = %d DEX = %d INT = %d Path = [%s]",
			pWorldItem->id, 
			pWorldItem->positionedComp.gridPos.x, pWorldItem->positionedComp.gridPos.y,
			pItem->displayName.c_str(),
			pItem->stack.Size,
			pItem->quality.ItemQuality,
			pItem->Address,
			pItem->mods.nItemLevel,
			pItem->mods.nItemRequireLevel,
			pItem->mods.nItemRarity,
			pItem->mods.bIdentified,
			pItem->charges.CurrentCharges,
			pItem->itemWidth,
			pItem->itemHeight,
			(int)pItem->itemSocket.lstSocketColor.size(),
			pItem->skillGem.GemLevel,
			pItem->skillGem.CurrentGemExp,
			pItem->skillGem.MaxGemExp,
			pItem->armour.EvasionScore,
			pItem->armour.AmourScore,
			pItem->armour.EnergyShieldScore,
			pItem->attribute.strength,
			pItem->attribute.dexterity,
			pItem->attribute.intelligence,
			pItem->path.c_str());
	}

	Log(L"------------------------- Chest -----------------------");
	for (size_t i = 0; i < GameController::Instance->entityList->chests.size(); i++)
	{
		ChestsData *pChest = &GameController::Instance->entityList->chests[i];
		Log(L"ID = %08X, Name = [%s], Pos = (%d, %d) Open = %d, Lock = %d, Path = %s", 
			pChest->id, 
			pChest->name.c_str(),
			pChest->positionedComp.gridPos.x, pChest->positionedComp.gridPos.y,
	 		pChest->chest.isOpened, pChest->chest.isLocked,
			pChest->path.c_str());
 			//for (int i = 0; i < pChest->stats.size(); i++)
 			//{
 			//	Log(L"     Stat %d %d", pChest->stats[i].id, pChest->stats[i].val);
 			//}
	}

	Log(L"------------------------- Other Entity -----------------------");
	for (size_t i = 0; i < GameController::Instance->entityList->others.size(); i++)
	{
		OtherData *pOther = &GameController::Instance->entityList->others[i];
		if (pOther->path == L"Metadata/MiscellaneousObjects/Doodad")
			continue;

		Log(L"Addr = %p, Id = %08X, Path = %s, Pos = (%d, %d), Render = %s, Transition = (%d, %d)", 
			pOther->Address, 
			pOther->id,
			pOther->path.c_str(), 
			pOther->positionedComp.gridPos.x, pOther->positionedComp.gridPos.y, 
			pOther->render.name.c_str(), 
			pOther->transitionable.flag1, pOther->transitionable.flag2);
	}

	Log(L"------------------------- UI -----------------------, %p", GameStateController::InGameState.ingameUi.Address);
	Element *pElement = &GameStateController::InGameState.ingameUi.skillBar;
	Log(L"SkillBar-    Visible = %d, ChildCount = %d, Pos = (%.1f, %.1f), Size = (%.1f, %.1f), Scale = %f, Highlight = %d, Addr = %p, Root = %p, Parent = %p",
		pElement->isVisible,
		pElement->childCount,
		pElement->x, pElement->y,
		pElement->width, pElement->height,
		pElement->scale, 
		pElement->isHighlighted, 
		pElement->Address, 
		pElement->root, 
		pElement->parent);
	StashElement *pStashElement = &GameStateController::InGameState.ingameUi.stashElement;
	pElement = pStashElement;
	Log(L"Stash-    Tab = %d, Visible = %d, ChildCount = %d, Pos = (%.1f, %.1f), Size = (%.1f, %.1f), Scale = %f, Highlight = %d, Addr = %p, Root = %p, Parent = %p",
		pStashElement->indexVisibleStash,
		pElement->isVisible,
		pElement->childCount,
		pElement->x, pElement->y,
		pElement->width, pElement->height,
		pElement->scale,
		pElement->isHighlighted,
		pElement->Address,
		pElement->root,
		pElement->parent);
	pElement = &GameStateController::InGameState.ingameUi.interaction;
	Log(L"Interaction-    Visible = %d, ChildCount = %d, Pos = (%.1f, %.1f), Size = (%.1f, %.1f), Scale = %f, Highlight = %d, Addr = %p, Root = %p, Parent = %p",
		pElement->isVisible,
		pElement->childCount,
		pElement->x, pElement->y,
		pElement->width, pElement->height,
		pElement->scale,
		pElement->isHighlighted,
		pElement->Address,
		pElement->root,
		pElement->parent);
	pElement = &GameStateController::InGameState.ingameUi.purchaseItem;
	Log(L"PurchaseItem-    Visible = %d, ChildCount = %d, Pos = (%.1f, %.1f), Size = (%.1f, %.1f), Scale = %f, Highlight = %d, Addr = %p, Root = %p, Parent = %p",
		pElement->isVisible,
		pElement->childCount,
		pElement->x, pElement->y,
		pElement->width, pElement->height,
		pElement->scale,
		pElement->isHighlighted,
		pElement->Address,
		pElement->root,
		pElement->parent);
	pElement = &GameStateController::InGameState.ingameUi.sellItem;
	Log(L"SellItem-    Visible = %d, ChildCount = %d, Pos = (%.1f, %.1f), Size = (%.1f, %.1f), Scale = %f, Highlight = %d, Addr = %p, Root = %p, Parent = %p",
		pElement->isVisible,
		pElement->childCount,
		pElement->x, pElement->y,
		pElement->width, pElement->height,
		pElement->scale,
		pElement->isHighlighted,
		pElement->Address,
		pElement->root,
		pElement->parent);
	pElement = &GameStateController::InGameState.ingameUi.inventory;
	Log(L"Inventory-    Visible = %d, ChildCount = %d, Pos = (%.1f, %.1f), Size = (%.1f, %.1f), Scale = %f, Highlight = %d, Addr = %p, Root = %p, Parent = %p",
		pElement->isVisible,
		pElement->childCount,
		pElement->x, pElement->y,
		pElement->width, pElement->height,
		pElement->scale,
		pElement->isHighlighted,
		pElement->Address,
		pElement->root,
		pElement->parent);
	pElement = &GameStateController::InGameState.ingameUi.reward;
	Log(L"Reward-    Visible = %d, ChildCount = %d, Pos = (%.1f, %.1f), Size = (%.1f, %.1f), Scale = %f, Highlight = %d, Addr = %p, Root = %p, Parent = %p",
		pElement->isVisible,
		pElement->childCount,
		pElement->x, pElement->y,
		pElement->width, pElement->height,
		pElement->scale,
		pElement->isHighlighted,
		pElement->Address,
		pElement->root,
		pElement->parent);
	pElement = &GameStateController::InGameState.ingameUi.passiveSkillPanel;
	Log(L"PassiveSkillPanel-    Visible = %d, ChildCount = %d, Pos = (%.1f, %.1f), Size = (%.1f, %.1f), Scale = %f, Highlight = %d, Addr = %p, Root = %p, Parent = %p",
		pElement->isVisible,
		pElement->childCount,
		pElement->x, pElement->y,
		pElement->width, pElement->height,
		pElement->scale,
		pElement->isHighlighted,
		pElement->Address,
		pElement->root,
		pElement->parent);
	pElement = &GameStateController::InGameState.ingameUi.passiveSkillUp;
	Log(L"PassiveSkillUp-    Visible = %d, ChildCount = %d, Pos = (%.1f, %.1f), Size = (%.1f, %.1f), Scale = %f, Highlight = %d, Addr = %p, Root = %p, Parent = %p",
		pElement->isVisible,
		pElement->childCount,
		pElement->x, pElement->y,
		pElement->width, pElement->height,
		pElement->scale,
		pElement->isHighlighted,
		pElement->Address,
		pElement->root,
		pElement->parent);
	pElement = &GameStateController::InGameState.ingameUi.worldmap;
	Log(L"WorldMap-    Visible = %d, ChildCount = %d, Pos = (%.1f, %.1f), Size = (%.1f, %.1f), Scale = %f, Highlight = %d, Addr = %p, Root = %p, Parent = %p",
		pElement->isVisible,
		pElement->childCount,
		pElement->x, pElement->y,
		pElement->width, pElement->height,
		pElement->scale,
		pElement->isHighlighted,
		pElement->Address,
		pElement->root,
		pElement->parent);
	pElement = &GameStateController::InGameState.ingameUi.specialProcMonsterPanel;
	Log(L"SpecialProcMonster-    Visible = %d, ChildCount = %d, Pos = (%.1f, %.1f), Size = (%.1f, %.1f), Scale = %f, Highlight = %d, Addr = %p, Root = %p, Parent = %p",
		pElement->isVisible,
		pElement->childCount,
		pElement->x, pElement->y,
		pElement->width, pElement->height,
		pElement->scale,
		pElement->isHighlighted,
		pElement->Address,
		pElement->root,
		pElement->parent);

	Log(L"------------------------- Passive Skills -----------------------");
	for (DWORD i = 0; i < GameStateController::InGameState.serverData.passiveSkills.size(); i++)
	{
		auto passiveSkill = &GameStateController::InGameState.serverData.passiveSkills[i];
		Log(L"Key = %d, ID = %s, GraphID = %d, Name = [%s]", 
			passiveSkill->key,
			passiveSkill->id.c_str(),
			passiveSkill->graphId,
			passiveSkill->name.c_str());
	}

	Log(L"------------------------- Minimap Icons -----------------------");
	for (DWORD i = 0; i < GameStateController::InGameState.serverData.minimapIcons.size(); i++)
	{
		auto icon = &GameStateController::InGameState.serverData.minimapIcons[i];
		Log(L"Valid = %d, Name = [%s], Path = [%s], Icon = [%s], Pos = (%d, %d), Addr = %p",
			icon->isValid, 
			icon->name.c_str(),
			icon->path.c_str(),
			icon->iconName.c_str(),
			icon->pos.gridPos.x, icon->pos.gridPos.y,
			icon->Address);
	}
	Log(L"------------------------- Minimap Icons Cache -----------------------");
	for (DWORD i = 0; i < GameLogic::Instance->_map.iconsCache.size(); i++)
	{
		auto icon = &GameLogic::Instance->_map.iconsCache[i];
		Log(L"Valid = %d, Name = [%s], Path = [%s], Icon = [%s], Pos = (%d, %d), CanGo = %d, Addr = %p",
			icon->isValid,
			icon->name.c_str(),
			icon->path.c_str(),
			icon->iconName.c_str(),
			icon->pos.gridPos.x, icon->pos.gridPos.y,
			icon->canGoto,
			icon->Address);
	}

	Log(L"------------------------- Waypoint Active Areas -----------------------");
	for (DWORD i = 0; i < GameStateController::InGameState.ingameUi.teleportFieldIds.size(); i++)
	{
		Log(L"ID = %d",
			GameStateController::InGameState.ingameUi.teleportFieldIds[i]);
	}

	GameStateController::SelectCharacter.Update(GameStateController::SelectCharacterStatePtr);

	Log(L"------------------------- Character List -----------------------");
	for (size_t i = 0; i < GameStateController::SelectCharacter.lstCharInfo.size(); i++)
	{
		Log(L"Index = %d Name = %s League = %s Level = %d",
			i,
			GameStateController::SelectCharacter.lstCharInfo[i].Name.c_str(),
			GameStateController::SelectCharacter.lstCharInfo[i].League.c_str(),
			GameStateController::SelectCharacter.lstCharInfo[i].Level);
	}
}

void DumpMapBlock()
{
	FILE *fp;
	WCHAR szPath[MAX_PATH];
	BYTE *pBuffer;
	int i, nIndex;

	uintptr_t mapInfoPtr = *(uintptr_t *)(GameStateController::InGameState.Address + DATA_OFFSET_INGAMESTATE_MAP);
	if (!mapInfoPtr)
		return;
	uintptr_t addr = *(uintptr_t *)mapInfoPtr;
	if (!addr)
		return;

	addr += DATA_OFFSET_MAP_BLOCK;

	IVector posMin, posMax;
	posMin.x = 1;
	posMin.y = 1;
	posMax.x = *(int *)(addr + 0x08);
	posMax.y = *(int *)(addr + 0x0C);

	swprintf(szPath, L"D:\\%s(%dX%d).dump", GameStateController::InGameState.data.currentWorldArea.name.c_str(), posMax.x, posMax.y);
	fp = _wfopen(szPath, L"wb");
	if (!fp)
		return;

	fwrite(&posMin, sizeof(IVector), 1, fp);
	fwrite(&posMax, sizeof(IVector), 1, fp);
	int nWidth = posMax.x - posMin.x + 1;
	pBuffer = new BYTE[nWidth];

	for (int y = posMin.y; y <= posMax.y; y++)
	{
		memset(pBuffer, 0, nWidth);
		for (int x = posMin.x; x <= posMax.x; x++)
		{
			nIndex = (x - posMin.x) + (y - posMin.y) * nWidth;
			i = (x - posMin.x);
			pBuffer[i] = *(BYTE *)(*(uintptr_t *)addr + nIndex);
		}
		fwrite(pBuffer, 1, nWidth, fp);
	}

	fclose(fp);

	delete[] pBuffer;
}

void Test3()
{
	auto setting = &GameLogic::Instance->_setting;
	Log(L"Status = %d-%d, Path = %d, Part = %d", GameLogic::Instance->_status, GameLogic::Instance->_subStatus, setting->targetPathType, setting->currentAreaPart);
}

void TestMap()
{
	//Log(L"Ingamestate %p", GameStateController::InGameState.Address);
	//uintptr_t mapInfoPtr = *(uintptr_t *)(GameStateController::InGameState.Address + 0x560);
	//Log(L"mapinfo %p", mapInfoPtr);
	//if (!mapInfoPtr)
	//	return;
	//uintptr_t addr = *(uintptr_t *)(mapInfoPtr + 0x118);
	//if (!addr)
	//	return;
	//Log(L"Size = %d, %d", *(DWORD *)(*(uintptr_t *)addr + 0x2C), *(DWORD *)(*(uintptr_t *)addr + 0x30));
	//
	//Positioned positionedComp;
	//positionedComp.Update(*(uintptr_t *)(GameController::Instance->entityList->pPlayer->Address + 0x50));
	//Log(L"Pos = [%d, %d]", positionedComp.gridPos.x, positionedComp.gridPos.y);
	DumpMapBlock();
}

void LocalPlayerPosition()
{
	GameController::Instance->Update();
	PlayerData *pLocalPlayer = &GameController::Instance->entityList->localPlayer;
	Log(L"{ %d, %d, L\"%s\" }, ", pLocalPlayer->positionedComp.gridPos.x, pLocalPlayer->positionedComp.gridPos.y, GameStateController::InGameState.data.currentWorldArea.name.c_str());

	//Log(L"%d", *(BYTE *)(GameStateController::InGameState.data.Address + 0x363));
	//MovePlayer(GameStateController::InGameState.serverData.Address, 469, 1443);
	//Log(L"%p", GameStateController::InGameState.serverData.Address);
	//Log(L"%p", GameStateController::InGameState.data.Address);
}

void TestEquip()
{
	Log(L"TestEquip");
	GameController::Instance->Update();
	//InventoryHolder *pInven = &GameController::Instance->playerInventories[MainInventory1];
	//for (size_t j = 0; j < pInven->inventory.slotItems.size(); j++)
	//{
	//	ItemData *pItem = &pInven->inventory.slotItems[j].item;
	//	Log(L"Item [%s], %d", pItem->displayName.c_str(), GameLogic::Instance->IsBetterThanEquip(pItem));
	//}
	//
	//int newX, newY;
	//BYTE bEmpty = GameLogic::Instance->GetEmptyCellInInven(1, 1, true, &newX, &newY);
	//Log(L"Empty %d (%d, %d)", bEmpty, newX, newY);
	//GameLogic::Instance->AutoEquipItem();
	GameLogic::Instance->AutoEquipGem();
}

void Test5()
{
	DumpMapBlock();
	//TestEquip();
}

void TestTutorial()
{
	Log(L"TestTutorial");
	GameController::Instance->Update();
	DWORD64 dwSkipLayoutAddr = *(DWORD64*)(GameStateController::InGameState.ingameUi.Address + DATA_OFFSET_SKIPTUTORIAL_LAYOUT);
	Log(L"  %p", dwSkipLayoutAddr);
}

std::vector<PassiveSkill> g_prevPassiveSkills;

void PrintNewPassivSkills()
{
	DWORD i, j;

	Log(L"------------------------- Passive Skills -----------------------");
	GameController::Instance->Update();
	auto serverData = &GameStateController::InGameState.serverData;
	for (i = 0; i < serverData->passiveSkills.size(); i++)
	{
		for (j = 0; j < g_prevPassiveSkills.size(); j++)
		{
			if (serverData->passiveSkills[i].graphId == g_prevPassiveSkills[j].graphId)
				break;
		}
		if (j >= g_prevPassiveSkills.size())
		{
			g_prevPassiveSkills.push_back(serverData->passiveSkills[i]);

			Log(L"{ %d, L\"%s\" },      [%s]", serverData->passiveSkills[i].graphId, serverData->passiveSkills[i].id.c_str(), serverData->passiveSkills[i].name.c_str());
		}
	}
}

void Test6()
{
	PrintNewPassivSkills();
	//TestTutorial();
}

void Test7()
{
	GameController::Instance->Update();
	if (GameLogic::Instance->_setting.currentQuestIndex != NOINDEX)
	{
		auto currentQuest = &g_pInfoQuest[GameLogic::Instance->_setting.currentQuestIndex];
		Log(L"Current quest-%d [%s], %d, %d, Mode = %d, SubMode = %d, Area = %s, Part = %d", 
			GameLogic::Instance->_setting.currentQuestIndex, 
			currentQuest->questName, 
			currentQuest->questKey, 
			currentQuest->questStep, 
			currentQuest->mode, 
			currentQuest->subMode, 
			currentQuest->areaName, 
			currentQuest->areaPartIndex);
	}
	Log(L"------------------------- Quest -----------------------");
	int questCount = 0;
	for (size_t i = 0; i < GameStateController::InGameState.ingameUi.questData.quests.size(); i++)
	{
		Quest *pQuest = &GameStateController::InGameState.ingameUi.questData.quests[i];
		if (pQuest->step <= 0)
			continue;
		if (pQuest->name == L"The Lord's Labyrinth")
			continue;

		Log(L"{ 0, %d, %d, L\"%s\", QM_FullScan, 0, 0, L\"\", L\"\", L\"%s\" }, ",
			pQuest->key,
			pQuest->step,
			pQuest->name.c_str(),
			GameStateController::InGameState.data.currentWorldArea.name.c_str());

		questCount++;
	}
	if (questCount > 0)
		return;
	for (size_t i = 0; i < GameStateController::InGameState.ingameUi.questData.quests.size(); i++)
	{
		Quest *pQuest = &GameStateController::InGameState.ingameUi.questData.quests[i];
		if (pQuest->step != -1)
			continue;

		Log(L"{ 0, %d, %d, L\"%s\", QM_FullScan, 0, 0, L\"\", L\"\", L\"%s\" }, ",
			pQuest->key,
			pQuest->step,
			pQuest->name.c_str(),
			GameStateController::InGameState.data.currentWorldArea.name.c_str());

		return;
	}
}

void TestGetQuestState()
{
	Log(L"TestGetQuestState");

	GameController::Instance->Update();
	
	for (size_t i = 0; i < GameStateController::InGameState.ingameUi.questData.quests.size(); i++)
	{
		Quest *pQuest = &GameStateController::InGameState.ingameUi.questData.quests[i];
		if (pQuest->name != L"Einhar's Hunt")
			continue;

		Log(L"{ 0, %d, %d, L\"%s\", QM_FullScan, 0, 0, L\"\", L\"\", L\"%s\" }, ",
			pQuest->key,
			pQuest->step,
			pQuest->name.c_str(),
			GameStateController::InGameState.data.currentWorldArea.name.c_str());

		DWORD64 pdwBuf[0x100];
		memset(pdwBuf, 0, sizeof(pdwBuf));
		reinterpret_cast<DWORD64(*)(DWORD64, DWORD64 *, DWORD64, DWORD64)>(TO_POE(ADDR_QUEST_GETSTATE))(GameController::Instance->entityList->localPlayer.player.Address, pdwBuf, pQuest->Address + 0x10, 0);

		DumpStack((DWORD64)pdwBuf, 0x10);
	}
	
}

void Test8()
{
	GameController::Instance->Update();
	PlayerData *pLocalPlayer = &GameController::Instance->entityList->localPlayer;
	Log(L"{ %d, %d, L\"%s\" }, ", 
		pLocalPlayer->positionedComp.gridPos.x, pLocalPlayer->positionedComp.gridPos.y, GameStateController::InGameState.data.currentWorldArea.name.c_str());
	//TestGetQuestState();
}