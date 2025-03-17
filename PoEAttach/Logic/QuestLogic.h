#pragma once

#include <vector>
#include "Common.h"


enum QuestMode
{
	QM_None = 0,
	QM_FullScan,
	QM_Interaction,
	QM_Hunt,
	QM_InteractAndHunt,
	QM_GotoArea,
	QM_Reward,
	QM_PickupItem,
	QM_ItemsAndWall,
	QM_WAYPOINT,
};

enum InteractionKind
{
	IK_None = 0,
	IK_Npc,
	IK_Other,
	IK_Monster,
	IK_Npc_Spec,
	IK_Chest,
	IK_Npc_Element,
};

enum ObjectCacheType
{
	OCT_None = 0,
	OCT_Npc,
	OCT_Other,
};

struct QuestInfo
{
	BYTE	isSkip;
	int		questKey;
	int		questStep;
	WCHAR	*questName;
	DWORD	mode;
	DWORD	subMode;
	DWORD	reserve;
	WCHAR	*name;
	WCHAR	*szReserve;
	WCHAR	*areaName;
	BYTE	areaPartIndex;
	BYTE	preQuestKey;
};

struct ObjectCacheInfo
{
	BYTE type;
	WCHAR *name;
	WCHAR *path;
	WCHAR *areaName;
	IVector pos;
};

struct IconInfo
{
	DWORD partIndex;
	BYTE next;		// 다음 혹은 이전 부분 지역으로 이동하는 기발  0: 이전부분지역, 1: 다음 부분 지역, 2: 다음 지역, 0xFF:입구 아님
	BYTE unique;	// 입구로 현재 부분지역을 검출할수있는가 확인
	WCHAR *szIconName;
	WCHAR *szName;
	WCHAR *szPath;
	DWORD entityId;
};

struct SpecialAreaInfo
{
	WCHAR *szAreaName;		// 지역 이름
	DWORD partCount;		// 부분 지역개수
	DWORD waypointPartIndex;	// 거점 있는 지역 번호
	BYTE usePortal;			// 포탈주문서를 사용하겠는가 하는 체크
	std::vector<IconInfo> minimapIcons;	// 입구 정보
};

struct ClickNpcInfo
{
	WCHAR *TraderNpcName;
	BYTE	NpcElement;
};

extern std::vector<QuestInfo> g_pInfoQuest;
extern std::vector<SpecialAreaInfo> g_pInfoSpecialArea;
