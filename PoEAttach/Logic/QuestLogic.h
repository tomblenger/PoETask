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
	BYTE next;		// ���� Ȥ�� ���� �κ� �������� �̵��ϴ� ���  0: �����κ�����, 1: ���� �κ� ����, 2: ���� ����, 0xFF:�Ա� �ƴ�
	BYTE unique;	// �Ա��� ���� �κ������� �����Ҽ��ִ°� Ȯ��
	WCHAR *szIconName;
	WCHAR *szName;
	WCHAR *szPath;
	DWORD entityId;
};

struct SpecialAreaInfo
{
	WCHAR *szAreaName;		// ���� �̸�
	DWORD partCount;		// �κ� ��������
	DWORD waypointPartIndex;	// ���� �ִ� ���� ��ȣ
	BYTE usePortal;			// ��Ż�ֹ����� ����ϰڴ°� �ϴ� üũ
	std::vector<IconInfo> minimapIcons;	// �Ա� ����
};

struct ClickNpcInfo
{
	WCHAR *TraderNpcName;
	BYTE	NpcElement;
};

extern std::vector<QuestInfo> g_pInfoQuest;
extern std::vector<SpecialAreaInfo> g_pInfoSpecialArea;
