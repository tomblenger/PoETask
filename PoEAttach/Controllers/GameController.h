#pragma once

#include "InventoryHolder.h"

class GameStateController;
class EntityListWrapper;
class FsController;

class GameController
{
public:
	GameController();
	void Update();
	void UpdateInventory();
	void UpdateQuestStates();

	static GameController *Instance;

	GameStateController *stateController;
	EntityListWrapper *entityList;
	FsController * files;
	std::vector<InventoryHolder> playerInventories;
	std::vector<InventoryHolder> npcInventories;
};

