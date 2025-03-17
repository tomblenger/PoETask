#include "stdafx.h"
#include "WorldArea.h"
#include "Patch.h"

WorldArea::WorldArea()
{
	act = 0;
	isTown = false;
	hasWaypoint = false;
	areaLevel = 0;
	worldAreaID = 0;
	isAtlasMap = false;
	isMapWorlds = false;
	isCorruptedArea = false;
	isMissionArea = false;
	isDailyArea = false;
	isMapTrialArea = false;
	isLabyrinthArea = false;
	isAbyssArea = false;
	isMap = false;
	isEndGameMap = false;
}

void WorldArea::Update(uintptr_t addrPointer)
{
	Address = addrPointer;
	if (!Address)
		return;

	id = ReadStringU(*(uintptr_t*)Address);
	name = ReadStringU(*(uintptr_t *)(Address + 0x08));
	act = *(int *)(Address + 0x10);
	isTown = *(BYTE *)(Address + DATA_OFFSET_WORLDAREA_TOWN) == 1;
	hasWaypoint = *(BYTE *)(Address + DATA_OFFSET_WORLDAREA_TOWN + 1) == 1;
	areaLevel = *(int *)(Address + DATA_OFFSET_WORLDAREA_TOWN + 0x12);
	worldAreaID = *(int *)(Address + DATA_OFFSET_WORLDAREA_TOWN + 0x16);
	isAtlasMap = id.find(L"MapAtlas") == 0;
	isMapWorlds = id.find(L"MapWorlds") == 0;
	isCorruptedArea = id.find(L"SideArea") != std::wstring::npos || id.find(L"Sidearea") != std::wstring::npos;
	isMissionArea = id.find(L"Mission") != std::wstring::npos;
	isDailyArea = id.find(L"Daily") != std::wstring::npos;
	isMapTrialArea = id.find(L"EndGame_Labyrinth_trials") == 0;
	isLabyrinthArea = !isMapTrialArea && id.find(L"Labyrinth") != std::wstring::npos;
	isAbyssArea = !id.compare(L"AbyssLeague") || !id.compare(L"AbyssLeague2") || !id.compare(L"AbyssLeagueBoss") || !id.compare(L"AbyssLeagueBoss2") || !id.compare(L"AbyssLeagueBoss3");
	
	//AreaTemplate
	isMap = *(BYTE *)(Address + 0x7A) > 0;
	isEndGameMap = *(BYTE *)(Address + 0xBA) > 0;
	//

 	connections.clear();
 	int connectionsCount = *(int *)(Address + DATA_OFFSET_WORLDAREA_TOWN + 2);
	if (connectionsCount < 11)
	{
		uintptr_t connectionsPtr = *(uintptr_t *)(Address + DATA_OFFSET_WORLDAREA_TOWN + 0x0A);
		for (int i = 0; i < connectionsCount; i++)
		{
			connections.push_back(*(uintptr_t *)(connectionsPtr));
			connectionsPtr += 8;
		}
	}

	corruptedAreas.clear();
	int corruptedAreasCount = *(int *)(Address + 0xFB);
	if (corruptedAreasCount < 11)
	{
		uintptr_t corruptedAreasPtr = *(uintptr_t *)(Address + 0x103);
		for (int i = 0; i < corruptedAreasCount; i++)
		{
			corruptedAreas.push_back(*(uintptr_t *)(corruptedAreasPtr));
			corruptedAreasPtr += 8;
		}
	}
}