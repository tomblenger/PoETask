#include "stdafx.h"
#include "GameController.h"
#include "GameStateController.h"
#include "EntityListWrapper.h"
#include "FsController.h"
#include "GameFunc.h"

GameController *GameController::Instance = nullptr;

GameController::GameController()
{
	Instance = this;
	stateController = new GameStateController;
	entityList = new EntityListWrapper(this);
	//files = new FsController();
}

void GameController::Update()
{
	stateController->Update();
	entityList->Update();
	UpdateInventory();
	UpdateQuestStates();
}

void GameController::UpdateInventory()
{
	playerInventories.clear();
	npcInventories.clear();

	for (size_t i = 0; i < stateController->InGameState.serverData.playerInventories.size(); i++)
	{
		InventoryHolder inven;
		inven.Update(stateController->InGameState.serverData.playerInventories[i]);
		playerInventories.push_back(inven);
	}

	for (size_t i = 0; i < stateController->InGameState.serverData.npcInventories.size(); i++)
	{
		InventoryHolder npcInven;
		npcInven.Update(stateController->InGameState.serverData.npcInventories[i]);
		npcInventories.push_back(npcInven);
	}
}

void GameController::UpdateQuestStates()
{
	for (size_t i = 0; i < stateController->InGameState.ingameUi.questData.quests.size(); i++)
	{
		Quest *pQuest = &stateController->InGameState.ingameUi.questData.quests[i];
		auto addr = GetQuestStatePtr(GameController::Instance->entityList->localPlayer.player.Address, pQuest->Address);
		if (pQuest->state.Update(addr))
		{
			pQuest->step = pQuest->state.questStateId;
			if (pQuest->step == 0 || pQuest->state.questProgressText.find(QUEST_COMPLETE_TEXT) != -1)
				pQuest->isCompleted = true;
		}
	}
}
