#include "stdafx.h"
#include "IngameData.h"
#include "Patch.h"

IngameData::IngameData()
{
	currentAreaPtr = 0;
	currentAreaLevel = 0;
	currentAreaHash = 0;
	localPlayerPtr = 0;
	entityListPtr = 0;
	labyrinthDataPtr = 0;
}

void IngameData::Update(uintptr_t addrPointer)
{
	Address = addrPointer;
	if (!Address)
		return;

	currentAreaPtr = *(uintptr_t *)(Address + DATA_OFFSET_INGAMEDATA_WORLDAREA);
	currentWorldArea.Update(currentAreaPtr);
	currentAreaLevel = *(BYTE *)(Address + DATA_OFFSET_INGAMEDATA_WORLDAREA + 0x18);
	currentAreaHash = *(UINT *)(Address + 0xCC);
	localPlayerPtr = *(uintptr_t *)(Address + DATA_INGAMEDATA_LOCALPLAYER);
	entityListPtr = Address + DATA_INGAMEDATA_LOCALPLAYER + 0x88;
	labyrinthDataPtr = *(uintptr_t *)(Address + 0x11C);
}
