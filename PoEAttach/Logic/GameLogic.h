#pragma once

#include <string>
#include <vector>
#include <map>
#include "Path.h"
#include "MinimapIcon.h"
#include "OtherData.h"
#include "ItemResource.h"

enum MainStatus
{
	MS_First = 0,
	MS_Login,
	MS_CharacterSelect,
	MS_Main,
	MS_NPC_Sell,
	MS_NPC_Purchase,
	MS_Stash_Deposit,
	MS_Stash_Withdraw,
};

enum TargetPathType
{
	TPT_None = 0,
	TPT_GotoObject,
	TPT_GotoNpc,
	TPT_GotoDeadPos,
	TPT_GotoEntrance,
	TPT_GotoWaypoint,
	TPT_ChaoPos,
};

class GameController;
class IngameState;
class IngameData;
class ServerData;
class IngameUIElements;
class EntityListWrapper;
class PlayerData;
class WorldArea;
class QuestData;
class ItemData;
struct QuestInfo;

struct GemInfo
{
	WCHAR	*szName;
	BYTE	socketColor;
};

struct PassiveSkillInfo
{
	int		nGraphId;
	WCHAR	*szId;
};

struct PurchaseItemInfo
{
	WCHAR *purchaseItemName;
	WCHAR *needItemName;
	BYTE  needItemCount;
	WCHAR *npcName;
	WCHAR *completeQuestName;
};

struct TradeNpcInfo
{
	WCHAR *TraderNpcName;
	BYTE	NpcElement[2];
};

struct DoorInfo
{
	WCHAR *name;
	BYTE isConnect;
	WCHAR *keyItem;
};

struct SpecialDoorInfo
{
	WCHAR	*name;	
	WCHAR	*path;
	float	range;
	BYTE	transition;
	BYTE	isPassed;
	int		delay;
};

struct SocketColorInfo
{
	BYTE equipType;
	DWORD redCount;
	DWORD greenCount;
	DWORD blueCount;
};

struct ItemStatsElement
{
	WCHAR *szName;
	int rating;
};

struct ItemStatsInfo
{
	DWORD elementCount;
	ItemStatsElement elements[20];
};

struct ItemStatsList
{
	DWORD equipType;
	DWORD statsCount;
	ItemStatsInfo statsList[20];
};

class MapData
{
public:
	MapData()
	{
		Init();
	}

	void Init()
	{
		if (blockBuffer)
			delete[] blockBuffer;
		blockBuffer = nullptr;
		mapWidth = mapHeight = 0;
		findPathStep = 30;
		areaName = L"";
		iconsCache.clear();
		entranceCache.clear();
		skipEntrances.clear();
	}

	
	BYTE	*blockBuffer;
	int		mapWidth;
	int		mapHeight;
	int		findPathStep;
	std::wstring				areaName;
	std::vector<MinimapIcon>	iconsCache;
	std::vector<DWORD>			entranceCache;
	std::vector<DWORD>			skipEntrances;
};

class CharacterSetting
{
public:
	CharacterSetting()
	{
		Init();
	}

	void Init()
	{
		newMapBlock = 0;
		questsDone.clear();
		firstOpenedWorldMap = 0;
		currentAreaPart = 0;
		toPartIndex = 0;
		selectedObjectID = 0;
		selectedNpcID = 0;
		prevEntranceCacheSize = 0;
		fullPathLoadCount = 0;

		InitEquip();
		InitPath();
		InitDeath();
		InitGlobalMove();
		InitQuest();
		InitMove();
		InitStash();
	}

	void InitEquip()
	{
		toEquipItemName = L"";
		toEquipInvenIndex = NOINDEX;
		toEquipCellX = 0;
		toEquipCellY = 0;
		holdType = 0;
	}

	void InitDeath()
	{
		isDead = false;
		posDead = { 0, 0 };
	}

	void InitPath()
	{
		targetPathType = TPT_None;
	}

	void InitGlobalMove()
	{
		globalMove = 0;
		globalAreas.clear();
	}

	void InitQuest()
	{
		isQuestDone = 0;
		currentQuestIndex = NOINDEX;
		prevQuestIndex = NOINDEX;
	}

	void InitMove()
	{
		prevMovePos = { 0, 0 };
		prevMoveTick = 0;
	}

	void InitStash()
	{
		prevStashIndex = 0;
	}

	BYTE			newMapBlock;			// 맵이 새로 적재되였는가
	std::wstring	toEquipItemName;		// 장착할 아이템 이름 (보석)
	DWORD			toEquipInvenIndex;		// 장착할 아이템 인벤 번호
	int				toEquipCellX;			// 장착할 아이템의 인벤 셀 X좌표
	int				toEquipCellY;			// 장착할 아이템의 인벤 셀 Y좌표
	BYTE			holdType;				// 아이템을 놓는 방식 (0: 장착, 1: 보석장착, 2: 창고)
	int				targetPathType;			// 목표 경로 형태
	bool			isDead;					// 사망 기발
	IVector			posDead;				// 사망한 위치
	BYTE			globalMove;				// 대역 이동 기발
	std::vector<int> globalAreas;			// 목적지까지 거치는 지역목록
	BYTE			isQuestDone;			// 현재 퀘스트를 진행하였는가 체크
	DWORD			currentQuestIndex;		// 현재 진행중인 퀘스트 번호
	DWORD			prevQuestIndex;			// 전 퀘스트 번호
	std::vector<int> questsDone;			// 이미 완료한 퀘스트 목록
	BYTE			firstOpenedWorldMap;	// 처음에 월드맵창을 켰는가를 확인
	IVector			prevMovePos;			// 이전 이동점
	DWORD64			prevMoveTick;			// 이전 이동 시작 티크
	DWORD			currentAreaPart;		// 현재 지역에서 부분번호
	DWORD			toPartIndex;			// 목적지 부분 번호
	DWORD			selectedObjectID;		// 입구가 선택되였는가 확인
	DWORD			selectedNpcID;			// 선택된 NPC 아이디
	DWORD			prevStashIndex;			// 이전에 선택한 스태쉬 인덱스
	DWORD			prevEntranceCacheSize;	// 이전 입구목록 개수
	DWORD			fullPathLoadCount;		// 전체 경로 적재 회수
};

class GameLogic
{
public:
	GameLogic();

	// Initializer
	void Init();

	// Logic Status
	void SetStatus(DWORD status, DWORD subStatus = 0, DWORD delay = 0, bool prevSet = false);
	inline void SetDelay(DWORD delay);
	void ToggleRun(BYTE isRun);

	// Iteration Utils
	DWORD GetNearestObjectByPath(IVector posCenter, float range, std::wstring path = L"", bool checkConnect = true);
	DWORD GetNearestObjectByRender(IVector posCenter, float range, std::wstring renderName, bool checkConnect = true, bool checkTransition = false);
	DWORD GetNearestSpecialObjectByRenderPath(IVector posCenter, float range, std::wstring renderName, std::wstring path, BYTE transition = 2);
	DWORD GetNearestObjectByID(IVector posCenter, float range, DWORD id, bool checkConnect = true, bool checkTransition = false);
	DWORD GetNearestMonster(IVector posCenter, float range, std::wstring name = L"");
	DWORD GetNearestAttackingMonster(IVector posCenter, float range, std::wstring name = L"");
	DWORD GetNearestWorldItem(IVector posCenter, float range, std::wstring name);
	DWORD GetNearestNpc(IVector posCenter, float range, std::wstring name = L"", bool checkConnect = true);
	DWORD GetNearestChest(IVector posCenter, float range, std::wstring name = L"");

	DWORD GetSkillBarIndexByName(std::wstring skillName);
	DWORD GetSkillBarIDByName(std::wstring skillName);
	DWORD GetSkillIndexByName(std::wstring skillName, WORD skillID = 0);
	DWORD GetSkillIndexByID(DWORD skillId);
	BYTE  IsEnableSkill(DWORD dwIndex);
	DWORD GetMyBuffIndex(std::wstring buffName);
	DWORD GetItemIndexByName(DWORD invenIndex, std::wstring itemName);

	BYTE GetEmptyCellInInven(int cellWidth, int cellHeight, bool remainEmpty = true, int *outEmptyX = nullptr, int *outEmptyY = nullptr, DWORD dwInvenIndex = MainInventory1);
	BYTE GetEmptyCellByItemData(ItemData *itemData, bool remainEmpty = true, int *outEmptyX = nullptr, int *outEmptyY = nullptr, DWORD dwInvenIndex = MainInventory1);
	DWORD GetEquipType(std::wstring itemPath);
	int GetStatsValue(ItemData *itemData, std::wstring statsName);
	DWORD GetSocketCountByColor(ItemData *itemData, int socketColor);
	DWORD GetNeedSocketCount(ItemData *itemData);
	int CompareStats(ItemData *item1, ItemData *item2);
	int CompareItem(ItemData *item1, ItemData *item2);
	bool CanEquipItem(ItemData *itemData);
	DWORD IsBetterThanEquip(ItemData *itemData, DWORD *outItemIndex = nullptr);
	BYTE IsGoodItemToPick(ItemData *itemData);
	bool CanUpgradeGem(ItemData *itemData);
	GemInfo * GetGemInfoByName(std::wstring gemName);
	DWORD GetItemIndexWithEmptySocket(int socketColor, DWORD *outItemIndex, DWORD *outSockeIndex);
	BYTE HasItemToSell(ItemData *itemData);
	DWORD GetItemIndexToSell();
	DWORD GetPurchaseItemIndex(std::wstring npcName);
	DWORD GetItemCountByType(std::wstring ItemName, DWORD dwInvenType, BYTE itemType);
	DWORD GetItemIndexToStore(BYTE bItemType);

	// Basic Function
	void MoveToPos(IVector pos);
	void SelectEntity(DWORD entityId);
	void UseSkill(WORD skillId, DWORD targetId);
	void UseSkill(WORD skillId, IVector pos);
	void MoveItem(DWORD dstInvenIndex, int cellX, int cellY);
	void MoveItemToCursor(DWORD invenIndex, DWORD itemKey);
	void EquipGem(DWORD dstInvenIndex, DWORD dstItemIndex, DWORD socketIndex);
	void UnequipGem(DWORD invenIndex, DWORD itemIndex, DWORD socketIndex);
	void LevelUpGem(DWORD invenIndex, DWORD itemIndex, DWORD socketIndex);
	void UseOrb(DWORD invenOrb, DWORD orbIndex, DWORD invenItem, DWORD dwItemIndex);
	void Resurrect(BYTE type = 0);
	void ReceiveReward(DWORD npcItemIndex, DWORD dstInvenIndex, int dstCellX, int dstCellY, BYTE npcInvenIndex = 0);
	void NpcElementClick(BYTE bNpcElementId);
	void TradeAccept(DWORD dwLastItemKey);
	void MoveTradeItem(DWORD dwInvenIndex, DWORD dwItemKey, BYTE bTargetCellX, BYTE bTargetCellY);
	void GetNextStashInfo(DWORD stashIndex);

	// Pathfinding Utils
	IVector ConvertPointsToBlock(IVector pos);
	IVector ConvertPointsFromBlock(IVector cpos);
	bool IsConnectedPoints(IVector pos1, IVector pos2);
	void FindFullPath();
	bool FindTargetPath(IVector posTarget);
	std::vector<int> FindAreaPath(std::wstring startArea, std::wstring endArea);
	DWORD GetGlobalAreaIndexByName(std::wstring areaName);

	// Logic Handler
	void ReadMapBlock();
	void LoadFullPath();
	bool LoadTargetPath(IVector posTarget);
	BYTE UsePortion();
	bool AutoEquip();
	bool AutoEquipItem();
	bool AutoEquipGem();
	bool AutoUpgradeGem();
	bool UseForwardSkill(IVector posTarget);
	bool UseBuffSkill();
	bool AttackTarget(IVector posCenter, float range, DWORD targetId);
	bool PickItems();
	bool OpenChests();
	bool OpenDoors();
	bool OpenSpecialDoors();
	bool IsOpenedSpecialDoors(std::wstring doorName = L"");
	bool ApplyPassiveSkills();
	bool CleanInven();
	BYTE InteractionOther(WCHAR* renderName);
	BYTE InteractionNpc(std::wstring renderName);
	bool InteractionMonster(WCHAR* szName, float range);
	bool UseSkillByKey(std::wstring skillName, DWORD targetId);
	bool UsePortalScroll();
	bool SellItemToNpc(std::wstring npcName, BYTE npcElement);
	bool PurchaseItem(std::wstring npcName, BYTE npcElement);
	bool StoreInvenItems();
	bool ProcessInTown();
	void RunField();
	void Main();

	// Quest Logic
	DWORD GetCurrentQuestIndex(bool checkArea = false);
	DWORD GetMinimapIconCacheIndex(std::wstring iconName, std::wstring objectName = L"", std::wstring path = L"", bool checkCanGoto = false);
	bool LoadMinimapIconTargetPath(DWORD dwIconIndex);
	void LoadObjectCache();
	void InitObjectCacheGoState();
	DWORD GetSpecialAreaIndex(std::wstring areaName);
	bool RegisterWaypoint();
	BYTE GotoWaypoint(bool isRegister = true, DWORD *outWaypointIndex = nullptr);
	bool ContinueDialog();
	bool ProcessReward();
	void QuestRun();

	static GameLogic *Instance;

	GameController		*_controller;
	IngameState			*_ingameState;
	IngameData			*_ingameData;
	ServerData			*_serverData;
	IngameUIElements	*_ingameUi;
	EntityListWrapper	*_entityList;
	PlayerData			*_localPlayer;
	WorldArea			*_worldArea;
	QuestData			*_questData;

	DWORD64	_tick;
	DWORD	_status;
	DWORD	_subStatus;
	DWORD	_prevStatus;
	DWORD	_prevSubStatus;
	DWORD	_delay;
	BYTE	_isRun;

	MapData	_map;
	CharacterSetting _setting;

	// Path
	IVector		*_fullPath;
	DWORD		_fullPathSize;
	IVector		*_targetPath;
	DWORD		_targetPathSize;
	IVector		*_pointsScan;
	int			_pointsScanSize;
	IVector		*_currentPath;
	DWORD		_currentPathSize;
	DWORD		_moveStep;
};
