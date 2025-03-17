#pragma once

#include <vector>
#include "Entity.h"
#include "PlayerData.h"
#include "NpcData.h"
#include "MonsterData.h"
#include "WorldItemData.h"
#include "ChestsData.h"
#include "OtherData.h"

class GameController;

class EntityListWrapper
{
public:
	EntityListWrapper(GameController *controller);
	void Update();
	void UpdatePlayer();
	void UpdateEntities();

	GameController *gameController;
	std::vector<Entity> entityCache;
	std::vector<PlayerData> players;
	std::vector<NpcData> npcs;
	std::vector<MonsterData> monsters;
	std::vector<WorldItemData> worldItems;
	std::vector<ChestsData> chests;
	std::vector<OtherData> others;
	PlayerData localPlayer;
};