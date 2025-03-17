#pragma once

#include <Windows.h>
#include <vector>

#define			ITEM_RES_COUNT			5572
#define			ITEMCLASS_RES_COUNT		54
#define			ITEMEXP_RES_COUNT		7791

enum Inventories
{
	MainInventory1 = 0,
	BodyArmour1,
	Weapon1,
	Offhand1,
	Helm1,
	Amulet1,
	Ring1,
	Ring2,
	Gloves1,
	Boots1,
	Belt1,
	Flask1,
	Cursor1,
	Map1,
	Weapon2,
	Offhand2,
	StrMasterCrafting,
	StrDexMasterCrafting,
	DexMasterCrafting,
	DexIntMasterCrafting,
	IntMasterCrafting,
	StrIntMasterCrafting,
	PVPMasterCrafting,
	PassiveJewels1,
	AnimatedArmour1,
	GuildTag1,
	StashInventoryId,
	DivinationCardTrade,
	Darkshrine,
	TalismanTrade,
	Leaguestone1,
	BestiaryCrafting,
	IncursionSacrifice,
	BetrayalUnveiling,
	ItemSynthesisInput,
	ItemSynthesisOutput,
	Inven_Unknown_1,
	Inven_Unknown_2,
	Stash1,
	Stash2,
	Stash3,
	Stash4,
	Inventory_Max,
};

struct PoEResItem
{
	DWORD	dwKey;
	WCHAR	*szPath;
	DWORD	dwItemClassKey;
	BYTE	bCellWidth;
	BYTE	bCellHeight;
	WCHAR	*szName;
	DWORD	dwHash;
};

struct PoEResItemClass
{
	WCHAR *szID;
	WCHAR *szName;
	WCHAR *szCategory;
	DWORD dwType;
};

struct PoEResItemExp
{
	DWORD dwBaseItemTypeKey;
	DWORD dwItemCurrentLevel;
	DWORD dwExperience;
};

extern PoEResItem		g_pResItem[ITEM_RES_COUNT];
extern PoEResItemClass	g_pResItemClass[ITEMCLASS_RES_COUNT];
extern PoEResItemExp	g_pResItemExp[ITEMEXP_RES_COUNT];

PoEResItem *			GetItemResource(std::wstring szItemPath);
//PoEResItem *			GetItemResource(DWORD dwItemHashKey);
//uintptr_t				GetItemResource(std::wstring szItemPath);
