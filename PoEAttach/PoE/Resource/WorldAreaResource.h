#pragma once

#include <Windows.h>

#define			WORLDAREA_RES_COUNT					1478
#define			WORLDAREA_CONNECTIONS_MAX_COUNT		10

struct PoEResWorldArea
{
	DWORD	dwKey;
	WCHAR	*szName;
	BYTE	bAct;
	BYTE	bIsTown;
	BYTE	bHasWaypoint;
	DWORD	dwNumConnections;
	DWORD	pdwConnectionsList[WORLDAREA_CONNECTIONS_MAX_COUNT];
	BYTE	bAreaLevel;
	DWORD	dwID;
	BYTE	bIsHideout;
};

extern	PoEResWorldArea	g_pResWorldArea[WORLDAREA_RES_COUNT];

PoEResWorldArea *   GetWorldAreaResourceByID(DWORD dwAreaID);
PoEResWorldArea *   GetWorldAreaResourceByName(const WCHAR *szAreaName);
