#include "stdafx.h"
#include "IngameState.h"
#include "GameStateController.h"
#include "Offsets.h"
#include "Patch.h"

IngameState::IngameState()
{
	entityLabelMap = 0;
	uiRootPtr = 0;
	uiHoverTooltipPtr = 0;
	currentUIElementPosX = 0.f;
	currentUIElementPosY = 0.f;
	uiHoverPtr = 0;
	uiHoverX = 0.f;
	uiHoverY = 0.f;
	timeInGameF = 0.f;
	cameraPtr = 0;
}

void IngameState::Update()
{
	if (!Address || !GameStateController::IsInGameState())
		return;

	ingameUi.Update(*(uintptr_t *)(Address + 0x78));
	entityLabelMap = *(uintptr_t *)(*(uintptr_t *)(Address + 0x98) + 0x2A0);
	data.Update(*(uintptr_t *)(Address + DATA_OFFSET_INGAMESTATE_SERVERDATA - 8 + Offsets::Instance->IgsOffset));
	serverData.Update(*(uintptr_t *)(Address + DATA_OFFSET_INGAMESTATE_SERVERDATA + Offsets::Instance->IgsOffset));
	uiRootPtr = *(uintptr_t *)(Address + 0x4A0 + Offsets::Instance->IgsOffset);
	uiHoverTooltipPtr = *(uintptr_t *)(Address + 0x4D8 + Offsets::Instance->IgsOffset);
	currentUIElementPosX = *(float *)(Address + 0x4E0 + Offsets::Instance->IgsOffset);
	currentUIElementPosY = *(float *)(Address + 0x4E4 + Offsets::Instance->IgsOffset);
	uiHoverPtr = *(uintptr_t *)(Address + 0x4E8 + Offsets::Instance->IgsOffset);
	uiHoverX = *(float *)(Address + 0x524 + Offsets::Instance->IgsOffset);
	uiHoverY = *(float *)(Address + 0x528 + Offsets::Instance->IgsOffset);
	timeInGameF = *(float *)(Address + 0x554 + Offsets::Instance->IgsOffset);
	cameraPtr = *(uintptr_t *)(Address + 0xF58 + Offsets::Instance->IgsOffset);
}