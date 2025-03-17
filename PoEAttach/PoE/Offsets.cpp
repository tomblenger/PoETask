#include "stdafx.h"
#include "Offsets.h"
#include "Patch.h"

Offsets * Offsets::Instance = nullptr;

Offsets::Offsets()
{
	Instance = this;

	AreaChangeCount = 0;
	Base = 0;
	FileRoot = 0;
	GameStateOffset = 0;

	IgsDelta = 0;
	IgsOffset = 0;
}

void Offsets::DoPatternScans()
{
	FileRoot = TO_POE(ADDR_DATA_FILEROOT);
	AreaChangeCount = TO_POE(ADDR_DATA_AREACHANGECOUNT);
	GameStateOffset = TO_POE(ADDR_DATA_GAMESTATE);
}