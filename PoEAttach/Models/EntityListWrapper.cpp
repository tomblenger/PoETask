#include "stdafx.h"
#include <queue>
#include <set>
#include "EntityListWrapper.h"
#include "GameController.h"
#include "Entity.h"
#include "GameStateController.h"

EntityListWrapper::EntityListWrapper(GameController *controller)
{
	gameController = controller;

}

void EntityListWrapper::Update()
{
	UpdatePlayer();
	UpdateEntities();

	players.clear();
	npcs.clear();
	monsters.clear();
	worldItems.clear();
	chests.clear();
	others.clear();

	for (int i = 0; i < entityCache.size(); i++)
	{
		if (entityCache[i].path.find(L"Character") != -1)
		{
			PlayerData player;
			player.Update(entityCache[i].Address);
			if (player.id != localPlayer.id)
			{
				players.push_back(player);
			}
		}
		else if (entityCache[i].path.find(L"NPC") != -1)
		{
			if (entityCache[i].path.find(L"Metadata/NPC/Act3/ScionCage") != -1)
			{
				OtherData other;
				other.Update(entityCache[i].Address);
				others.push_back(other);
			}
			else
			{
				NpcData npc;
				npc.Update(entityCache[i].Address);
				npcs.push_back(npc);
			}
		}
		else if (entityCache[i].path.find(L"Monster") != -1)
		{
			MonsterData monster;
			monster.Update(entityCache[i].Address);
			
			if (entityCache[i].path.find(L"Metadata/Monsters/Masters/Einhar@") != -1 || 
				entityCache[i].path.find(L"Metadata/Monsters/Masters/BlightBuilderWild@") != -1)
			{
				NpcData npc;
				npc.Update(entityCache[i].Address);
				npc.name = monster.name;
				npcs.push_back(npc);
			}
			else if (entityCache[i].path == L"Metadata/Monsters/IncaShadowBoss/IncaBossSpawner")
			{
				OtherData other;
				other.Update(entityCache[i].Address);
				others.push_back(other);
			}
			else
			{
				if (monster.life.curHP > 0)
				{
					monsters.push_back(monster);
				}
			}
		}
		else if (entityCache[i].path.find(L"WorldItem") != -1)
		{
			WorldItemData worlditem;
			worlditem.Update(entityCache[i].Address);
			worldItems.push_back(worlditem);
		}
		else if (entityCache[i].path.find(L"Chests") != -1)
		{
			ChestsData chest;
			chest.Update(entityCache[i].Address);
			chests.push_back(chest);

		}
		else //other
		{
			OtherData other;
			other.Update(entityCache[i].Address);
			others.push_back(other);
		}		
	}
}

void EntityListWrapper::UpdatePlayer()
{
	localPlayer.Update(GameStateController::InGameState.data.localPlayerPtr);
}

void EntityListWrapper::UpdateEntities()
{
	entityCache.clear();

	auto listPtr = GameStateController::InGameState.data.entityListPtr;
	if (!listPtr || !*(uintptr_t *)listPtr)
		return;
	listPtr = *(uintptr_t *)listPtr;

	// # - EntityList.CollectEntities
	auto addr = *(uintptr_t *)(listPtr + 8);		// Root
	std::vector<uintptr_t> hashSet;
	std::queue<uintptr_t> queuePtr;
	std::vector<UINT> ids;

	queuePtr.push(addr);
	while (queuePtr.size() > 0)
	{
		auto nextAddr = queuePtr.front();
		queuePtr.pop();
		if (std::find(hashSet.begin(), hashSet.end(), nextAddr) != hashSet.end())
			continue;

		hashSet.push_back(nextAddr);
		if (nextAddr != listPtr && nextAddr != 0)
		{
			queuePtr.push(*(uintptr_t *)nextAddr);			// Previous
			queuePtr.push(*(uintptr_t *)(nextAddr + 0x10));	// Next

			auto address = *(uintptr_t *)(nextAddr + 0x28);
			if (!address)
				continue;
			auto entityID = Entity::GetID(address);
			if (std::find(ids.begin(), ids.end(), entityID) != ids.end())
				continue;

			Entity entity;
			entity.Update(address);
			entityCache.push_back(entity);
		}
	}
}
