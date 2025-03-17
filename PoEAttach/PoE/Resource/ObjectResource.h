#pragma once

#include <Windows.h>
#include <string>
#include "Utils/MurmurHash2.h"

#define OBJECT_RES_COUNT		10616
#define MONSTER_RES_COUNT		6056
#define NPC_RES_COUNT			387
#define CHEST_RES_COUNT			1048
#define EXP_RES_COUNT			100

enum NpcTalkType
{
	NTT_None = 0,
	NTT_Quest,
	NTT_PurchaseItem,
	NTT_SellItem,
};

struct PoEMonsterResource
{
	DWORD dwNameHash;
	WCHAR *szMetaPath;
	WCHAR *szName;
};

struct PoENPCResource
{
	DWORD dwNameHash;
	WCHAR *szMetaPath;
	WCHAR *szName;
};

struct PoEChestResource
{
	DWORD dwNameHash;
	WCHAR *szMetaPath;
	WCHAR *szName;
};

struct PoEEntityResoure
{
	DWORD dwNameHash;
	std::wstring strMetaPath;
	BYTE bEntityType;
	DWORD dwResIndex;
};

struct PoEExpLevel
{
	BYTE bLevel;
	DWORD dwExp;
};

extern PoEMonsterResource g_pResMonster[MONSTER_RES_COUNT];
extern PoENPCResource g_pResNpc[NPC_RES_COUNT];
extern PoEChestResource g_pResChest[CHEST_RES_COUNT];
extern PoEExpLevel g_pResExperienceLevels[EXP_RES_COUNT];

void GetNpcResource(std::wstring szPath, WCHAR* szName);
void GetChestResource(std::wstring szPath, WCHAR* szName);
void GetMonsterResource(std::wstring szPath, WCHAR* szName);
