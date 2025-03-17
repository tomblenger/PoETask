#pragma once

#include <vector>
#include "RemoteMemoryObject.h"

class WorldArea : public RemoteMemoryObject
{
public:
	WorldArea();
	void	Update(uintptr_t addrPointer);

	std::wstring	id;
	std::wstring	name;
	int				act;
	bool			isTown;
	bool			hasWaypoint;
	int				areaLevel;
	int				worldAreaID;
	bool			isAtlasMap;
	bool			isMapWorlds;
	bool			isCorruptedArea;
	bool			isMissionArea;
	bool			isDailyArea;
	bool			isMapTrialArea;
	bool			isLabyrinthArea;
	bool			isAbyssArea;
	std::vector<uintptr_t>	connections;
	std::vector<uintptr_t>	corruptedAreas;

	//AreaTemplate
	bool			isMap;
	bool			isEndGameMap;
};