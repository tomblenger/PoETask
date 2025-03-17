#include "stdafx.h"
#include "GameLogic.h"
#include "Common.h"
#include "GameController.h"
#include "GameStateController.h"
#include "Offsets.h"
#include "EntityListWrapper.h"
#include "Entity.h"
#include "Patch.h"
#include "GameFunc.h"
#include "CalcPath.h"
#include "Skill.h"
#include "PoEAttach.h"
#include "ItemResource.h"
#include "PathFinder.h"
#include "Dijkstra.h"
#include "WorldAreaResource.h"
#include "QuestLogic.h"
#include "Gem.h"

#define MAP_BLOCK_POINTS					3
#define INVENTORY_CELL_WIDTH				12
#define INVENTORY_CELL_HEIGHT				5
#define FLASK_SLOT_COUNT					5

#define AREA_MUD_FLATS						L"The Mud Flats"
#define AREA_CHAMBER_LV1					L"The Chamber of Sins Level 1"

enum SocketColor
{
	SC_None = 0,
	SC_Red,
	SC_Green,
	SC_Blue,
};

class AreaNode : public DijkstraNode
{
public:
	AreaNode()
	{
		key = 0;
	}

	std::wstring name;
	int key;
};

std::vector<GemInfo> g_pInfoGem
{
	{ L"Fireball", SC_Blue }, 
	{ L"Freezing Pulse", SC_Blue },
	{ L"Arcane Surge Support", SC_Blue },
	{ L"Lesser Poison Support", SC_Green },
	{ L"Elemental Proliferation Support", SC_Blue },
	//{ L"Magma Orb", SC_Blue },
	{ L"Clarity", SC_Blue },
	{ L"Added Lightning Damage Support", SC_Blue },
	{ L"Orb of Storms", SC_Blue },
	{ L"Storm Brand", SC_Blue },
	{ L"Herald of Thunder", SC_Blue },
	{ L"Frostblink", SC_Blue },
	{ L"Faster Casting Support", SC_Blue },
};

std::vector<PassiveSkillInfo> g_infoPassiveSkill
{
	// 1
	{ 57264, L"spell_damage721" },
	{ 33296, L"spell_damage740" },
	{ 1957, L"cast_speed418" },
	{ 739, L"cast_speed419" },
	{ 18866, L"cast_speed420" },
	{ 11420, L"savant901" },
	{ 4184, L"elemental_damage1644" },
	{ 30225, L"lightning_walker311" },
	{ 4397, L"intelligence918" },
	{ 31875, L"strength828" },
	{ 60398, L"intelligence882" },
	{ 42760, L"intelligence886" },
	{ 40366, L"intelligence909" },
	{ 6764, L"intelligence904" },
	{ 14057, L"life187" },
	{ 9386, L"life182" },
	{ 5743, L"life1161" },
	{ 58218, L"fitness1212" },
	{ 64210, L"intelligence912" },
	{ 26270, L"intelligence917" },
	{ 23083, L"totem_life20" },
	{ 22757, L"totem_damage1492" },
	{ 49939, L"totem_damage1493" },
	{ 42343, L"sigil_totem_keystone" },

	//2
	{ 55332, L"intelligence911" },
	{ 22088, L"elemental_overload_keystone2152" },
	{ 29033, L"sigil_damage2265" },
	{ 51954, L"sigil_damage2266" },
	{ 29861, L"sigil_notable_2_2267" },

	//3
	{ 29199, L"intelligence1995" },
	{ 23027, L"strength824" },
	{ 6712, L"life1209" },
	{ 36949, L"fitness1186" },
	{ 10031, L"life1210" },
	{ 15064, L"strength1001" },
	{ 12536, L"physical_and_elemental_damage_2223" },
	{ 33435, L"elementalist939" },
	{ 44799, L"physical_and_elemental_damage_2219" },
	{ 54694, L"spell_elemental_damage_notable1877" },
	{ 41472, L"discipline_and_training1194" },
	{ 61471, L"life1405" },
};

std::vector<WCHAR *> g_pInfoItemsToUse
{
	L"Book of Regrets",
	L"Book of Skill",
};

std::vector<DoorInfo> g_pInfoDoors
{
	{ L"Door", 1, L"" },
	//{ L"Locked Door", 1, L"" },
	{ L"Trial of Ascendancy", 1, L"" },
	{ L"Ancient Seal", 1, L"" },
	{ L"Crafting Recipe", 1, L"" },
	{ L"Sewer Grating", 0, L"Sewer Keys" },
};

std::vector<SpecialDoorInfo> g_pInfoSpecialDoors
{
	{ L"Arena", L"Metadata/Terrain/Mountain/Arena/Pit/Objects/PitGateTransition", 1.f, 2, 0 },
	{ L"The Bowels of the Beast", L"Metadata/Terrain/Mountain/Belly/Objects/BellyArenaTransition", 5.f, 1, 0 },
	{ L"Shavronne's Arena", L"Metadata/Terrain/Mountain/Belly/Objects/BellyArenaTransition", 6.f, 1, 0 },
	{ L"Maligaro's Arena", L"Metadata/Terrain/Mountain/Belly/Objects/BellyArenaTransition", 6.f, 1, 0 },
	{ L"Doedre's Arena", L"Metadata/Terrain/Mountain/Belly/Objects/BellyArenaTransition", 6.f, 1, 0 },
	{ L"The Black Heart", L"Metadata/MiscellaneousObjects/AreaTransition", 5.f, 0, 3 },
	{ L"Highgate", L"Metadata/QuestObjects/Act4/MalachaiDeathPortal", 5.f, 1, 2 },
	{ L"Lever", L"Metadata/Terrain/Act4/Area7/Objects/PortalDeviceLever", 5.f, 1, 2, 60 },
	{ L"Oriath Portal", L"Metadata/Terrain/Act4/Area7/Objects/PortalDeviceTransition", 5.f, 2, 2 },
	{ L"Cathedral Apex", L"Metadata/MiscellaneousObjects/AreaTransition", 5.f, 0, 3 },
};

std::vector<WCHAR *> g_pInfoQuestChests
{
	L"Large Chest",
};

std::vector<PurchaseItemInfo>	g_pInfoPurchaseItem
{
	{ L"Storm Brand", L"Orb of Transmutation", 1, L"Nessa", L"The Siren's Cadence" },
};

std::vector<TradeNpcInfo>	g_pInfoTradeNpcs
{
	//{L"Tarkleigh",{0x22,0x21}},
	{L"Nessa",				{0x15,0x14}},
	{L"Yeena",				{0x18,0x17}},
	{L"Greust",				{0x12,0x11}},
	{L"Clarissa",			{0x14,0x13}},
	{L"Petarus and Vanja",	{0x0F,0x0E}},
	{L"Lani",				{0x15,0x14}},
};

std::vector<WCHAR *>	g_pFirstAttackMonster
{
	L"Vaal Totem",
};

std::vector<SocketColorInfo>	g_pInfoSocketColor_Witch
{
	{ Weapon1, 1, 0, 2 },
	{ Helm1, 3, 0, 1 },
	{ Gloves1, 1, 1, 2 },
	{ BodyArmour1, 0, 1, 5 },
	{ Boots1, 0, 0, 4 },
};

/*
시전 속도 : base_cast_speed_+%
주문 피해 : spell_damage_+%
번개 피해 : lightning_damage_+%
화염 저항 : base_fire_damage_resistance_%
냉기 저항 : base_cold_damage_resistance_%
번개 저항 : base_lightning_damage_resistance_%
최대 생명력 : base_maximum_life
이동 속도 : base_movement_velocity_+%
원소 피해 : elemental_damage_+%
지능 추가 : additional_intelligence
*/

std::vector<ItemStatsList>		g_pInfoItemStats_Witch
{
	{ 
		Weapon1, 1,
		{
			{ 3, { { L"base_cast_speed_+%", 5 }, { L"spell_damage_+%", 3 }, { L"lightning_damage_+%", 2 } } },	// 시전 속도, 주문 피해, 번개 피해
		}
	},
	{
		Helm1, 2,
		{
			{ 3, { { L"base_fire_damage_resistance_%", 1 }, { L"base_cold_damage_resistance_%", 1 }, { L"base_lightning_damage_resistance_%", 1 } } },	// 화염 저항, 냉기 저항, 번개 저항
			{ 1, { { L"base_maximum_life", 1 } } }	// 최대 생명력
		}
	},
	{
		BodyArmour1, 2,
		{
			{ 3, { { L"base_fire_damage_resistance_%", 1 }, { L"base_cold_damage_resistance_%", 1 }, { L"base_lightning_damage_resistance_%", 1 } } },	// 화염 저항, 냉기 저항, 번개 저항
			{ 1, { { L"base_maximum_life", 1 } } }	// 최대 생명력
		}
	},
	{
		Gloves1, 2,
		{
			{ 3, { { L"base_fire_damage_resistance_%", 1 }, { L"base_cold_damage_resistance_%", 1 }, { L"base_lightning_damage_resistance_%", 1 } } },	// 화염 저항, 냉기 저항, 번개 저항
			{ 1, { { L"base_maximum_life", 1 } } }	// 최대 생명력
		}
	},
	{
		Boots1, 3,
		{
			{ 1, { { L"base_movement_velocity_+%", 1 } } },	//이동 속도
			{ 3, { { L"base_fire_damage_resistance_%", 1 }, { L"base_cold_damage_resistance_%", 1 }, { L"base_lightning_damage_resistance_%", 1 } } },	// 화염 저항, 냉기 저항, 번개 저항
			{ 1, { { L"base_maximum_life", 1 } } }	// 최대 생명력
		}
	},
	{
		Ring1, 3,
		{
			{ 3, { { L"base_cast_speed_+%", 5 }, { L"elemental_damage_+%", 2 }, { L"lightning_damage_+%", 3 } } },	// 시전 속도, 원소 피해, 번개 피해
			{ 3, { { L"base_fire_damage_resistance_%", 1 }, { L"base_cold_damage_resistance_%", 1 }, { L"base_lightning_damage_resistance_%", 1 } } },	// 화염 저항, 냉기 저항, 번개 저항
			{ 1, { { L"base_maximum_life", 1 } } }	// 최대 생명력
		}
	},
	{
		Amulet1, 3,
		{
			{ 3, { { L"base_cast_speed_+%", 5 }, { L"elemental_damage_+%", 2 }, { L"lightning_damage_+%", 3 } } },	// 시전 속도, 원소 피해, 번개 피해
			{ 3, { { L"base_fire_damage_resistance_%", 1 }, { L"base_cold_damage_resistance_%", 1 }, { L"base_lightning_damage_resistance_%", 1 } } },	// 화염 저항, 냉기 저항, 번개 저항
			{ 1, { { L"base_maximum_life", 1 } } }	// 최대 생명력
		}
	},
	{
		Belt1, 2,
		{
			{ 3, { { L"base_fire_damage_resistance_%", 1 }, { L"base_cold_damage_resistance_%", 1 }, { L"base_lightning_damage_resistance_%", 1 } } },	// 화염 저항, 냉기 저항, 번개 저항
			{ 1, { { L"base_maximum_life", 1 } } }	// 최대 생명력
		}
	},
};

GameLogic * GameLogic::Instance = nullptr;

GameLogic::GameLogic()
{
	Instance = this;
	_controller = GameController::Instance;
	_ingameState = &GameStateController::InGameState;
	_ingameData = &_ingameState->data;
	_serverData = &_ingameState->serverData;
	_ingameUi = &_ingameState->ingameUi;
	_entityList = _controller->entityList;
	_localPlayer = &_entityList->localPlayer;
	_worldArea = &_ingameData->currentWorldArea;
	_questData = &_ingameUi->questData;

	Init();
}

// ==================== Initializer ====================

void GameLogic::Init()
{
	_tick = 0;
	_status = MS_First;
	_subStatus = 0;
	_prevStatus = 0;
	_prevSubStatus = 0;
	_isRun = 0;

	_fullPath = nullptr;
	_fullPathSize = 0;
	_targetPath = nullptr;
	_targetPathSize = 0;
	_currentPath = nullptr;
	_currentPathSize = 0;
	_pointsScan = nullptr;
	_pointsScanSize = 0;
	_moveStep = NOINDEX;

	_map.Init();
	_setting.Init();
}

// =================== Logic Status ================

void GameLogic::SetStatus(DWORD status, DWORD subStatus/* = 0*/, DWORD delay/* = 0*/, bool prevSet/* = false*/)
{
	if (prevSet)
	{
		_prevStatus = _status;
		_prevSubStatus = _subStatus;
	}

	_status = status;
	_subStatus = subStatus;
	SetDelay(delay);
}

void GameLogic::SetDelay(DWORD delay)
{
	_delay = delay;
}

void GameLogic::ToggleRun(BYTE isRun)
{
	if (isRun)
	{
		if (!_isRun)
		{
			_isRun = 1;
			_setting.prevMoveTick = _tick;
			Log(L"Start");
		}
	}
	else
	{
		if (_isRun)
		{
			_isRun = 0;
			Log(L"Stop");
		}
	}
}

// ========================== Iteration Utils ===========================

DWORD GameLogic::GetNearestObjectByPath(IVector posCenter, float range, std::wstring path/* = L""*/, bool checkConnect/* = true*/)
{
	DWORD i, dwIndex = NOINDEX;
	float dist, distMin = 1000000000.f;

	for (i = 0; i < _entityList->others.size(); i++)
	{
		if (!path.empty() && _entityList->others[i].path != path)
			continue;
		dist = DIST(posCenter, _entityList->others[i].positionedComp.gridPos);
		if (dist > range)
			continue;
		if (checkConnect && !IsConnectedPoints(_localPlayer->positionedComp.gridPos, _entityList->others[i].positionedComp.gridPos))
			continue;

		dist = DIST(_localPlayer->positionedComp.gridPos, _entityList->others[i].positionedComp.gridPos);
		if (dist < distMin)
		{
			distMin = dist;
			dwIndex = i;
		}
	}

	return dwIndex;
}

DWORD GameLogic::GetNearestObjectByRender(IVector posCenter, float range, std::wstring renderName, bool checkConnect/* = true*/, bool checkTransition/* = false*/)
{
	DWORD i, dwIndex = NOINDEX;
	float dist, distMin = 1000000000.f;

	for (i = 0; i < _entityList->others.size(); i++)
	{
		if (_entityList->others[i].render.name != renderName)
			continue;
		dist = DIST(posCenter, _entityList->others[i].positionedComp.gridPos);
		if (dist > range)
			continue;
		if (checkTransition && (_entityList->others[i].transitionable.flag1 != 1))
			continue;
		if (checkConnect && !IsConnectedPoints(_localPlayer->positionedComp.gridPos, _entityList->others[i].positionedComp.gridPos))
			continue;

		dist = DIST(_localPlayer->positionedComp.gridPos, _entityList->others[i].positionedComp.gridPos);
		if (dist < distMin)
		{
			distMin = dist;
			dwIndex = i;
		}
	}

	return dwIndex;
}

DWORD GameLogic::GetNearestSpecialObjectByRenderPath(IVector posCenter, float range, std::wstring renderName, std::wstring path, BYTE transition/* = 2*/)
{
	DWORD i, dwIndex = NOINDEX;
	float dist, distMin = 1000000000.f;

	for (i = 0; i < _entityList->others.size(); i++)
	{
		if (_entityList->others[i].render.name != renderName)
			continue;
		if (_entityList->others[i].path != path)
			continue;
		if (_entityList->others[i].transitionable.flag1 != transition)
			continue;
		dist = DIST(posCenter, _entityList->others[i].positionedComp.gridPos);
		if (dist > range)
			continue;

		dist = DIST(_localPlayer->positionedComp.gridPos, _entityList->others[i].positionedComp.gridPos);
		if (dist < distMin)
		{
			distMin = dist;
			dwIndex = i;
		}
	}

	return dwIndex;
}

DWORD GameLogic::GetNearestObjectByID(IVector posCenter, float range, DWORD id, bool checkConnect/* = true*/, bool checkTransition/* = false*/)
{
	DWORD i, dwIndex = NOINDEX;
	float dist, distMin = 1000000000.f;

	for (i = 0; i < _entityList->others.size(); i++)
	{
		if (_entityList->others[i].id != id)
			continue;
		dist = DIST(posCenter, _entityList->others[i].positionedComp.gridPos);
		if (dist > range)
			continue;
		if (checkTransition && _entityList->others[i].transitionable.flag1 != 1)
			continue;
		if (checkConnect && !IsConnectedPoints(_localPlayer->positionedComp.gridPos, _entityList->others[i].positionedComp.gridPos))
			continue;

		dist = DIST(_localPlayer->positionedComp.gridPos, _entityList->others[i].positionedComp.gridPos);
		if (dist < distMin)
		{
			distMin = dist;
			dwIndex = i;
		}
	}

	return dwIndex;
}

DWORD GameLogic::GetNearestMonster(IVector posCenter, float range, std::wstring name/* = L""*/)
{
	DWORD i, j, dwIndex = NOINDEX;
	float dist, distMin = 1000000000.f;

	for (i = 0; i < g_pFirstAttackMonster.size(); i++)
	{
		for (j = 0; j < _entityList->monsters.size(); j++)
		{
			if (_entityList->monsters[j].name != g_pFirstAttackMonster[i])
				continue;
			if (!_entityList->monsters[j].targetable.isTargetable)
				continue;
			dist = DIST(posCenter, _entityList->monsters[j].positionedComp.gridPos);
			if (dist > range)
				continue;
			//if (!IsConnectedPoints(_localPlayer->positionedComp.gridPos, _entityList->monsters[j].positionedComp.gridPos))
			//	continue;
			return j;
		}
	}

	for (i = 0; i < _entityList->monsters.size(); i++)
	{
		if (!name.empty() && _entityList->monsters[i].name != name)
			continue;
		if (!_entityList->monsters[i].targetable.isTargetable)
			continue;
		if (name.empty() && _entityList->monsters[i].actor.actionID == Actor::None)
			continue;
		if (_entityList->monsters[i].path.find(L"League/") != -1)
			continue;
		dist = DIST(posCenter, _entityList->monsters[i].positionedComp.gridPos);
		if (dist > range)
			continue;

		if (name.empty() && !IsConnectedPoints(_localPlayer->positionedComp.gridPos, _entityList->monsters[i].positionedComp.gridPos))
			continue;

		dist = DIST(_localPlayer->positionedComp.gridPos, _entityList->monsters[i].positionedComp.gridPos);
		if (dist < distMin)
		{
			distMin = dist;
			dwIndex = i;
		}
	}

	return dwIndex;
}

DWORD GameLogic::GetNearestAttackingMonster(IVector posCenter, float range, std::wstring name/* = L""*/)
{
	DWORD i, dwIndex = NOINDEX;
	float dist, distMin = 1000000000.f;

	for (i = 0; i < _entityList->monsters.size(); i++)
	{
		if (!name.empty() && _entityList->monsters[i].name != name)
			continue;
		if (!_entityList->monsters[i].targetable.isTargetable)
			continue;
		if (!_entityList->monsters[i].actor.isAttacking)
			continue;
		if (_entityList->monsters[i].path.find(L"League/") != -1)
			continue;
		dist = DIST(posCenter, _entityList->monsters[i].positionedComp.gridPos);
		if (dist > range)
			continue;

		if (!IsConnectedPoints(_localPlayer->positionedComp.gridPos, _entityList->monsters[i].positionedComp.gridPos))
			continue;

		dist = DIST(_localPlayer->positionedComp.gridPos, _entityList->monsters[i].positionedComp.gridPos);
		if (dist < distMin)
		{
			distMin = dist;
			dwIndex = i;
		}
	}

	return dwIndex;
}

DWORD GameLogic::GetNearestWorldItem(IVector posCenter, float range, std::wstring name)
{
	DWORD i, dwIndex = NOINDEX;
	float dist, distMin = 1000000000.f;

	for (i = 0; i < _entityList->worldItems.size(); i++)
	{
		if (_entityList->worldItems[i].item.displayName != name)
			continue;
		dist = DIST(posCenter, _entityList->worldItems[i].positionedComp.gridPos);
		if (dist > range)
			continue;
		if (!IsConnectedPoints(_localPlayer->positionedComp.gridPos, _entityList->worldItems[i].positionedComp.gridPos))
			continue;

		dist = DIST(_localPlayer->positionedComp.gridPos, _entityList->worldItems[i].positionedComp.gridPos);
		if (dist < distMin)
		{
			distMin = dist;
			dwIndex = i;
		}
	}

	return dwIndex;
}

DWORD GameLogic::GetNearestNpc(IVector posCenter, float range, std::wstring name/* = L""*/, bool checkConnect/* = true*/)
{
	DWORD i, dwIndex = NOINDEX;
	float dist, distMin = 1000000000.f;

	for (i = 0; i < _entityList->npcs.size(); i++)
	{
		if (!name.empty() && _entityList->npcs[i].name != name)
			continue;
		if (!_entityList->npcs[i].targetable.isTargetable)
			continue;
		dist = DIST(posCenter, _entityList->npcs[i].positionedComp.gridPos);
		if (dist > range)
			continue;
		if (checkConnect && !IsConnectedPoints(_localPlayer->positionedComp.gridPos, _entityList->npcs[i].positionedComp.gridPos))
			continue;

		dist = DIST(_localPlayer->positionedComp.gridPos, _entityList->npcs[i].positionedComp.gridPos);
		if (dist < distMin)
		{
			distMin = dist;
			dwIndex = i;
		}
	}

	return dwIndex;
}

DWORD GameLogic::GetNearestChest(IVector posCenter, float range, std::wstring name/* = L""*/)
{
	DWORD i, dwIndex = NOINDEX;
	float dist, distMin = 1000000000.f;

	for (i = 0; i < _entityList->chests.size(); i++)
	{
		if (!name.empty() && _entityList->chests[i].name != name)
			continue;
		if (_entityList->chests[i].chest.isOpened)
			continue;
		if (_entityList->chests[i].chest.isLocked)
			continue;
		dist = DIST(posCenter, _entityList->chests[i].positionedComp.gridPos);
		if (dist > range)
			continue;
		if (!IsConnectedPoints(_localPlayer->positionedComp.gridPos, _entityList->chests[i].positionedComp.gridPos))
			continue;

		dist = DIST(_localPlayer->positionedComp.gridPos, _entityList->chests[i].positionedComp.gridPos);
		if (dist < distMin)
		{
			distMin = dist;
			dwIndex = i;
		}
	}

	return dwIndex;
}

DWORD GameLogic::GetSkillBarIndexByName(std::wstring skillName)
{
	DWORD i, dwIndex = NOINDEX;

	for (i = 0; i < _ingameUi->skillBarSkills.size(); i++)
	{
		if (_ingameUi->skillBarSkills[i].skillName == skillName)
			return i;
	}

	return dwIndex;
}

DWORD GameLogic::GetSkillBarIDByName(std::wstring skillName)
{
	DWORD i, dwIndex = NOINDEX;

	for (i = 0; i < _ingameUi->skillBarSkills.size(); i++)
	{
		if (_ingameUi->skillBarSkills[i].skillName == skillName)
			return _ingameUi->skillBarSkills[i].skillid;
	}

	return dwIndex;
}

DWORD GameLogic::GetSkillIndexByName(std::wstring skillName, WORD skillID/* = 0*/)
{
	DWORD i, dwIndex = NOINDEX;

	for (i = 0; i < _localPlayer->actor.actorSkills.size(); i++)
	{
		if (_localPlayer->actor.actorSkills[i].name == skillName)
		{
			if(!skillID || skillID == _localPlayer->actor.actorSkills[i].id)
				return i;
		}
	}

	return dwIndex;
}

DWORD GameLogic::GetSkillIndexByID(DWORD skillId)
{
	DWORD i, dwIndex = NOINDEX;

	for (i = 0; i < _localPlayer->actor.actorSkills.size(); i++)
	{
		if (_localPlayer->actor.actorSkills[i].id == skillId)
			return i;
	}

	return dwIndex;
}

BYTE GameLogic::IsEnableSkill(DWORD dwIndex)
{
	DWORD dwActorSkillIndex = NOINDEX;

	if (dwIndex >= _localPlayer->actor.actorSkills.size())
		return 0;
	//if (_localPlayer->actor.actorSkills[dwIndex].isUsing != 0)
	//	return 0;

	return 1;
}

DWORD GameLogic::GetMyBuffIndex(std::wstring buffName)
{
	DWORD i, dwIndex = NOINDEX;

	for (i = 0; i < _localPlayer->life.buffs.size(); i++)
	{
		if (_localPlayer->life.buffs[i].name == buffName)
			return i;
	}

	return dwIndex;
}

DWORD GameLogic::GetItemIndexByName(DWORD invenIndex, std::wstring itemName)
{
	if (invenIndex >= _controller->playerInventories.size())
		return NOINDEX;

	InventoryHolder *pInven = &_controller->playerInventories[invenIndex];
	for (DWORD i = 0; i < pInven->inventory.slotItems.size(); i++)
	{
		ItemData *pItem = &pInven->inventory.slotItems[i].item;
		if (pItem->displayName == itemName)
			return i;
	}

	return NOINDEX;
}

BYTE GameLogic::GetEmptyCellInInven(int cellWidth, int cellHeight, bool remainEmpty/* = true*/, int *outEmptyX/* = nullptr*/, int *outEmptyY/* = nullptr*/, DWORD dwInvenIndex /*= MainInventory1*/)
{
	BYTE bSuccess = 0;
	int i = 0, j = 0, k = 0, nInvenWidth, nInvenHeight;
	int nLeftSum, nUpSum, nMidSum, nSum;
	//int **pInventoryCell;
	int pInventoryCell[INVENTORY_CELL_WIDTH * 5][INVENTORY_CELL_WIDTH * 5];

	if (cellWidth == 0 || cellHeight == 0)		// Error
		return 0;
	if (dwInvenIndex >= _controller->playerInventories.size())
		return 0;

	auto pInven = &_controller->playerInventories[dwInvenIndex];

	nInvenWidth = pInven->inventory.columns;
	nInvenHeight = pInven->inventory.rows;

	if (remainEmpty)	// 퀘스트 아이템을 위한 빈자리를 남긴다
		nInvenWidth -= 2;

	// Initialize
	//pInventoryCell = new int*[dwInvenWidth];
	//for (i = 0; i < dwInvenWidth; i++)
	//{
	//	pInventoryCell[i] = new int[dwInvenHeight];
	//	memset(pInventoryCell[i], 0, sizeof(int) * dwInvenHeight);
	//}
	memset(pInventoryCell, 0, sizeof(pInventoryCell));

	// Set matrix non-empty cell
	for (i = 0; i < pInven->inventory.slotItems.size(); i++)
	{
		auto pItem = &pInven->inventory.slotItems[i];
		for (j = 0; j < pItem->item.itemWidth; j++)
		{
			for (k = 0; k < pItem->item.itemHeight; k++)
			{
				pInventoryCell[pItem->location.xMin + j][pItem->location.yMin + k] = 1;
			}
		}
	}

	// Sum matrix
	for (i = 0; i < nInvenWidth; i++)
	{
		for (j = 0; j < nInvenHeight; j++)
		{
			if (i == 0)
				nLeftSum = 0;
			else
				nLeftSum = pInventoryCell[i - 1][j];
			if (j == 0)
				nUpSum = 0;
			else
				nUpSum = pInventoryCell[i][j - 1];
			if (i > 0 && j > 0)
				nMidSum = pInventoryCell[i - 1][j - 1];
			else
				nMidSum = 0;

			pInventoryCell[i][j] += nLeftSum + nUpSum - nMidSum;
		}
	}

	// See if any cell is empty
	for (i = 0; i <= nInvenWidth - cellWidth; i++)
	{
		for (j = 0; j <= nInvenHeight - cellHeight; j++)
		{
			if (i == 0)
				nLeftSum = 0;
			else
				nLeftSum = pInventoryCell[i - 1][j + cellHeight - 1];
			if (j == 0)
				nUpSum = 0;
			else
				nUpSum = pInventoryCell[i + cellWidth - 1][j - 1];
			if (i > 0 && j > 0)
				nMidSum = pInventoryCell[i - 1][j - 1];
			else
				nMidSum = 0;

			nSum = pInventoryCell[i + cellWidth - 1][j + cellHeight - 1] + nMidSum - nLeftSum - nUpSum;

			if (nSum == 0)
			{
				if (outEmptyX)
					*outEmptyX = i;
				if (outEmptyY)
					*outEmptyY = j;
				bSuccess = 1;
				break;
			}
		}
		if (j <= nInvenHeight - cellHeight)
			break;
	}

	// Finalize
	//for (i = 0; i < dwInvenWidth; i++)
	//	delete[] pInventoryCell[i];
	//
	//delete[] pInventoryCell;

	return bSuccess;
}

BYTE GameLogic::GetEmptyCellByItemData(ItemData *itemData, bool remainEmpty/* = true*/, int *outEmptyX/* = nullptr*/, int *outEmptyY/* = nullptr*/, DWORD dwInvenIndex /*= MainInventory1*/)
{
	if (!itemData)
		return 0;
	if (dwInvenIndex >= _controller->playerInventories.size())
		return 0;

	// 스택으로 보관할수 있는가를 판단한다
	//auto pInven = &_controller->playerInventories[dwInvenIndex];
	//if (itemData->stack.Size > 0)
	//{
	//	for (DWORD i = 0; i < pInven->inventory.slotItems.size(); i++)
	//	{
	//		auto pItem = &pInven->inventory.slotItems[i];
	//		if (pItem->item.displayName == itemData->displayName && (pItem->item.stack.MaxStackSize - pItem->item.stack.Size) >= itemData->stack.Size)
	//		{
	//			if (outEmptyX)
	//				*outEmptyX = pItem->location.xMin;
	//			if (outEmptyY)
	//				*outEmptyY = pItem->location.yMin;
	//			return 1;
	//		}
	//	}
	//}

	return GetEmptyCellInInven(itemData->itemWidth, itemData->itemHeight, remainEmpty, outEmptyX, outEmptyY, dwInvenIndex);
}

DWORD GameLogic::GetEquipType(std::wstring itemPath)
{
	if (itemPath.find(L"/BodyArmours/") != -1)
		return BodyArmour1;
	else if (itemPath.find(L"/Weapons/") != -1)
		return Weapon1;
	else if (itemPath.find(L"/Shields/") != -1)
		return Offhand1;
	else if (itemPath.find(L"/Helmets/") != -1)
		return Helm1;
	else if (itemPath.find(L"/Amulets/") != -1)
		return Amulet1;
	else if (itemPath.find(L"/Rings/") != -1)
		return Ring1;
	//else if (itemPath.find(L"//") != -1)
	//	return Ring2;
	else if (itemPath.find(L"/Gloves/") != -1)
		return Gloves1;
	else if (itemPath.find(L"/Boots/") != -1)
		return Boots1;
	else if (itemPath.find(L"/Belts/") != -1)
		return Belt1;
	else if (itemPath.find(L"/Flasks/") != -1)
		return Flask1;
	//else if (itemPath.find(L"//") != -1)
	//	return Weapon2;
	//else if (itemPath.find(L"//") != -1)
	//	return Offhand2;

	return NOINDEX;
}

int GameLogic::GetStatsValue(ItemData *itemData, std::wstring statsName)
{
	for (int l = 0; l < itemData->mods.implicitMods.size(); l++)
	{
		for (int m = 0; m < itemData->mods.implicitMods[l].lstStatsDetailInfo.size(); m++)
		{
			if (itemData->mods.implicitMods[l].lstStatsDetailInfo[m].statsName == statsName)
				return itemData->mods.implicitMods[l].lstStatsDetailInfo[m].statsValue;
		}
	}

	return 0;
}

DWORD GameLogic::GetSocketCountByColor(ItemData *itemData, int socketColor)
{
	DWORD socketsCount = 0;

	for (DWORD k = 0; k < itemData->itemSocket.lstItemSocketInfo.size(); k++)
	{
		if (itemData->itemSocket.lstItemSocketInfo[k].SocketColor == socketColor)
			socketsCount++;
	}

	return socketsCount;
}

DWORD GameLogic::GetNeedSocketCount(ItemData *itemData)
{
	DWORD socketsCount = 0, maxCount = 0, redCount = 0, greenCount = 0, blueCount = 0;
	SocketColorInfo *pColorInfo = nullptr;

	DWORD equipType = GetEquipType(itemData->path);
	auto colors = &g_pInfoSocketColor_Witch;

	for (DWORD i = 0; i < colors->size(); i++)
	{
		if (colors->at(i).equipType == equipType)
		{
			pColorInfo = &colors->at(i);
			break;
		}
	}

	if (!pColorInfo)
		return 0;

	for (DWORD i = 0; i < itemData->itemSocket.lstSocketLinkInfo.size(); i++)
	{
		socketsCount = 0;
		redCount = 0; 
		greenCount = 0;
		blueCount = 0; 
		for (DWORD j = 0; j < itemData->itemSocket.lstSocketLinkInfo[i].size(); j++)
		{
			int index = itemData->itemSocket.lstSocketLinkInfo[i][j];
			if (index < 0 || index >= itemData->itemSocket.lstItemSocketInfo.size())
				continue;

			switch (itemData->itemSocket.lstItemSocketInfo[index].SocketColor)
			{
			case SC_Red:
			{
				if (pColorInfo->redCount > redCount)
					redCount++;
			}
			break;
			case SC_Green:
			{
				if (pColorInfo->greenCount > greenCount)
					greenCount++;
			}
			break;
			case SC_Blue:
			{
				if (pColorInfo->blueCount > blueCount)
					blueCount++;
			}
			break;
			}
			
		}

		socketsCount = redCount + greenCount + blueCount;
		if (maxCount < socketsCount)
			maxCount = socketsCount;
	}

	return maxCount;
}

int GameLogic::CompareStats(ItemData *item1, ItemData *item2)
{
	std::vector<ItemStatsList> *pStatsInfo = &g_pInfoItemStats_Witch;
	DWORD i, j, k;
	DWORD  equipType1;
	int statSum1, statSum2;
	
	if (!pStatsInfo)
		return 0;
	equipType1 = GetEquipType(item1->path);
	
	for (i = 0; i < pStatsInfo->size(); i++)
	{
		if (pStatsInfo->at(i).equipType == equipType1)
		{
			auto pStats = pStatsInfo->at(i).statsList;
			for (j = 0; j < pStatsInfo->at(i).statsCount; j++)
			{
				// 스텟 값들을 파라메터에 곱한 합을 계산하여 비교한다
				statSum1 = 0;
				statSum2 = 0;
				for (k = 0; k < pStats[j].elementCount; k++)
				{
					statSum1 += pStats[j].elements[k].rating * GetStatsValue(item1, pStats[j].elements[k].szName);
					statSum2 += pStats[j].elements[k].rating * GetStatsValue(item2, pStats[j].elements[k].szName);
				}

				if (statSum1 > statSum2)
					return 1;
				else if (statSum1 < statSum2)
					return -1;
			}

			break;
		}
	}

	//// intelligence 속성 비교
	//int INT1 = 0, INT2 = 0;
	//INT1 = GetStatsValue(item1, L"");
	//INT2 = GetStatsValue(item2, L"additional_intelligence");
	//if (INT1 > INT2)
	//	return 1;
	//else if (INT1 == INT2)

	return 0;
}

// 아이템1이 아이템2보다 더 좋으면 1, 같으면 0, 못하면 -1을 귀환한다 
int GameLogic::CompareItem(ItemData *item1, ItemData *item2)
{
	DWORD  equipType1, equipType2;

	equipType1 = GetEquipType(item1->path);
	equipType2 = GetEquipType(item2->path);

	// 아이템의 종류가 다른 경우는 비교할수 없다
	if (equipType1 == Weapon1 && equipType2 == Offhand1)
		return 1;

	if (equipType1 != equipType2 || equipType1 == NOINDEX)
		return 0;

	// 다른 종류의 물병인 경우도 비교할수 없다
	if (equipType1 == Flask1)
	{
		if (item1->path.substr(0, item1->path.length() - 2) != item2->path.substr(0, item1->path.length() - 2))
			return 0;
		if (item1->displayName.find(L"Life") == -1 && item2->displayName.find(L"Mana") == -1)		// HP, MP 물병만 비교한다
			return 0;
		if (item1->path > item2->path)
			return 1;
		else if (item1->path < item2->path)
			return -1;
	}
	
	//// 비교 순서 (sockets, stats, rarity, level, quality)
	// 스켓을 비교한다
	DWORD needSockets1 = 0, needSockets2 = 0;
	needSockets1 = GetNeedSocketCount(item1);
	needSockets2 = GetNeedSocketCount(item2);
	if (needSockets1 > needSockets2)
		return 1;
	else if (needSockets1 == needSockets2)
	{
		// 스텟을 비교한다
		int statsResult = CompareStats(item1, item2);
		if (statsResult > 0)
			return 1;

		if (statsResult == 0)
		{
			// 희귀성 비교
			if (item1->mods.nItemRarity > item2->mods.nItemRarity)
				return 1;
			else if (item1->mods.nItemRarity == item2->mods.nItemRarity)
			{
				// 아이템 레벨 비교
				if (item1->mods.nItemLevel > item2->mods.nItemLevel)
					return 1;
				else if (item1->mods.nItemLevel == item2->mods.nItemLevel)
				{
					// 아이템 질 비교
					if (item1->quality.ItemQuality > item2->quality.ItemQuality)
						return 1;
					else if (item1->quality.ItemQuality == item2->quality.ItemQuality)
						return 0;
				}
			}
		}
	}

	return -1;
}

bool GameLogic::CanEquipItem(ItemData *itemData)
{
	if (_localPlayer->player.level < itemData->mods.nItemRequireLevel)
		return false;
	if (_localPlayer->player.strength < itemData->attribute.strength)
		return false;
	if (_localPlayer->player.dexterity < itemData->attribute.dexterity)
		return false;
	if (_localPlayer->player.intelligence < itemData->attribute.intelligence)
		return false;
	return true;
}

// 장착된 아이템보다 좋은가를 판단하고 그보다 못한 아이템 번호를 돌려준다
DWORD GameLogic::IsBetterThanEquip(ItemData *itemData, DWORD *outItemIndex/* = nullptr*/)
{
	DWORD i, equipType, dwIndex = NOINDEX, anotherEquipType = NOINDEX;
	bool isBetter = false;

	equipType = GetEquipType(itemData->path);
	if (equipType == Weapon1)
		anotherEquipType = Offhand1;
	else if (equipType == Ring1)
		anotherEquipType = Ring2;

	if (equipType >= _controller->playerInventories.size())
		return NOINDEX;

	if (!CanEquipItem(itemData))
		return NOINDEX;

	// INT 요구가 없는 아이템은 제외한다
	if (itemData->attribute.intelligence == 0 && (itemData->attribute.strength > 0 || itemData->attribute.dexterity > 0))
		return NOINDEX;

	// 무기인 경우 한손 무기만 장착한다
	if (equipType == Weapon1)
	{
		if (itemData->path.find(L"/OneHandWeapons/") == -1)
			return NOINDEX;
		if (_serverData->playerClass == Witch)
		{
			if (itemData->path.find(L"/Wands/") == -1)
				return NOINDEX;
		}
	}
	
	// 장착슬롯이 비여있는가를 확인한다
	if (equipType == Flask1)
	{
		if (_controller->playerInventories[equipType].inventory.slotItems.size() < FLASK_SLOT_COUNT)
		{
			if (outItemIndex)
				*outItemIndex = (DWORD)_controller->playerInventories[equipType].inventory.slotItems.size();
			return  equipType;
		}
	}
	else
	{
		if (_controller->playerInventories[equipType].inventory.slotItems.size() == 0)
		{
			if (outItemIndex)
				*outItemIndex = 0;
			return equipType;
		}
		else if (anotherEquipType != NOINDEX)
		{
			if (_controller->playerInventories[anotherEquipType].inventory.slotItems.size() == 0)
			{
				if (outItemIndex)
					*outItemIndex = 0;
				return anotherEquipType;;
			}
		}
	}

	// 물병인 경우 
	if (equipType == Flask1)
	{
		const int FlaskTypeCount = 3;
		DWORD flasksCount[FlaskTypeCount];
		DWORD flasksMaxCount[FlaskTypeCount] = { 2, 2, 1 };
		WCHAR *flasksNameType[FlaskTypeCount] = { L"Life", L"Mana", L"Quicksilver" };
		if (_localPlayer->player.level >= 25)		// 25렙 이상부터 
		{
			flasksMaxCount[0] = 3;
			flasksMaxCount[1] = 1;
		}

		// 물병 개수 계산
		memset(flasksCount, 0, sizeof(flasksCount));
		for (i = 0; i < _controller->playerInventories[equipType].inventory.slotItems.size(); i++)
		{
			ItemData *flaskItem = &_controller->playerInventories[equipType].inventory.slotItems[i].item;
			for (DWORD j = 0; j < FlaskTypeCount; j++)
			{
				if (flaskItem->displayName.find(flasksNameType[j]) != -1)
				{
					flasksCount[j]++;
					break;
				}
			}
		}

		// 필요한 물병을 장착할 위치를 검색
		dwIndex = NOINDEX;
		for (DWORD j = 0; j < FlaskTypeCount; j++)
		{
			if (itemData->displayName.find(flasksNameType[j]) == -1)
				continue;
			if (flasksCount[j] < flasksMaxCount[j])
			{
				for (i = 0; i < _controller->playerInventories[equipType].inventory.slotItems.size(); i++)
				{
					ItemData *flaskItem = &_controller->playerInventories[equipType].inventory.slotItems[i].item;
					if (flaskItem->path.find(flasksNameType[j]) != -1)
						continue;

					// 다른 물병중에서 목표개수보다 넘는 개수의 물병을 선택한다
					DWORD k;
					for (k = 0; k < FlaskTypeCount; k++)
					{
						if (k == j)
							continue;
						if (flaskItem->path.find(flasksNameType[k]) != -1)
							break;
					}
					if (k < FlaskTypeCount && flasksCount[k] <= flasksMaxCount[k])
						continue;

					if (dwIndex == NOINDEX || CompareItem(&_controller->playerInventories[equipType].inventory.slotItems[dwIndex].item, itemData) > 0)
					{
						dwIndex = i;
					}
				}
			}
			

			if (dwIndex != NOINDEX)
			{
				if (outItemIndex)
					*outItemIndex = dwIndex;
				return equipType;
			}
		}
	}

	// 장착된 아이템과 레벨을 비교한다
	dwIndex = NOINDEX;
	for (i = 0; i < _controller->playerInventories[equipType].inventory.slotItems.size(); i++)
	{
		if (CompareItem(itemData, &_controller->playerInventories[equipType].inventory.slotItems[i].item) > 0)
		{
			dwIndex = i;
			break;
		}
	}

	// 장착할수 있는 다른 슬롯도 비교한다
	if (dwIndex == NOINDEX)
	{
		if (anotherEquipType != NOINDEX)
		{
			dwIndex = NOINDEX;
			for (i = 0; i < _controller->playerInventories[anotherEquipType].inventory.slotItems.size(); i++)
			{
				if (CompareItem(itemData, &_controller->playerInventories[anotherEquipType].inventory.slotItems[i].item) > 0)
				{
					dwIndex = i;
					if (outItemIndex)
						*outItemIndex = dwIndex;
					return anotherEquipType;
				}
			}
		}
	}
	

	if (dwIndex != NOINDEX)
	{
		if (outItemIndex)
			*outItemIndex = dwIndex;
		return equipType;
	}
	
	return NOINDEX;
}

BYTE GameLogic::IsGoodItemToPick(ItemData *itemData)
{
	if (itemData->path.find(L"/QuestItems/") != -1)	// 화페 아이템
		return 2;
	if (itemData->path.find(L"/Currency/") != -1)	// 화페 아이템
		return 1;
	if (itemData->path.find(L"/Gems/") != -1)	// 보석 아이템
	{
		if (itemData->quality.ItemQuality > 0 || GetGemInfoByName(itemData->displayName))
			return 1;
	}
	if (itemData->path.find(L"Cards/") != -1)	// 카드 아이템
		return 1;
	if (itemData->path.find(L"/Jewels/") != -1)	// 카드 아이템
		return 1;
	if (itemData->path.find(L"/Flasks/") != -1)	// 물병 아이템
	{
		if (itemData->quality.ItemQuality > 0)
			return 1;
	}
	if (IsBetterThanEquip(itemData) != NOINDEX)		// 더 좋은 물병
		return 1;
	if (itemData->mods.nItemRarity >= 2)		// 노란 아이템
		return 1;
	if (itemData->itemSocket.lstItemSocketInfo.size() >= 6)		// 소켓이 6개 이상 있는 경우
		return 1;
	// 빨,풀, 파 세 색이 다 포함된 경우
	if (itemData->itemWidth * itemData->itemHeight <= 4)
	{
		DWORD redCount = 0, greenCount = 0, blueCount = 0;
		for (DWORD i = 0; i < itemData->itemSocket.lstSocketLinkInfo.size(); i++)
		{
			redCount = 0;
			greenCount = 0;
			blueCount = 0;
			for (DWORD j = 0; j < itemData->itemSocket.lstSocketLinkInfo[i].size(); j++)
			{
				int index = itemData->itemSocket.lstSocketLinkInfo[i][j];
				if (index < 0 || index >= itemData->itemSocket.lstItemSocketInfo.size())
					continue;

				switch (itemData->itemSocket.lstItemSocketInfo[index].SocketColor)
				{
				case SC_Red:
				{
					redCount++;
				}
				break;
				case SC_Green:
				{
					greenCount++;
				}
				break;
				case SC_Blue:
				{
					blueCount++;
				}
				break;
				}

				if (redCount > 0 && greenCount > 0 && blueCount > 0)
					return 1;
			}
		}
	}
	

	return 0;
}

bool GameLogic::CanUpgradeGem(ItemData *itemData)
{
	if (itemData->skillGem.CurrentGemExp + 1 != itemData->skillGem.MaxGemExp)
		return false;
	for (DWORD i = 0; i < GEMINFO_COUNT; i++)
	{
		if (itemData->displayName == g_pGemInfoList[i].gemName)
		{
			if (itemData->skillGem.GemLevel >= g_pGemInfoList[i].gemLeveldetailInfos.size())
				continue;
			if (_localPlayer->player.level < g_pGemInfoList[i].gemLeveldetailInfos[itemData->skillGem.GemLevel].playerLv)
				continue;
			if (_localPlayer->player.intelligence < g_pGemInfoList[i].gemLeveldetailInfos[itemData->skillGem.GemLevel].intelligence)
				continue;
			if (_localPlayer->player.strength < g_pGemInfoList[i].gemLeveldetailInfos[itemData->skillGem.GemLevel].strength)
				continue;
			if (_localPlayer->player.dexterity < g_pGemInfoList[i].gemLeveldetailInfos[itemData->skillGem.GemLevel].dexterity)
				continue;
			return true;
		}
	}

	return false;
}

GemInfo * GameLogic::GetGemInfoByName(std::wstring gemName)
{
	for (int i = 0; i < g_pInfoGem.size(); i++)
	{
		if (gemName == g_pInfoGem[i].szName)
			return &g_pInfoGem[i];
	}
	return nullptr;
}

DWORD GameLogic::GetItemIndexWithEmptySocket(int socketColor, DWORD *outItemIndex, DWORD *outSockeIndex)
{
	for (DWORD j = MainInventory1 + 1; j < _controller->playerInventories.size(); j++)
	{
		InventoryHolder *pInven = &_controller->playerInventories[j];
		for (DWORD i = 0; i < pInven->inventory.slotItems.size(); i++)
		{
			ItemData *pItem = &pInven->inventory.slotItems[i].item;
			for (int k = 0; k < pItem->itemSocket.lstItemSocketInfo.size(); k++)
			{
				if (!pItem->itemSocket.lstItemSocketInfo[k].IsSocketEquiped && pItem->itemSocket.lstItemSocketInfo[k].SocketColor == socketColor)
				{
					if (outSockeIndex)
						*outSockeIndex = k;
					if (outItemIndex)
						*outItemIndex = i;
					return j;
				}
			}
		}
	}

	return NOINDEX;
}

BYTE GameLogic::HasItemToSell(ItemData *itemData)
{
	if (itemData->path.find(L"/QuestItems/") != -1)
		return 0;
	if (itemData->path.find(L"/Currency/") != -1)
		return 0;
	if (itemData->path.find(L"/Gems/") != -1)
	{
		//if (itemData->quality.ItemQuality > 0 || GetGemInfoByName(itemData->displayName))
			return 0;
	}
	if (itemData->path.find(L"Cards/") != -1)
		return 0;
	if (itemData->path.find(L"/Jewels/") != -1)	// 카드 아이템
		return 0;
	if (itemData->path.find(L"/Flasks/") != -1)	// 물병 아이템
	{
		if (itemData->quality.ItemQuality > 0)
			return 0;
	}
	// 	if (IsBetterThanEquip(itemData) != NOINDEX)
	// 	{
	// 		return 0;
	// 	}
	return 1;
}

DWORD GameLogic::GetItemIndexToSell()
{
	DWORD dwIndex = NOINDEX;
	DWORD i = 0;

	auto pInven = &_controller->playerInventories[MainInventory1];
	for (i = 0; i < pInven->inventory.slotItems.size(); i++)
	{
		auto *pItem = &pInven->inventory.slotItems[i];
		if (HasItemToSell(&pItem->item))
		{
			dwIndex = i;
		}
	}

	return dwIndex;
}

DWORD GameLogic::GetPurchaseItemIndex(std::wstring npcName)
{
	DWORD dwItemCount = 0;
	DWORD dwNeedItemCount = 0;
	DWORD dwResult = NOINDEX;
	DWORD i = 0, j = 0, k = 0;

	for (i = 0; i < g_pInfoPurchaseItem.size(); i++)
	{
		// 조건 퀘스트를 만료하였는지 확인한다
		if (g_pInfoPurchaseItem[i].completeQuestName && g_pInfoPurchaseItem[i].completeQuestName[0])
		{
			for (j = 0; j < _ingameUi->questData.quests.size(); j++)
			{
				auto pQuest = &_ingameUi->questData.quests[j];
				if (pQuest->name == g_pInfoPurchaseItem[i].completeQuestName && pQuest->isCompleted)
					break;
			}
			if (j >= _ingameUi->questData.quests.size())
				continue;
		}

		// 해당 NPC한테서 구매할수있는 아이템인지 확인한다
		if (npcName != g_pInfoPurchaseItem[i].npcName)
			continue;

		for (j = 0; j < _controller->playerInventories.size(); j++)
		{
			dwItemCount = GetItemCountByType(g_pInfoPurchaseItem[i].purchaseItemName, j, 1);
			if (dwItemCount)
			{
				break;
			}
			dwNeedItemCount += GetItemCountByType(g_pInfoPurchaseItem[i].needItemName, j, 0);
		}

		if (dwItemCount)
		{
			continue;
		}

		if (dwNeedItemCount < g_pInfoPurchaseItem[i].needItemCount)
		{
			continue;
		}

		dwResult = i;
		return dwResult;
	}
	return dwResult;
}


DWORD GameLogic::GetItemCountByType(std::wstring ItemName, DWORD dwInvenType, BYTE itemType)
{
	DWORD dwCount = 0;
	DWORD i = 0, j = 0;

	auto pInven = &_controller->playerInventories[dwInvenType];
	for (i = 0; i < pInven->inventory.slotItems.size(); i++)
	{
		auto *pItem = &pInven->inventory.slotItems[i];
		if (ItemName == pItem->item.displayName)
		{
			if (!itemType)
			{
				dwCount = dwCount + pItem->item.stack.Size;
			}
			else
			{
				dwCount++;
			}
		}
	}
	if (itemType == 1)
	{
		for (i = 0; i < pInven->inventory.slotItems.size(); i++)
		{
			auto *pItem = &pInven->inventory.slotItems[i];

			for (j = 0; j < pItem->item.itemSocket.lstItemSocketInfo.size(); j++)
			{
				if (ItemName == pItem->item.itemSocket.lstItemSocketInfo[j].SocketedName)
				{
					dwCount = 1;
					return dwCount;
				}
			}
		}
	}
	return dwCount;
}

DWORD GameLogic::GetItemIndexToStore(BYTE bItemType)
{
	DWORD i = 0;
	DWORD wisdomIndex = NOINDEX, portalIndex = NOINDEX, wisdomCount = 0, portalCount = 0;
	int wisdomMinStack = 100000, portalMinStack = 100000;

	auto pInven = &_controller->playerInventories[MainInventory1];

	if (bItemType == 0)
	{
		for (i = 0; i < pInven->inventory.slotItems.size(); i++)
		{
			auto *pItem = &pInven->inventory.slotItems[i];
			if (pItem->item.path.find(L"/Currency/") != -1)
			{
				if (pItem->item.displayName == ITEM_WISDOM)
				{
					if (pItem->item.stack.Size < wisdomMinStack)
					{
						wisdomIndex = i;
						wisdomMinStack = pItem->item.stack.Size;
					}
					wisdomCount++;
				}
				else if (pItem->item.displayName == ITEM_PORTAL)
				{
					if (pItem->item.stack.Size < portalMinStack)
					{
						portalIndex = i;
						portalMinStack = pItem->item.stack.Size;
					}
					portalCount++;
				}
				else
				{
					return i;
				}
			}
		}

		if (wisdomCount > 1)
			return wisdomIndex;
		if (portalCount > 1)
			return portalIndex;
	}
	else if (bItemType == 1)
	{
		for (i = 0; i < pInven->inventory.slotItems.size(); i++)
		{
			auto *pItem = &pInven->inventory.slotItems[i];
			if (pItem->item.path.find(L"/Gems/") != -1)
			{
				if (GetGemInfoByName(pItem->item.displayName) == nullptr)
					return i;
			}
			if (pItem->item.path.find(L"/Jewels/") != -1)
			{
				return i;
			}
		}
	}
	else if (bItemType == 2)
	{
		for (i = 0; i < pInven->inventory.slotItems.size(); i++)
		{
			auto *pItem = &pInven->inventory.slotItems[i];
			if (pItem->item.path.find(L"/Flasks/") != -1)
			{
				return i;
			}
		}
	}

	return NOINDEX;
}

// ========================= Basic Function ===========================

void GameLogic::MoveToPos(IVector pos)
{
	DoPlayerAction(_serverData->Address, 0x2909, 0, (int *)&pos, 0x0808);
}

void GameLogic::SelectEntity(DWORD entityId)
{
	DoPlayerAction(_serverData->Address, 0x0266, (DWORD64)entityId, nullptr, 0x0808, false);
}

void GameLogic::UseSkill(WORD skillId, DWORD targetId)
{
	DoPlayerAction(_serverData->Address, skillId, (DWORD64)targetId, nullptr, 0x0808, false);
}

void GameLogic::UseSkill(WORD skillId, IVector pos)
{
	DoPlayerAction(_serverData->Address, skillId, 0, (int *)&pos, 0x0808, false);
}

void GameLogic::MoveItem(DWORD dstInvenIndex, int cellX, int cellY)
{
	ReleaseItem(_serverData->Address, _controller->playerInventories[dstInvenIndex].id, cellX, cellY);
}

void GameLogic::MoveItemToCursor(DWORD invenIndex, DWORD itemIndex)
{
	HoldItem(_serverData->Address, _controller->playerInventories[invenIndex].id, _controller->playerInventories[invenIndex].inventory.slotItems[itemIndex].itemKey);
}

void GameLogic::EquipGem(DWORD dstInvenIndex, DWORD dstItemIndex, DWORD socketIndex)
{
	EquipGemToItem(_serverData->Address, _controller->playerInventories[dstInvenIndex].id, _controller->playerInventories[dstInvenIndex].inventory.slotItems[dstItemIndex].itemKey, socketIndex);
}

void GameLogic::UnequipGem(DWORD invenIndex, DWORD itemIndex, DWORD socketIndex)
{
	DisarmingGem(_serverData->Address, _controller->playerInventories[invenIndex].id, _controller->playerInventories[invenIndex].inventory.slotItems[itemIndex].itemKey, socketIndex);
}

void GameLogic::LevelUpGem(DWORD invenIndex, DWORD itemIndex, DWORD socketIndex)
{
	GemLevelUp(_serverData->Address, _controller->playerInventories[invenIndex].id, _controller->playerInventories[invenIndex].inventory.slotItems[itemIndex].itemKey, socketIndex);
}

void GameLogic::UseOrb(DWORD invenOrb, DWORD orbIndex, DWORD invenItem, DWORD dwItemIndex)
{
	InventoryHolder *pInvenOrb = &_controller->playerInventories[invenOrb];
	InventoryHolder *pInvenItem = &_controller->playerInventories[invenItem];
	UsingOrbToItem(_serverData->Address, pInvenOrb->id, pInvenOrb->inventory.slotItems[orbIndex].itemKey, pInvenItem->id, pInvenItem->inventory.slotItems[dwItemIndex].itemKey);
}

void GameLogic::Resurrect(BYTE type/* = 0*/)
{
	ResurrectAtCheckPoint(_serverData->Address);
}

void GameLogic::ReceiveReward(DWORD npcItemIndex, DWORD dstInvenIndex, int dstCellX, int dstCellY, BYTE npcInvenIndex/* = 0*/)
{
	RecvReward(_serverData->Address, _controller->npcInventories[npcInvenIndex].inventory.slotItems[npcItemIndex].itemKey, _controller->playerInventories[dstInvenIndex].id, dstCellX, dstCellY, npcInvenIndex);
}

void GameLogic::NpcElementClick(BYTE bNpcElementId)
{
	ClickNpcElement(_serverData->Address, bNpcElementId);
}

void GameLogic::TradeAccept(DWORD dwLastItemKey)
{
	SellItem(_serverData->Address, dwLastItemKey);
}

void GameLogic::MoveTradeItem(DWORD dwInvenIndex, DWORD dwItemKey, BYTE bTargetCellX, BYTE bTargetCellY)
{
	ReleaseTradeItem(_serverData->Address, dwInvenIndex, dwItemKey, bTargetCellX, bTargetCellY);
}

void GameLogic::GetNextStashInfo(DWORD stashIndex)
{
	NextStash(_serverData->Address, stashIndex);
}

// =================== Pathfind utils =========================
IVector GameLogic::ConvertPointsToBlock(IVector pos)
{
	IVector cpos;

	cpos.x = pos.x / MAP_BLOCK_POINTS;
	cpos.y = pos.y / MAP_BLOCK_POINTS;

	return cpos;
}

IVector GameLogic::ConvertPointsFromBlock(IVector cpos)
{
	IVector pos;

	pos.x = cpos.x * MAP_BLOCK_POINTS;
	pos.y = cpos.y * MAP_BLOCK_POINTS;

	return pos;
}

bool GameLogic::IsConnectedPoints(IVector pos1, IVector pos2)
{
	IVector cPos1, cPos2;

	cPos1 = ConvertPointsToBlock(pos1);
	cPos2 = ConvertPointsToBlock(pos2);

	return IsConnected(_map.blockBuffer, _map.mapWidth, _map.mapHeight, 255, cPos1, cPos2);
}

void GameLogic::FindFullPath()
{
	IVector startPos;
	IVector *points;
	BYTE *blockBuffer;
	int pointsNum, mapSize;

	if (!_map.blockBuffer || !_fullPath)
		return;

	mapSize = _map.mapWidth * _map.mapHeight;
	points = new IVector[4 * mapSize];
	blockBuffer = new BYTE[mapSize];

	memcpy(blockBuffer, _map.blockBuffer, mapSize);

	startPos = ConvertPointsToBlock(_localPlayer->positionedComp.gridPos);
	// 맵경계 처리
	for (int i = 0; i < mapSize; i++)
	{
		if (blockBuffer[i])
			blockBuffer[i] = 255;
	}
	blockBuffer[startPos.x + startPos.y * _map.mapWidth] = 0;

	DetectConnectedArea(blockBuffer, blockBuffer, startPos.x, startPos.y, _map.mapWidth, _map.mapHeight, 255);

	if (_worldArea->name == AREA_MUD_FLATS)		// 이 지역에서 Breaking Some Eggs 퀘스트가 완료되지 않았으면 경로탐색을 넒게 진행한다
	{
		for (DWORD j = 0; j < _questData->quests.size(); j++)
		{
			if (_questData->quests[j].key == 3 )
			{
				if ((_questData->quests[j].step > 1 || _questData->quests[j].step == -1))
				{
					Log(L"Special The Mud Flats");
					_map.findPathStep = 30;
				}
				break;
			}
		}
	}
	CalcScanPath2(blockBuffer, startPos.x, startPos.y, _map.mapWidth, _map.mapHeight, 255, _map.findPathStep, _map.findPathStep, &_pointsScanSize, _pointsScan);
	Log(L"Scan points(%d) = %d", _map.findPathStep, _pointsScanSize);

	_fullPathSize = 0;
	for (int i = 0; i < _pointsScanSize; i++)
	{
		// 시작점, 끝점에 대한 거친경로얻기 
		CalcRawPath(blockBuffer, startPos.x, startPos.y, _pointsScan[i].x, _pointsScan[i].y, _map.mapWidth, _map.mapHeight, 255, &pointsNum, points);

		// 거친경로 교정하기
		CalcRefinePath(blockBuffer, _map.mapWidth, _map.mapHeight, 255, 20, &pointsNum, points);

		startPos = _pointsScan[i];

		for (int j = 0; j < pointsNum; j++)
		{
			_fullPath[_fullPathSize] = ConvertPointsFromBlock(points[j]);
			//Log(L"Path %d, %d", _fullPath[_fullPathSize].x, _fullPath[_fullPathSize].y);
			_fullPathSize++;
		}
	}

	Log(L"Full Path  = %d", _fullPathSize);

	delete[] points;
	delete[] blockBuffer;
}

bool GameLogic::FindTargetPath(IVector posTarget)
{
	IVector startPos, endPos;
	IVector *points;
	IVector pos, nearStart, nearEnd;
	BYTE *blockBuffer;
	int pointsNum, mapSize;
	float dist, distMinStart = 100000000.f, distMinEnd = 100000000.f;

	if (!_map.blockBuffer || !_targetPath)
		return false;

	mapSize = _map.mapWidth * _map.mapHeight;
	points = new IVector[4 * mapSize];
	blockBuffer = new BYTE[mapSize];

	memcpy(blockBuffer, _map.blockBuffer, mapSize);

	startPos = ConvertPointsToBlock(_localPlayer->positionedComp.gridPos);
	endPos = ConvertPointsToBlock(posTarget);
	TESTLOG(L"Postion (%d, %d) (%d, %d)", startPos.x, startPos.y, endPos.x, endPos.y);

	// 맵경계 처리
	for (int i = 0; i < mapSize; i++)
	{
		if (blockBuffer[i])
			blockBuffer[i] = 255;
	}

	// 갈수있는 가장 가까운 점을 찾는다
	for (int i = 0; i < _map.mapWidth; i++)
	{
		for (int j = _map.mapHeight - 1; j >= 0; j--)
		{
			pos = { i, j };
			if (!blockBuffer[i + j * _map.mapWidth])
			{
				dist = DIST(pos, startPos);
				if (dist < distMinStart)
				{
					distMinStart = dist;
					nearStart = pos;
				}

				dist = DIST(pos, endPos);
				if (dist < distMinEnd)
				{
					distMinEnd = dist;
					nearEnd = pos;
				}
			}
		}
	}
	startPos = nearStart;
	endPos = nearEnd;

	TESTLOG(L"Near Postion (%d, %d) (%d, %d)", startPos.x, startPos.y, endPos.x, endPos.y);

	// 시작점, 끝점에 대한 거친경로얻기 
	if (_worldArea->name == AREA_CHAMBER_LV1)
		CalcRawPath(blockBuffer, startPos.x, startPos.y, endPos.x, endPos.y, _map.mapWidth, _map.mapHeight, 255, &pointsNum, points, true);
	else
		CalcRawPath(blockBuffer, startPos.x, startPos.y, endPos.x, endPos.y, _map.mapWidth, _map.mapHeight, 255, &pointsNum, points, false);
	//Log(L"Raw points = %d", pointsNum);
	CalcRefinePath(blockBuffer, _map.mapWidth, _map.mapHeight, 255, 20, &pointsNum, points);

	_targetPathSize = 0;
	for (int i = 0; i < pointsNum; i++)
	{
		_targetPath[_targetPathSize] = ConvertPointsFromBlock(points[i]);
		//Log(L"Path %d, %d", _targetPath[_targetPathSize].x, _targetPath[_targetPathSize].y);
		_targetPathSize++;
	}

	delete[] points;
	delete[] blockBuffer;

	if (_targetPathSize == 0)
		return false;

	dist = DIST(_targetPath[_targetPathSize - 1], posTarget);
	TESTLOG(L"Target Path  = %d, LastPoint = %.1f", _targetPathSize, dist);
	if (dist > 10.f)
		return false;
	return true;
}

std::vector<int> GameLogic::FindAreaPath(std::wstring startArea, std::wstring endArea)
{
	std::vector<AreaNode> areas;
	std::vector<int> areaKeys;
	DWORD i, j, k, startIndex = NOINDEX, endIndex = NOINDEX;

	for (i = 0; i < WORLDAREA_RES_COUNT; i++)
	{
		AreaNode area;
		area.name = g_pResWorldArea[i].szName;
		area.key = i;

		if (startIndex == NOINDEX && area.name == startArea)
			startIndex = i;
		if (endIndex == NOINDEX && area.name == endArea)
			endIndex = i;
		areas.push_back(area);
	}

	if (startIndex == NOINDEX || endIndex == NOINDEX)		// Area error
		return areaKeys;

	for (i = 0; i < WORLDAREA_RES_COUNT; i++)
	{
		for (j = 0; j < g_pResWorldArea[i].dwNumConnections; j++)
		{
			if (g_pResWorldArea[i].pdwConnectionsList[j] < areas.size())
			{
				areas[i].addChild(&areas[g_pResWorldArea[i].pdwConnectionsList[j]], 1.0f);
			}
		}

		// 거점 있는 지역 추가
		if (std::find(_ingameUi->teleportFieldIds.begin(), _ingameUi->teleportFieldIds.end(), g_pResWorldArea[i].dwID) != _ingameUi->teleportFieldIds.end())
		{
			for (j = 0; j < WORLDAREA_RES_COUNT; j++)
			{
				if (i == j)
					continue;
				if (std::find(_ingameUi->teleportFieldIds.begin(), _ingameUi->teleportFieldIds.end(), g_pResWorldArea[j].dwID) == _ingameUi->teleportFieldIds.end())
					continue;
				for (k = 0; k < g_pResWorldArea[i].dwNumConnections; k++)
				{
					if (j == g_pResWorldArea[i].pdwConnectionsList[k])
						break;
				}
				if (k < g_pResWorldArea[i].dwNumConnections)
					continue;

				areas[i].addChild(&areas[j], 1.1f);
			}
		}
	}

	// Create the PathFinder and PathAlgorithm stuff
	PathFinder<AreaNode> p;
	std::vector<AreaNode*> solution;

	// We'll assume that the user is giving correct arguments, because we deny all responsibilities !
	p.setStart(areas[startIndex]);
	p.setGoal(areas[endIndex]);

	bool r = p.findPath<Dijkstra>(solution);
	if (r) // path found
	{
		for (const auto& area : solution)
			areaKeys.push_back(area->key);
	}
	Dijkstra::getInstance().clear();

	return areaKeys;
}

DWORD GameLogic::GetGlobalAreaIndexByName(std::wstring areaName)
{
	for (DWORD i = 0; i < _setting.globalAreas.size(); i++)
	{
		if (areaName == g_pResWorldArea[_setting.globalAreas[i]].szName)
			return i;
	}
	return NOINDEX;
}

// ========================== Logic Handler ===========================

void GameLogic::ReadMapBlock()
{
	uintptr_t mapInfoPtr = *(uintptr_t *)(_ingameState->Address + DATA_OFFSET_INGAMESTATE_MAP);
	if (!mapInfoPtr)
		return;
	uintptr_t addr = *(uintptr_t *)mapInfoPtr;
	if (!addr)
		return;

	addr += DATA_OFFSET_MAP_BLOCK;

	int curMapWidth = *(int *)(addr + 0x08);
	int curMapHeight = *(int *)(addr + 0x0C);
	if (_map.mapWidth == curMapWidth && _map.mapHeight == curMapHeight && _map.areaName == _worldArea->name)
		return;

	_map.Init();
	_map.mapWidth = curMapWidth;
	_map.mapHeight = curMapHeight;
	_map.areaName = _worldArea->name;

	int mapSize = curMapWidth * curMapHeight;
	_map.blockBuffer = new BYTE[mapSize];
	memcpy(_map.blockBuffer, *(void **)addr, mapSize);
	// 맵경계 처리
	for (int i = 0; i < mapSize; i++)
	{
		if (_map.blockBuffer[i])
			_map.blockBuffer[i] = 255;
	}

	Log(L"Read map block [%d, %d]", curMapWidth, curMapHeight);
	_setting.newMapBlock = 1;

	if (_fullPath)
		delete[] _fullPath;
	if (_targetPath)
		delete[] _targetPath;
	if (_pointsScan)
		delete[] _pointsScan;
	
	_fullPath = new IVector[mapSize];
	_fullPathSize = 0;
	_targetPath = new IVector[mapSize];
	_targetPathSize = 0;
	_pointsScan = new IVector[mapSize];
	_pointsScanSize = 0;
}

void GameLogic::LoadFullPath()
{
	FindFullPath();
	_currentPath = _fullPath;
	_currentPathSize = _fullPathSize;
	_moveStep = NOINDEX;
}

bool GameLogic::LoadTargetPath(IVector posTarget)
{
	if (FindTargetPath(posTarget))
	{
		_currentPath = _targetPath;
		_currentPathSize = _targetPathSize;
		_moveStep = NOINDEX;
		return true;
	}
	return false;
}

BYTE GameLogic::UsePortion()
{
	DWORD i;

	if (_worldArea->isTown)
		return 0;

	if (_localPlayer->life.hpPercentage < 0.9f && GetMyBuffIndex(L"flask_effect_life") == NOINDEX)
	{
		InventoryHolder *pInven = &_controller->playerInventories[Flask1];
		for (i = 0; i < pInven->inventory.slotItems.size(); i++)
		{
			InvenSlotItem *pItem = &pInven->inventory.slotItems[i];
			if (pItem->item.displayName.find(L"Life") != -1 && pItem->item.charges.CurrentCharges >= pItem->item.charges.ChargesPerUse)
			{
				TESTLOG(L"HP-Portion [%s] %d %d",
					pItem->item.displayName.c_str(),
					pItem->location.xMin,
					pItem->item.charges.CurrentCharges);
				PressKeyAlternate(L'1' + pItem->location.xMin);
				SetDelay(5);
				return 1;
			}
		}
	}

	if (_localPlayer->life.mpPercentage < 0.7f && GetMyBuffIndex(L"flask_effect_mana") == NOINDEX)
	{
		InventoryHolder *pInven = &_controller->playerInventories[Flask1];
		for (i = 0; i < _controller->playerInventories[Flask1].inventory.slotItems.size(); i++)
		{
			InvenSlotItem *pItem = &pInven->inventory.slotItems[i];
			if (pItem->item.displayName.find(L"Mana") != -1 && pItem->item.charges.CurrentCharges >= pItem->item.charges.ChargesPerUse)
			{
				TESTLOG(L"MP-Portion [%s] %d %d",
					pItem->item.displayName.c_str(),
					pItem->location.xMin,
					pItem->item.charges.CurrentCharges);
				PressKeyAlternate(L'1' + pItem->location.xMin);
				SetDelay(5);
				return 1;
			}
		}
	}

	if (GetMyBuffIndex(L"flask_utility_sprint") == NOINDEX && _localPlayer->actor.isMoving)
	{
		InventoryHolder *pInven = &_controller->playerInventories[Flask1];
		for (i = 0; i < _controller->playerInventories[Flask1].inventory.slotItems.size(); i++)
		{
			InvenSlotItem *pItem = &pInven->inventory.slotItems[i];
			if (pItem->item.displayName.find(L"Quicksilver") != -1 && pItem->item.charges.CurrentCharges >= pItem->item.charges.ChargesPerUse)
			{
				TESTLOG(L"Silver-Portion [%s] %d %d",
					pItem->item.displayName.c_str(),
					pItem->location.xMin,
					pItem->item.charges.CurrentCharges);
				PressKeyAlternate(L'1' + pItem->location.xMin);
				SetDelay(5);
				return 1;
			}
		}
	}

	return 0;
}

bool GameLogic::AutoEquip()
{
	if (AutoEquipItem())
		return true;
	if (AutoEquipGem())
		return true;
	if (AutoUpgradeGem())
		return true;

	return false;
}

bool GameLogic::AutoEquipItem()
{
	InventoryHolder *pInven = &_controller->playerInventories[Cursor1];

	if (pInven->inventory.slotItems.size() > 0)
	{
		ItemData *pItem = &pInven->inventory.slotItems[0].item;
		if (pItem->path.find(L"/Gems/") != -1)		// 보석인 경우 스킵
			return false;
		if (_setting.holdType != 0)
			return false;
		if (_setting.toEquipInvenIndex < Inventory_Max && _setting.toEquipItemName == pItem->displayName)
		{
			TESTLOG(L"Equip Item [%s], To %d (%d, %d)", pItem->displayName.c_str(), _setting.toEquipInvenIndex, _setting.toEquipCellX, _setting.toEquipCellY);
			MoveItem(_setting.toEquipInvenIndex, _setting.toEquipCellX, _setting.toEquipCellY);
		}
		else
		{
			int emptyX = 0, emptyY = 0;
			if (!GetEmptyCellByItemData(pItem, false, &emptyX, &emptyY))
			{
				Log(L"Cursor item error! [%s], [%s], %d", pItem->displayName.c_str(), _setting.toEquipItemName.c_str(), _setting.toEquipInvenIndex);
				ToggleRun(0);
				return true;
			}

			TESTLOG(L"Move from cursor1 to main inventory [%s] (%d, %d)", pItem->displayName.c_str(), emptyX, emptyY);
			MoveItem(MainInventory1, emptyX, emptyY);
		}

		_setting.InitEquip();
		SetDelay(5);
		return true;
	}

	pInven = &_controller->playerInventories[MainInventory1];
	for (DWORD i = 0; i < pInven->inventory.slotItems.size(); i++)
	{
		ItemData *pItem = &pInven->inventory.slotItems[i].item;
		DWORD changeEquipIndex = NOINDEX;
		DWORD dstInvenIndex = IsBetterThanEquip(pItem, &changeEquipIndex);
		if (dstInvenIndex != NOINDEX)
		{
			if (!pItem->mods.bIdentified)
			{
				DWORD dwScrollIndex = GetItemIndexByName(MainInventory1, ITEM_WISDOM);
				if (dwScrollIndex == NOINDEX)		// Scroll 이 없으면 스킵
					continue;

				TESTLOG(L"Use orb [%s]->[%s]", pInven->inventory.slotItems[dwScrollIndex].item.displayName.c_str(), pItem->displayName.c_str());
				UseOrb(MainInventory1, dwScrollIndex, MainInventory1, i);
				SetDelay(5);
				return true;
			}
			_setting.toEquipInvenIndex = dstInvenIndex;
			_setting.toEquipItemName = pItem->displayName;
			_setting.toEquipCellX = 0;
			_setting.toEquipCellY = 0;
			_setting.holdType = 0;
			if (dstInvenIndex == Flask1)
			{
				InventoryHolder *pFlaskInven = &_controller->playerInventories[dstInvenIndex];
				if (changeEquipIndex < pFlaskInven->inventory.slotItems.size())	// 이미 있는 물병인 경우 해당 물병 슬롯
				{
					_setting.toEquipCellX = pFlaskInven->inventory.slotItems[changeEquipIndex].location.xMin;
					_setting.toEquipCellY = pFlaskInven->inventory.slotItems[changeEquipIndex].location.yMin;
				}
				else		// 빈 슬롯을 찾는다
				{
					DWORD j, k;
					for (j = 0; j < FLASK_SLOT_COUNT; j++)
					{
						for (k = 0; k < pFlaskInven->inventory.slotItems.size(); k++)
						{
							if (j == pFlaskInven->inventory.slotItems[k].location.xMin)
								break;
						}
						if (k >= pFlaskInven->inventory.slotItems.size())
							break;
					}
					if (j >= FLASK_SLOT_COUNT)		// 빈 슬롯을 찾을수 없음 - Error
						continue;

					_setting.toEquipCellX = j;
				}
			}
			TESTLOG(L"Hold item [%s], %d, %d, %d", pItem->displayName.c_str(), dstInvenIndex, _setting.toEquipCellX, _setting.toEquipCellY);
			MoveItemToCursor(MainInventory1, i);
			SetDelay(5);
			return true;
		}
	}
	return false;
}

bool GameLogic::AutoEquipGem()
{
	InventoryHolder *pInven = &_controller->playerInventories[Cursor1];

	if (pInven->inventory.slotItems.size() > 0)
	{
		ItemData *pItem = &pInven->inventory.slotItems[0].item;
		if (pItem->path.find(L"/Gems/") == -1)		// 보석이 아닌 경우 스킵
			return false;
		if (_setting.holdType != 1)
			return false;
		if (_setting.toEquipInvenIndex < Inventory_Max && _setting.toEquipItemName == pItem->displayName)
		{
			TESTLOG(L"Equip Gem [%s], To %d %d %d", pItem->displayName.c_str(), _setting.toEquipInvenIndex, _setting.toEquipCellX, _setting.toEquipCellY);
			EquipGem(_setting.toEquipInvenIndex, _setting.toEquipCellX, _setting.toEquipCellY);
		}
		else
		{
			int emptyX = 0, emptyY = 0;
			if (!GetEmptyCellByItemData(pItem, false, &emptyX, &emptyY))
			{
				Log(L"Cursor item error! [%s], [%s], %d", pItem->displayName.c_str(), _setting.toEquipItemName.c_str(), _setting.toEquipInvenIndex);
				ToggleRun(0);
				return true;
			}

			TESTLOG(L"Move from cursor1 to main inventory [%s] (%d, %d)", pItem->displayName.c_str(), emptyX, emptyY);
			MoveItem(MainInventory1, emptyX, emptyY);
		}

		_setting.InitEquip();
		SetDelay(5);
		return true;
	}

	pInven = &_controller->playerInventories[MainInventory1];
	for (DWORD i = 0; i < pInven->inventory.slotItems.size(); i++)
	{
		ItemData *pItem = &pInven->inventory.slotItems[i].item;
		DWORD invenIndex, itemIndex, socketIndex;
		if (pItem->path.find(L"/Gems/") != -1)		// 보석인 경우
		{
			auto info = GetGemInfoByName(pItem->displayName);
			if (!info)
				continue;

			invenIndex = GetItemIndexWithEmptySocket(info->socketColor, &itemIndex, &socketIndex);
			if (invenIndex == NOINDEX)
				continue;
			
			_setting.toEquipInvenIndex = invenIndex;
			_setting.toEquipItemName = pItem->displayName;
			_setting.toEquipCellX = itemIndex;
			_setting.toEquipCellY = socketIndex;
			_setting.holdType = 1;

			TESTLOG(L"Hold gem [%s], %d, %d, %d", pItem->displayName.c_str(), invenIndex, _setting.toEquipCellX, _setting.toEquipCellY);
			MoveItemToCursor(MainInventory1, i);
			SetDelay(5);
			return true;
		}
		else
		{
			for (int k = 0; k < pItem->itemSocket.lstItemSocketInfo.size(); k++)
			{
				if (pItem->itemSocket.lstItemSocketInfo[k].IsSocketEquiped)
				{
					auto info = GetGemInfoByName(pItem->itemSocket.lstItemSocketInfo[k].SocketedName);
					if (!info)
						continue;

					invenIndex = GetItemIndexWithEmptySocket(info->socketColor, &itemIndex, &socketIndex);

					_setting.toEquipInvenIndex = invenIndex;
					_setting.toEquipItemName = pItem->itemSocket.lstItemSocketInfo[k].SocketedName.c_str();
					_setting.toEquipCellX = itemIndex;
					_setting.toEquipCellY = socketIndex;
					_setting.holdType = 1;

					TESTLOG(L"Unequip gem [%s], %d, %d, %d", pItem->itemSocket.lstItemSocketInfo[k].SocketedName.c_str(), 0, _setting.toEquipCellX, _setting.toEquipCellY);
					UnequipGem(MainInventory1, i, pItem->itemSocket.lstItemSocketInfo[k].SocketIndex);
					SetDelay(5);
					return true;
				}
			}
		}
	}

	return false;
}

bool GameLogic::AutoUpgradeGem()
{
	for (DWORD j = MainInventory1 + 1; j < _controller->playerInventories.size(); j++)
	{
		InventoryHolder *pInven = &_controller->playerInventories[j];
		for (DWORD i = 0; i < pInven->inventory.slotItems.size(); i++)
		{
			ItemData *pItem = &pInven->inventory.slotItems[i].item;
			ItemData socketItem;
			for (int k = 0; k < pItem->itemSocket.lstItemSocketInfo.size(); k++)
			{
				if (pItem->itemSocket.lstItemSocketInfo[k].IsSocketEquiped && socketItem.Update(pItem->itemSocket.lstItemSocketInfo[k].Address))
				{
					if (CanUpgradeGem(&socketItem))
					{
						TESTLOG(L"Level up gem [%s]", pItem->itemSocket.lstItemSocketInfo[k].SocketedName.c_str());
						LevelUpGem(j, i, pItem->itemSocket.lstItemSocketInfo[k].SocketIndex);
						SetDelay(5);
						return true;
					}
				}
			}
		}
	}

	return false;
}

bool GameLogic::UseForwardSkill(IVector posTarget)
{
	DoForwardSkillInfo *pDoSkillInfo = NULL;
	DWORD i, dwIndex = NOINDEX, dwSkillID = NOINDEX;;

	pDoSkillInfo = &g_pForwardSkillList_Witch;

	for (i = 0; i < pDoSkillInfo->generalNum; i++)
	{
		dwSkillID = GetSkillBarIDByName(pDoSkillInfo->pSkillGeneral[i].name);
		if (dwSkillID == NOINDEX)
			continue;
		dwIndex = GetSkillIndexByName(pDoSkillInfo->pSkillGeneral[i].name, (WORD)dwSkillID);
		if (dwIndex != NOINDEX)
		{
			if (IsEnableSkill(dwIndex) && _localPlayer->life.curMana >= pDoSkillInfo->pSkillGeneral[i].manaCost &&
				(_tick - pDoSkillInfo->pSkillGeneral[i].coolTime) >= pDoSkillInfo->pSkillGeneral[i].delay * 100)
			{
				TESTLOG(L"UseForwardSkill [%s]", _localPlayer->actor.actorSkills[dwIndex].name.c_str());
				DoPlayerForwardSkillAction(_serverData->Address, _localPlayer->actor.actorSkills[dwIndex].id, (int *)&posTarget);
				pDoSkillInfo->pSkillGeneral[i].coolTime = _tick;
				SetDelay(3);
				return true;
			}
		}
	}

	return false;
}

bool GameLogic::UseBuffSkill()
{
	if (_worldArea->isTown)
		return false;

	DoSkillInfo *pDoSkillInfo = NULL;
	DWORD i, dwIndex = NOINDEX, dwSkillID = NOINDEX;

	pDoSkillInfo = &g_pSkillList_Witch;

	for (i = 0; i < pDoSkillInfo->generalNum; i++)
	{
		if (pDoSkillInfo->pSkillGeneral[i].isBuff != 1)	//버프스킬이 아니면
			continue;
		if (GetMyBuffIndex(pDoSkillInfo->pSkillGeneral[i].buffName) != NOINDEX)		// 버프 스킬이 사용중이라면
			continue;
		dwSkillID = GetSkillBarIDByName(pDoSkillInfo->pSkillGeneral[i].name);
		if (dwSkillID == NOINDEX)
			continue;
		dwIndex = GetSkillIndexByName(pDoSkillInfo->pSkillGeneral[i].name, (WORD)dwSkillID);
		if (dwIndex != NOINDEX)
		{			
			if (IsEnableSkill(dwIndex) && _localPlayer->life.curMana >= pDoSkillInfo->pSkillGeneral[i].manaCost)
			{
				TESTLOG(L"UseBuffSkill [%s]", _localPlayer->actor.actorSkills[dwIndex].name.c_str());
				DoPlayerBuffSkillAction(_serverData->Address, _localPlayer->actor.actorSkills[dwIndex].id, _localPlayer->id);
				SetDelay(pDoSkillInfo->pSkillGeneral[i].delay);
				return true;
			}
		}
	}

	return false;
}

bool GameLogic::AttackTarget(IVector posCenter, float range, DWORD targetId)
{
	DoSkillInfo *pDoSkillInfo = NULL;
	DWORD i, dwIndex = NOINDEX, dwSkillID = NOINDEX;;

	pDoSkillInfo = &g_pSkillList_Witch;

	for (i = 0; i < pDoSkillInfo->generalNum; i++)
	{
		if (pDoSkillInfo->pSkillGeneral[i].isBuff == 1)		// 버프 스킬인 경우
			continue;
		dwSkillID = GetSkillBarIDByName(pDoSkillInfo->pSkillGeneral[i].name);
		if (dwSkillID == NOINDEX)
			continue;
// 		if (GetSkillBarIndexByName(pDoSkillInfo->pSkillGeneral[i].name) == NOINDEX)
// 			continue;
		if (pDoSkillInfo->pSkillGeneral[i].isBuff == 2 && _localPlayer->actor.isMoving)
			continue;
		dwIndex = GetSkillIndexByName(pDoSkillInfo->pSkillGeneral[i].name, (WORD)dwSkillID);
		//dwIndex = GetSkillIndexByName(pDoSkillInfo->pSkillGeneral[i].name);
		if (dwIndex != NOINDEX)
		{
			if (IsEnableSkill(dwIndex) && _localPlayer->life.curMana >= pDoSkillInfo->pSkillGeneral[i].manaCost && 
				(_tick - pDoSkillInfo->pSkillGeneral[i].coolTime) >= pDoSkillInfo->pSkillGeneral[i].delay * 100)
			{
				TESTLOG(L"UseSkill [%s]", _localPlayer->actor.actorSkills[dwIndex].name.c_str());
				if(pDoSkillInfo->pSkillGeneral[i].isBuff == 2)
					UseSkill(_localPlayer->actor.actorSkills[dwIndex].id, posCenter);
				else
					UseSkill(_localPlayer->actor.actorSkills[dwIndex].id, targetId);
				pDoSkillInfo->pSkillGeneral[i].coolTime = _tick;
				SetDelay(3);
				return true;
			}
		}
	}

	return false;
}

bool GameLogic::PickItems()
{
	DWORD i, dwIndex = NOINDEX;
	float dist, distMin = 1000000000.f, range = 7.f;

	// Iterate world item list to pick up.
	for (i = 0; i < _entityList->worldItems.size(); i++)
	{
		dist = DIST(_localPlayer->positionedComp.gridPos, _entityList->worldItems[i].positionedComp.gridPos);
		if (dist > range)
			continue;

		// 주울 필요가 있는 아이템인가 확인 (1: 일반, 2: 퀘스트아이템)
		BYTE isGood = IsGoodItemToPick(&_entityList->worldItems[i].item);
		if (!isGood)
			continue;
		if (!GetEmptyCellByItemData(&_entityList->worldItems[i].item, isGood == 1))
			continue;
		if (!IsConnectedPoints(_localPlayer->positionedComp.gridPos, _entityList->worldItems[i].positionedComp.gridPos))
			continue;

		if (dist < distMin)
		{
			distMin = dist;
			dwIndex = i;
		}
	}

	if (dwIndex != NOINDEX)
	{
		TESTLOG(L"Pickup world item [%s]", _entityList->worldItems[dwIndex].item.displayName.c_str());
		SelectEntity(_entityList->worldItems[dwIndex].id);
		SetDelay(10);
		return true;
	}

	return false;
}

bool GameLogic::OpenChests()
{
	DWORD dwIndex = NOINDEX, i;
	float dist, distMin = 100000000.f;

	for (i = 0; i < _entityList->chests.size(); i++)
	{
		if (_entityList->chests[i].chest.isOpened)
			continue;
		if (_entityList->chests[i].chest.isLocked)
			continue;
		if (_entityList->chests[i].path.find(L"/QuestChests/") == -1 && std::find(g_pInfoQuestChests.begin(), g_pInfoQuestChests.end(), _entityList->chests[i].name) == g_pInfoQuestChests.end())
			continue;

		dist = DIST(_localPlayer->positionedComp.gridPos, _entityList->chests[i].positionedComp.gridPos);
		if (dist > 7.f)
			continue;
		if (!IsConnectedPoints(_localPlayer->positionedComp.gridPos, _entityList->chests[i].positionedComp.gridPos))
			continue;

		if (dist < distMin)
		{
			distMin = dist;
			dwIndex = i;
		}
	}

	if (dwIndex != NOINDEX)
	{
		TESTLOG(L"Open chest [%s]", _entityList->chests[dwIndex].name.c_str());
		SelectEntity(_entityList->chests[dwIndex].id);
		SetDelay(10);
		return true;
	}

	return false;
}

bool GameLogic::OpenDoors()
{
	DWORD dwIndex;

	for (DWORD i = 0; i < g_pInfoDoors.size(); i++)
	{
		if (g_pInfoDoors[i].keyItem && g_pInfoDoors[i].keyItem[0])		// 문을 열기 위한 열쇠가 필요한가를 확인한다
		{
			dwIndex = GetItemIndexByName(MainInventory1, g_pInfoDoors[i].keyItem);
			if (dwIndex == NOINDEX)
				continue;
		}
		dwIndex = GetNearestObjectByRender(_localPlayer->positionedComp.gridPos, 5.f, g_pInfoDoors[i].name, g_pInfoDoors[i].isConnect == 1, true);
		if (dwIndex != NOINDEX)
		{
			TESTLOG(L"Open door [%s]", _entityList->others[dwIndex].render.name.c_str());
			SelectEntity(_entityList->others[dwIndex].id);
			SetDelay(10);
			return true;
		}
	}
	
	return false;
}

bool GameLogic::OpenSpecialDoors()
{
	DWORD dwIndex;

	for (DWORD i = 0; i < g_pInfoSpecialDoors.size(); i++)
	{
		if (g_pInfoSpecialDoors[i].isPassed == 1)
			continue;		
		dwIndex = GetNearestSpecialObjectByRenderPath(_localPlayer->positionedComp.gridPos, g_pInfoSpecialDoors[i].range, g_pInfoSpecialDoors[i].name, g_pInfoSpecialDoors[i].path, g_pInfoSpecialDoors[i].transition);
		if (dwIndex != NOINDEX)
		{
			TESTLOG(L"Open Special Door [%s]", _entityList->others[dwIndex].render.name.c_str());
			if(g_pInfoSpecialDoors[i].isPassed == 0)
				g_pInfoSpecialDoors[i].isPassed = 1;
			SelectEntity(_entityList->others[dwIndex].id);
			if(g_pInfoSpecialDoors[i].delay)
				SetDelay(g_pInfoSpecialDoors[i].delay);
			else
				SetDelay(5);
			if (g_pInfoSpecialDoors[i].isPassed == 3)
			{
				_currentPath = nullptr;
				SetDelay(20);
			}
			return true;
		}
	}

	return false;
}

bool GameLogic::IsOpenedSpecialDoors(std::wstring doorName/* = L""*/)
{
	if (doorName.empty())
		return true;

	for (DWORD i = 0; i < g_pInfoSpecialDoors.size(); i++)
	{
		if(doorName != g_pInfoSpecialDoors[i].name)
			continue;
		if (g_pInfoSpecialDoors[i].isPassed)
			return true;
	}

	return false;
}


bool GameLogic::ApplyPassiveSkills()
{
	DWORD i, j;

	if (_ingameUi->passiveSkillUp.isVisible)		// 패스브 스킬을 찍을수 있으면
	{
		for (i = 0; i < g_infoPassiveSkill.size(); i++)
		{
			for (j = 0; j < _serverData->passiveSkills.size(); j++)
			{
				if (_serverData->passiveSkills[j].graphId == g_infoPassiveSkill[i].nGraphId)
					break;
			}
			if (j >= _serverData->passiveSkills.size())
			{
				if (!_ingameUi->passiveSkillPanel.isVisible)
				{
					PressKeyAlternate(L'P');
					SetDelay(20);
					return true;
				}

				Log(L"Apply Passive Skill [%s]", g_infoPassiveSkill[i].szId);
				PointPassiveSkill(_serverData->Address, g_infoPassiveSkill[i].nGraphId);
				SetDelay(10);
				return true;
			}
		}
	}

	if (_ingameUi->passiveSkillPanel.isVisible)
	{
		//Log(L"Close passive skill ui");
		PressKeyAlternate(L'P');
		//PressKeyAlternate(VK_ESCAPE);
		SetDelay(10);
		return true;
	}

	return false;
}

bool GameLogic::CleanInven()
{
	DWORD i, dwIndex;

	// 아이템 사용
	for (i = 0; i < g_pInfoItemsToUse.size(); i++)
	{
		dwIndex = GetItemIndexByName(MainInventory1, g_pInfoItemsToUse[i]);
		auto pInven = &_controller->playerInventories[MainInventory1];
		if (dwIndex != NOINDEX)
		{
			TESTLOG(L"Use item [%s]", pInven->inventory.slotItems[dwIndex].item.displayName.c_str());
			UseItem(_serverData->Address, pInven->id, pInven->inventory.slotItems[dwIndex].itemKey);
			SetDelay(5);
			return true;
		}
	}

	// 감정서가 10개 이상인 경우 레어 이상 등급의 아이템을 감정한다
	DWORD dwScrollIndex = GetItemIndexByName(MainInventory1, ITEM_WISDOM);
	if (dwScrollIndex != NOINDEX && GetItemCountByType(ITEM_WISDOM, MainInventory1, 0) > 10)
	{
		auto pInven = &_controller->playerInventories[MainInventory1];
		for (i = 0; i < pInven->inventory.slotItems.size(); i++)
		{
			ItemData *pItem = &pInven->inventory.slotItems[i].item;
			if (pItem->mods.nItemRarity < 2)
				continue;
			if (!pItem->mods.bIdentified)
			{
				TESTLOG(L"Use orb [%s]->[%s]", pInven->inventory.slotItems[dwScrollIndex].item.displayName.c_str(), pItem->displayName.c_str());
				UseOrb(MainInventory1, dwScrollIndex, MainInventory1, i);
				SetDelay(5);
				return true;
			}
		}
	}
	

	// 필요 없는 아이템들은 버린다
	if (!_worldArea->isTown)
	{
		InventoryHolder *pInven = &_controller->playerInventories[Cursor1];
		if (pInven->inventory.slotItems.size() > 0)
		{
			ItemData *pItem = &pInven->inventory.slotItems[0].item;
			if (_setting.toEquipInvenIndex == NOINDEX - 1 && _setting.toEquipItemName == pItem->displayName)
			{
				TESTLOG(L"Drop Item [%s]", pItem->displayName.c_str());
				DropItem(_serverData->Address);
				_setting.InitEquip();
				SetDelay(5);
				return true;
			}

			return false;
		}

		pInven = &_controller->playerInventories[MainInventory1];
		for (DWORD i = 0; i < pInven->inventory.slotItems.size(); i++)
		{
			ItemData *pItem = &pInven->inventory.slotItems[i].item;
			if (IsGoodItemToPick(pItem))
				continue;
			// 보석이 있는지 확인한다
			bool hadGem = false;
			for (int k = 0; k < pItem->itemSocket.lstItemSocketInfo.size(); k++)
			{
				if (pItem->itemSocket.lstItemSocketInfo[k].IsSocketEquiped)
				{
					hadGem = true;
				}
			}
			if (hadGem)
				continue;
			TESTLOG(L"Hold item to drop [%s]", pItem->displayName.c_str());
			MoveItemToCursor(MainInventory1, i);
			_setting.toEquipInvenIndex = NOINDEX - 1;
			_setting.toEquipItemName = pItem->displayName;
			_setting.holdType = 0;
			SetDelay(5);
			return true;
		}
	}

	return false;
}

BYTE GameLogic::InteractionOther(WCHAR* renderName)
{
	DWORD dwIndex;

	dwIndex = GetNearestObjectByRender(_localPlayer->positionedComp.gridPos, 7.f, renderName, false);
	if (dwIndex != NOINDEX)
	{
		if (_entityList->others[dwIndex].transitionable.flag1 == 1)
		{
			TESTLOG(L"Interaction Other [%s]", _entityList->others[dwIndex].render.name.c_str());
			SelectEntity(_entityList->others[dwIndex].id);
			SetDelay(10);
			return 1;
		}
		else if (_entityList->others[dwIndex].transitionable.flag1 == 2)
		{
			return 2;
		}
	}

	return 0;
}

BYTE GameLogic::InteractionNpc(std::wstring	 strRender)
{
	DWORD dwIndex;

	dwIndex = GetNearestNpc(_localPlayer->positionedComp.gridPos, 7.f, strRender);
	if (dwIndex != NOINDEX)
	{
		if (_entityList->npcs[dwIndex].npc.hasIconOverhead)
		{
			TESTLOG(L"Interaction NPC [%s]", _entityList->npcs[dwIndex].name.c_str());
			SelectEntity(_entityList->npcs[dwIndex].id);
			SetDelay(10);
			return 1;
		}
		else
		{
			return 2;
		}
	}

	return 0;
}

bool GameLogic::InteractionMonster(WCHAR* szName, float range)
{
	DWORD i, dwIndex = NOINDEX;
	float dist, distMin = 1000000000.f;

	for (i = 0; i < _entityList->monsters.size(); i++)
	{
		if (_entityList->monsters[i].name != szName)
			continue;
		if (!_entityList->monsters[i].targetable.isTargetable)
			continue;
		dist = DIST(_localPlayer->positionedComp.gridPos, _entityList->monsters[i].positionedComp.gridPos);
		if (dist > range)
			continue;
// 		if (!IsConnectedPoints(_localPlayer->positionedComp.gridPos, _entityList->monsters[i].positionedComp.gridPos))
// 			continue;
		dist = DIST(_localPlayer->positionedComp.gridPos, _entityList->monsters[i].positionedComp.gridPos);
		if (dist < distMin)
		{
			distMin = dist;
			dwIndex = i;
			break;
		}
	}

	if (dwIndex != NOINDEX)
	{
		TESTLOG(L"Interation Monster [%s]", _entityList->monsters[dwIndex].name.c_str());
		SelectEntity(_entityList->monsters[dwIndex].id);
		SetDelay(10);
		return true;
	}
	return false;
}

bool GameLogic::UseSkillByKey(std::wstring skillName, DWORD targetId)
{
	DWORD dwIndex = GetSkillBarIndexByName(skillName);
	if (dwIndex == NOINDEX)
		return false;

	switch (dwIndex)
	{
	case 0:
	{
		Click(0, 0, FALSE, FALSE);
	}
	break;
	case 1:
	{
		MClick(0, 0, FALSE, FALSE);
	}
	break;
	case 2:
	{
		RClick(0, 0, FALSE, FALSE);
	}
	break;
	case 3:
	{
		PressKeyAlternate(L'Q');
	}
	break;
	case 4:
	{
		PressKeyAlternate(L'W');
	}
	break;
	case 5:
	{
		PressKeyAlternate(L'E');
	}
	break;
	case 6:
	{
		PressKeyAlternate(L'R');
	}
	break;
	case 7:
	{
		PressKeyAlternate(L'T');
	}
	break;
	}

	return false;
}

bool GameLogic::UsePortalScroll()
{
	DWORD dwIndex;

	if (_worldArea->isTown)
		return false;

	dwIndex = GetNearestObjectByPath(_localPlayer->positionedComp.gridPos, 7.f, L"Metadata/MiscellaneousObjects/MultiplexPortal", true);
	if (dwIndex != NOINDEX)
	{
		Log(L"Enter the portal [%s]", _worldArea->name.c_str());
		SelectEntity(_entityList->others[dwIndex].id);
		SetDelay(10);
		return true;
	}

	dwIndex = GetItemIndexByName(MainInventory1, ITEM_PORTAL);
	if (dwIndex != NOINDEX)
	{
		auto pInven = &_controller->playerInventories[MainInventory1];
		UseItem(_serverData->Address, pInven->id, pInven->inventory.slotItems[dwIndex].itemKey);
		SetDelay(5);
		return true;
	}

	return false;
}

bool GameLogic::SellItemToNpc(std::wstring npcName, BYTE npcElement)
{
	DWORD dwIndex = NOINDEX;
	DWORD dwSellIndex = NOINDEX;
	DWORD i = 0, j = 0;

	if (_ingameUi->sellItem.isVisible)
	{
		auto pInven = &_controller->playerInventories[MainInventory1];
		auto pNpcInven1 = &_controller->npcInventories[1];
		for (i = 0; i < pInven->inventory.slotItems.size(); i++)
		{
			auto *pItem = &pInven->inventory.slotItems[i];
			if (!HasItemToSell(&pItem->item))
			{
				continue;
			}
			for (j = 0; j < pNpcInven1->inventory.slotItems.size(); j++)
			{
				auto *pNpcItem = &pNpcInven1->inventory.slotItems[j];

				if ((pNpcItem->location.xMin == pItem->location.xMin) && (pNpcItem->location.yMin == pItem->location.yMin))
				{
					dwSellIndex = j;
					break;
				}
			}
			if (dwSellIndex == NOINDEX)
			{
				MoveTradeItem(pInven->id, pItem->itemKey, pItem->location.xMin, pItem->location.yMin);
				SetDelay(10);
				return true;
			}
		}
		if (_controller->npcInventories.size())
		{
			auto pNpcInven2 = &_controller->npcInventories[0];
			if (pNpcInven2->inventory.slotItems.size())
			{
				Log(L"TradeAccept");
				TradeAccept(pNpcInven2->inventory.slotItems[pNpcInven2->inventory.slotItems.size() - 1].itemKey);
				SetDelay(10);
				return true;
			}
		}
		return false;
	}

	dwIndex = GetNearestNpc(_localPlayer->positionedComp.gridPos, 5.f, npcName, false);
	if (dwIndex != NOINDEX)
	{
		if (_ingameUi->interaction.isVisible && _ingameUi->interaction.childCount == 2)
		{
			if (_setting.selectedNpcID == _entityList->npcs[dwIndex].id)
			{
				NpcElementClick(npcElement);
				SetDelay(10);
				_setting.selectedNpcID = 0;
				return true;
			}
		}
		else
		{
			SelectEntity(_entityList->npcs[dwIndex].id);
			SetDelay(10);
			_setting.selectedNpcID = _entityList->npcs[dwIndex].id;
			return true;
		}
	}

	return false;
}

bool GameLogic::PurchaseItem(std::wstring npcName, BYTE npcElement)
{
	DWORD dwIndex = NOINDEX;
	DWORD dwPurchaseItemIndex = NOINDEX;
	DWORD i = 0, j = 0;

	if (_ingameUi->purchaseItem.isVisible)
	{
		dwPurchaseItemIndex = GetPurchaseItemIndex(npcName);
		if (dwPurchaseItemIndex != NOINDEX)
		{
			for (i = 0; i < _controller->npcInventories.size(); i++)
			{
				auto pNpcInven = &_controller->npcInventories[i];

				for (j = 0; j < pNpcInven->inventory.slotItems.size(); j++)
				{
					auto *pNpcItem = &pNpcInven->inventory.slotItems[j];
					if (g_pInfoPurchaseItem[dwPurchaseItemIndex].purchaseItemName == pNpcItem->item.displayName)
					{
						int emptyX, emptyY;
						if (GetEmptyCellByItemData(&pNpcItem->item, false, &emptyX, &emptyY))
						{
							Log(L"Purchase Item %s Key = %d Xpos = %d Ypos = %d Width = %d Height = %d", pNpcItem->item.displayName.c_str(), pNpcItem->itemKey, emptyX, emptyY, pNpcItem->item.itemWidth, pNpcItem->item.itemHeight);
							ReceiveReward(j, MainInventory1, emptyX, emptyY, (BYTE)i);
							SetDelay(10);
							return true;
						}
					}
				}
			}
		}
		return false;
	}
	dwIndex = GetNearestNpc(_localPlayer->positionedComp.gridPos, 5.f, npcName, false);
	if (dwIndex != NOINDEX)
	{
		if (_ingameUi->interaction.isVisible && _ingameUi->interaction.childCount == 2)
		{
			if (_setting.selectedNpcID == _entityList->npcs[dwIndex].id)
			{
				NpcElementClick(npcElement);
				SetDelay(10);
				_setting.selectedNpcID = 0;
				return true;
			}
		}
		else
		{
			SelectEntity(_entityList->npcs[dwIndex].id);
			SetDelay(10);
			_setting.selectedNpcID = _entityList->npcs[dwIndex].id;
			return true;
		}
	}
	return false;
}

bool GameLogic::StoreInvenItems()
{
	DWORD dwIndex = NOINDEX;
	DWORD dwItemIndex = NOINDEX;
	DWORD dwHoldItemIndex = NOINDEX;
	DWORD i = 0;

	if (_ingameUi->stashElement.isVisible)
	{
		if (_controller->playerInventories.size() < 42)
		{
			TESTLOG(L"Next Stash %d %d", (int)_controller->playerInventories.size(), _setting.prevStashIndex);
			GetNextStashInfo(_setting.prevStashIndex);
			_setting.prevStashIndex++;
			if (_setting.prevStashIndex >= 4)
				_setting.prevStashIndex = 0;
			SetDelay(10);
			return true;
		}
		_setting.InitStash();
		auto pHoldInven = &_controller->playerInventories[Cursor1];
		if (pHoldInven->inventory.slotItems.size())
		{
			if (_setting.holdType == 2)
			{
				TESTLOG(L"StashIndex = %d Cell = %d/%d", _setting.toEquipInvenIndex, _setting.toEquipCellX, _setting.toEquipCellY);
				MoveItem(_setting.toEquipInvenIndex, _setting.toEquipCellX, _setting.toEquipCellY);
				_setting.InitEquip();
			}
			else
			{
				int emptyX = 0, emptyY = 0;
				auto itemData = &pHoldInven->inventory.slotItems[0].item;
				if (!GetEmptyCellByItemData(itemData, false, &emptyX, &emptyY))
				{
					Log(L"Cursor item error! [%s], [%s], %d", itemData->displayName.c_str(), _setting.toEquipItemName.c_str(), _setting.toEquipInvenIndex);
					ToggleRun(0);
					return true;
				}

				TESTLOG(L"Move from cursor1 to main inventory [%s] (%d, %d)", itemData->displayName.c_str(), emptyX, emptyY);
				MoveItem(MainInventory1, emptyX, emptyY);
			}
			SetDelay(10);
			return true;
		}
		for (i = 0; i < 3; i++)
		{
			dwItemIndex = GetItemIndexToStore((BYTE)i);
			//Log(L"ItemIndex = %d", dwItemIndex);

			if (dwItemIndex != NOINDEX)
			{
				int emptyX, emptyY;
				auto pInven = &_controller->playerInventories[MainInventory1];
				if (i == 0)
				{
					if (GetEmptyCellByItemData(&pInven->inventory.slotItems[dwItemIndex].item, false, &emptyX, &emptyY, Stash1 + i))
					{
						TESTLOG(L"InvenId1 = %d Name = %s Size = %d/%d key = %08X", pInven->id, pInven->inventory.slotItems[dwItemIndex].item.displayName.c_str(), pInven->inventory.slotItems[dwItemIndex].item.stack.Size, pInven->inventory.slotItems[dwItemIndex].item.stack.MaxStackSize, pInven->inventory.slotItems[dwItemIndex].itemKey);
						_setting.toEquipInvenIndex = Stash1 + i;
						_setting.toEquipCellX = emptyX;
						_setting.toEquipCellY = emptyY;
						_setting.toEquipItemName = pInven->inventory.slotItems[dwItemIndex].item.displayName;
						_setting.holdType = 2;
						MoveItemToCursor(MainInventory1, dwItemIndex);
						SetDelay(10);
						return true;
					}
					else if (GetEmptyCellByItemData(&pInven->inventory.slotItems[dwItemIndex].item, false, &emptyX, &emptyY, Stash1 + i + 1))
					{
						TESTLOG(L"InvenId2 = %d Name = %s Size = %d/%d key = %08X", pInven->id, pInven->inventory.slotItems[dwItemIndex].item.displayName.c_str(), pInven->inventory.slotItems[dwItemIndex].item.stack.Size, pInven->inventory.slotItems[dwItemIndex].item.stack.MaxStackSize, pInven->inventory.slotItems[dwItemIndex].itemKey);
						_setting.toEquipInvenIndex = Stash1 + i + 1;
						_setting.toEquipCellX = emptyX;
						_setting.toEquipCellY = emptyY;
						_setting.toEquipItemName = pInven->inventory.slotItems[dwItemIndex].item.displayName;
						_setting.holdType = 2;
						MoveItemToCursor(MainInventory1, dwItemIndex);
						SetDelay(10);
						return true;
					}
					else
					{
						continue;
					}
				}
				else if (GetEmptyCellByItemData(&pInven->inventory.slotItems[dwItemIndex].item, false, &emptyX, &emptyY, Stash1 + i + 1))
				{
					TESTLOG(L"InvenId3 = %d Name = %s Size = %d/%d key = %08X", pInven->id, pInven->inventory.slotItems[dwItemIndex].item.displayName.c_str(), pInven->inventory.slotItems[dwItemIndex].item.stack.Size, pInven->inventory.slotItems[dwItemIndex].item.stack.MaxStackSize, pInven->inventory.slotItems[dwItemIndex].itemKey);
					_setting.toEquipInvenIndex = Stash1 + i + 1;
					_setting.toEquipCellX = emptyX;
					_setting.toEquipCellY = emptyY;
					_setting.toEquipItemName = pInven->inventory.slotItems[dwItemIndex].item.displayName;
					_setting.holdType = 2;
					MoveItemToCursor(MainInventory1, dwItemIndex);
					SetDelay(10);
					return true;
				}
				else
				{
					continue;
				}
			}
			else
			{
				continue;
			}
		}
	}
	else
	{
		dwIndex = GetNearestObjectByRender(_localPlayer->positionedComp.gridPos, 5.f, STASH_NAME, false);
		if (dwIndex != NOINDEX)
		{
			TESTLOG(L"Select Stash");
			_setting.prevStashIndex = 1;
			SelectEntity(_entityList->others[dwIndex].id);
			SetDelay(30);
			return true;
		}
	}

	return false;
}

bool GameLogic::ProcessInTown()
{
	DWORD i, dwIndex = NOINDEX;

	if (!_worldArea->isTown)
		return false;

	// 아이템 판매
	if (GetItemIndexToSell() != NOINDEX)
	{
		for (i = 0; i < g_pInfoTradeNpcs.size(); i++)
		{
			dwIndex = GetMinimapIconCacheIndex(NPC_NAME, g_pInfoTradeNpcs[i].TraderNpcName, L"", true);
			if (dwIndex != NOINDEX)
			{
				if (_setting.targetPathType == TPT_None)
				{
					Log(L"Target Path - NPC to Sell -> [%s]", _map.iconsCache[dwIndex].name.c_str());
					if (LoadMinimapIconTargetPath(dwIndex))
					{
						_setting.targetPathType = TPT_GotoNpc;
						return true;
					}
				}

				if (DIST(_localPlayer->positionedComp.gridPos, _map.iconsCache[dwIndex].pos.gridPos) < 5.0f)
				{
					SetStatus(MS_NPC_Sell, 0, 0, true);
					return true;
				}

				break;
			}
		}
	}
	
	
	// 아이템 구매
	for (i = 0; i < g_pInfoTradeNpcs.size(); i++)
	{
		dwIndex = GetMinimapIconCacheIndex(NPC_NAME, g_pInfoTradeNpcs[i].TraderNpcName, L"", true);
		if (dwIndex != NOINDEX)
		{
			if (GetPurchaseItemIndex(g_pInfoTradeNpcs[i].TraderNpcName) != NOINDEX)
			{
				if (_setting.targetPathType == TPT_None)
				{
					Log(L"Target Path - NPC to Buy -> [%s]", _map.iconsCache[dwIndex].name.c_str());
					if (LoadMinimapIconTargetPath(dwIndex))
					{
						_setting.targetPathType = TPT_GotoNpc;
						return true;
					}
				}

				if (DIST(_localPlayer->positionedComp.gridPos, _map.iconsCache[dwIndex].pos.gridPos) < 5.0f)
				{
					SetStatus(MS_NPC_Purchase, 0, 0, true);
					return true;
				}

				break;
			}
		}
	}

	// 창고 맡기기
	bool useStash = false;
	for (i = 0; i < 3; i++)
	{
		dwIndex = GetItemIndexToStore((BYTE)i);
		if (dwIndex != NOINDEX)
		{
			useStash = true;
			break;
		}
	}
	auto pHoldInven = &_controller->playerInventories[Cursor1];
	if (pHoldInven->inventory.slotItems.size() == 0)
		_setting.InitEquip();
	if (_setting.holdType == 2)
		useStash = true;
	if (useStash)
	{
		dwIndex = GetMinimapIconCacheIndex(STASH_ICON, STASH_NAME, L"", true);
		if (dwIndex != NOINDEX)
		{
			if (_setting.targetPathType == TPT_None)
			{
				Log(L"Target Path - Stash -> [%s]", _map.iconsCache[dwIndex].name.c_str());
				if (LoadMinimapIconTargetPath(dwIndex))
				{
					_setting.targetPathType = TPT_GotoObject;
					return true;
				}
			}

			if (DIST(_localPlayer->positionedComp.gridPos, _map.iconsCache[dwIndex].pos.gridPos) < 5.0f)
			{
				SetStatus(MS_Stash_Deposit, 0, 0, true);
				return true;
			}
		}
	}
	
	return false;
}

void GameLogic::RunField()
{
	DWORD dwIndex = NOINDEX, i;
	float dist, distMin = 1000000000.f, range = 1.0f;

	if (_setting.newMapBlock)
	{
		_setting.newMapBlock = 0;
		LoadFullPath();
	}

	if (_worldArea->name == L"Lioneye's Watch")
	{
		dwIndex = GetNearestObjectByRender(_localPlayer->positionedComp.gridPos, 15, L"The Coast");
		if (dwIndex != NOINDEX)
		{
			Log(L"Select [%s]", _entityList->others[dwIndex].render.name.c_str());
			SelectEntity(_entityList->others[dwIndex].id);
			SetDelay(50);
			return;
		}
		return;
	}

	if (_worldArea->name != L"The Coast")
	{
		return;
	}

	if (!_currentPath)
	{
		LoadFullPath();
		return;
	}

	if (_moveStep == NOINDEX)
	{
		distMin = 1000000000.f;
		dwIndex = NOINDEX;
		for (i = _currentPathSize - 1; (int)i >= 0; i--)
		{
			dist = DIST(_localPlayer->positionedComp.gridPos, _currentPath[i]);
			if (dist <= distMin)
			{
				distMin = dist;
				dwIndex = i;
			}
		}

		if (dwIndex == NOINDEX)
		{
			Log(L"Faield to find the current step on the path.");
			ToggleRun(0);
			return;
		}

		Log(L"MoveStep = %d", dwIndex);
		_moveStep = dwIndex;
	}

	if (_moveStep >= _currentPathSize)
	{
		Log(L"Arrived last point.");
		ToggleRun(0);
		return;
	}

	range = 2.0f;
	dist = DIST(_localPlayer->positionedComp.gridPos, _currentPath[_moveStep]);

	if (UsePortion())
		return;

	// Attack near monster
	dwIndex = GetNearestAttackingMonster(_localPlayer->positionedComp.gridPos, 7.f);
	if (dwIndex != NOINDEX)
	{
		Log(L"Attack_Near [%s, %d, %.1f]", 
			_entityList->monsters[dwIndex].path.c_str(), 
			_entityList->monsters[dwIndex].life.curHP,
			DIST(_localPlayer->positionedComp.gridPos, _entityList->monsters[dwIndex].positionedComp.gridPos));
		AttackTarget(_currentPath[_moveStep], 20.f, _entityList->monsters[dwIndex].id);
		return;
	}

	if (PickItems())
		return;
	
	if (dist > range)
	{
		Log(L"Move (%d, %d), (%d, %d) %.1f", _localPlayer->positionedComp.gridPos.x, _localPlayer->positionedComp.gridPos.y, _currentPath[_moveStep].x, _currentPath[_moveStep].y, dist);
		MoveToPos(_currentPath[_moveStep]);
		SetDelay(2);
		return;
	}

	_moveStep++;
}

void GameLogic::Main()
{
	DWORD i = 0, dwIndex = NOINDEX;

	_tick = GetTickCount64();
	if (_delay)
	{
		_delay--;
		return;
	}

	if (!_isRun)
		return;

	if (!_controller->stateController->LoginStatePtr)
	{
		_controller->stateController->UpdateStates();
		SetDelay(10);
		return;
	}

	if (GameStateController::IsLoading())
	{
		Log(L"Loading...");
		SetDelay(30);
		return;
	}

	if (_status >= MS_Main)
	{
		_controller->Update();
	}

	switch (_status)
	{
	case MS_First:
	{
		if (GameStateController::IsLoginState())
		{
			Click(10, 10, FALSE, FALSE);
			SetStatus(MS_Login, 0, 10);
		}
	}
	break;
	case MS_Login:
	{
		if (GameStateController::IsSelectCharacterState())
		{
			SetStatus(MS_CharacterSelect, 0, 30);
			return;
		}
		switch (_subStatus)
		{
		case 0:
		{
			Log(L"Account [%s]", g_pGameSettingDLL->szGameID);
			if (!GetLoginedAccount(GameStateController::LoginStatePtr).empty())
				SettingGameAccount(GameStateController::LoginStatePtr, L"");
			std::wstring	str = g_pGameSettingDLL->szGameID;
			for (int i = 0; i < str.length(); i++)
			{
				InputGameAccount(GameStateController::LoginStatePtr, i, (char)str[i]);
			}
			SetStatus(MS_Login, 1);
		}
		break;
		case 1:
		{
			Log(L"Password [%s]", g_pGameSettingDLL->szGamePW);
			std::wstring	str = g_pGameSettingDLL->szGamePW;
			for (int i = 0; i < str.length(); i++)
			{
				InputGamePassword(GameStateController::LoginStatePtr, i, (char)str[i]);
			}
			SetStatus(MS_Login, 2);
		}
		break;
		case 2:
		{
			PressKeyAlternate(VK_RETURN);
			SetStatus(MS_Login, 3);
		}
		break;
		}
		SetDelay(10);
	}
	break;
	case MS_CharacterSelect:
	{
		if (GameStateController::IsInGameState())
		{
			_setting.Init();
			_map.Init();
			SetStatus(MS_Main, 0, 50);
			return;
		}
		GameStateController::SelectCharacter.Update(GameStateController::SelectCharacterStatePtr);
		DWORD dwCharNo = g_pGameSettingDLL->dwCharNo;
		if (GameStateController::SelectCharacter.lstCharInfo.size() > dwCharNo)
		{
			Log(L"Play Game %d-[%s]", g_pGameSettingDLL->dwCharNo, GameStateController::SelectCharacter.lstCharInfo[dwCharNo].Name.c_str());
			PlayGame(GameStateController::SelectCharacterStatePtr, GameStateController::SelectCharacter.lstCharInfo[dwCharNo].Name, dwCharNo);
			SetDelay(50);
		}
		SetDelay(10);
	}
	break;
	case MS_Main:
	{
		ReadMapBlock();

		if (_localPlayer->life.curHP == 0)
		{
			Log(L"Dead!");
			_setting.isDead = true;
			_setting.posDead = _localPlayer->positionedComp.gridPos;
			_setting.newMapBlock = 1;

			for (DWORD i = 0; i < g_pInfoSpecialDoors.size(); i++)
			{
				if(g_pInfoSpecialDoors[i].isPassed == 1)
					g_pInfoSpecialDoors[i].isPassed = 0;
			}

			Resurrect();
			SetDelay(30);
			return;
		}

		if (_setting.firstOpenedWorldMap == 0)
		{
			if (!_ingameUi->worldmap.isVisible)
			{
				Log(L"First world map.");
				PressKeyAlternate(L'U');
				SetDelay(10);
				return;
			}
		}
		if (_setting.firstOpenedWorldMap == 0 || _setting.firstOpenedWorldMap == 2)
		{
			if (_ingameUi->worldmap.isVisible)
			{
				PressKeyAlternate(L'U');
				_setting.firstOpenedWorldMap = 1;
				SetDelay(10);
				return;
			}
		}

		QuestRun();
		//RunField();
	}
	break;
	case MS_NPC_Sell:
	{
		switch (_subStatus)
		{
		case 0:
		{
			if (_ingameUi->interaction.isVisible && _ingameUi->interaction.childCount == 2)
			{
				PressKeyAlternate(VK_ESCAPE);
				SetDelay(10);
				_setting.selectedNpcID = 0;
				return;
			}

			SetStatus(_status, 1, 0);
		}
		break;
		case 1:
		{
			if (ContinueDialog())
				return;
			if (ProcessReward())
				return;

			if (GetItemIndexToSell() != NOINDEX)
			{
				for (i = 0; i < g_pInfoTradeNpcs.size(); i++)
				{
					dwIndex = GetNearestNpc(_localPlayer->positionedComp.gridPos, 5.f, g_pInfoTradeNpcs[i].TraderNpcName, false);
					if (dwIndex != NOINDEX)
					{
						SellItemToNpc(g_pInfoTradeNpcs[i].TraderNpcName, g_pInfoTradeNpcs[i].NpcElement[0]);
					}
				}

				return;
			}

			SetStatus(_status, 2, 0);
		}
		break;
		case 2:
		{
			if (_ingameUi->sellItem.isVisible)
			{
				Log(L"Close Sell Item UI.");
				PressKeyAlternate(VK_ESCAPE);
				SetDelay(10);
				return;
			}
			if (_ingameUi->interaction.isVisible && _ingameUi->interaction.childCount == 2)
			{
				PressKeyAlternate(VK_ESCAPE);
				SetDelay(10);
				_setting.selectedNpcID = 0;
				return;
			}
			SetStatus(_prevStatus, _prevSubStatus, 0);
		}
		break;
		}
	}
	break;
	case MS_NPC_Purchase:
	{
		switch (_subStatus)
		{
		case 0:
		{
			if (_ingameUi->interaction.isVisible && _ingameUi->interaction.childCount == 2)
			{
				PressKeyAlternate(VK_ESCAPE);
				SetDelay(10);
				_setting.selectedNpcID = 0;
				return;
			}

			SetStatus(_status, 1, 0);
		}
		break;
		case 1:
		{
			if (ContinueDialog())
				return;
			if (ProcessReward())
				return;

			for (i = 0; i < g_pInfoTradeNpcs.size(); i++)
			{
				dwIndex = GetNearestNpc(_localPlayer->positionedComp.gridPos, 5.f, g_pInfoTradeNpcs[i].TraderNpcName, false);
				if (dwIndex != NOINDEX)
				{
					if (GetPurchaseItemIndex(g_pInfoTradeNpcs[i].TraderNpcName) != NOINDEX)
					{
						PurchaseItem(g_pInfoTradeNpcs[i].TraderNpcName, g_pInfoTradeNpcs[i].NpcElement[1]);
						return;
					}
				}
			}

			SetStatus(_status, 2, 0);
		}
		break;
		case 2:
		{
			if (_ingameUi->purchaseItem.isVisible)
			{
				Log(L"Close Purchase Item UI.");
				PressKeyAlternate(VK_ESCAPE);
				SetDelay(10);
				return;
			}
			if (_ingameUi->interaction.isVisible && _ingameUi->interaction.childCount == 2)
			{
				PressKeyAlternate(VK_ESCAPE);
				SetDelay(10);
				_setting.selectedNpcID = 0;
				return;
			}
			SetStatus(_prevStatus, _prevSubStatus, 0);
		}
		break;
		}
	}
	break;
	case MS_Stash_Deposit:
	{
		switch (_subStatus)
		{
		case 0:
		{
			bool useStash = false;
			for (i = 0; i < 3; i++)
			{
				dwIndex = GetItemIndexToStore((BYTE)i);
				if (dwIndex != NOINDEX)
				{
					useStash = true;
					break;
				}
			}
			auto pHoldInven = &_controller->playerInventories[Cursor1];
			if (pHoldInven->inventory.slotItems.size() == 0)
				_setting.InitEquip();
			if (_setting.holdType == 2)
				useStash = true;
			if (useStash)
			{
				StoreInvenItems();
				return;
			}

			SetStatus(_status, 1, 0);
		}
		break;
		case 1:
		{
			if (_ingameUi->stashElement.isVisible)
			{
				Log(L"Close Stash UI.");
				PressKeyAlternate(VK_ESCAPE);
				SetDelay(10);
				return;
			}
			SetStatus(_prevStatus, _prevSubStatus, 0);
		}
		break;
		}
	}
	break;
	case MS_Stash_Withdraw:
	{

	}
	break;
	}

}