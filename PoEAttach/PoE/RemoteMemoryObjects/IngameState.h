#pragma once

#include "RemoteMemoryObject.h"
#include "IngameData.h"
#include "ServerData.h"
#include "IngameUIElements.h"

class IngameState : public RemoteMemoryObject
{
public:
	IngameState();

	void Update();

	IngameUIElements ingameUi;
	uintptr_t entityLabelMap;
	IngameData data;
	ServerData serverData;
	uintptr_t uiRootPtr;
	uintptr_t uiHoverTooltipPtr;
	float currentUIElementPosX;
	float currentUIElementPosY;
	uintptr_t uiHoverPtr;
	float uiHoverX;
	float uiHoverY;
	float timeInGameF;
	uintptr_t cameraPtr;
};