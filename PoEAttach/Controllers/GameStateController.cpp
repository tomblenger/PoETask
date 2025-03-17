#include "stdafx.h"
#include <stack>
#include "GameStateController.h"
#include "Common.h"

GameStateController *GameStateController::Instance = nullptr;
uintptr_t GameStateController::PreGameStatePtr = 0;
uintptr_t GameStateController::LoginStatePtr = 0;
uintptr_t GameStateController::SelectCharacterStatePtr = 0;
uintptr_t GameStateController::WaitingStatePtr = 0;
uintptr_t GameStateController::InGameStatePtr = 0;
AreaLoadingState GameStateController::LoadingState;
IngameState GameStateController::InGameState;
SelectCharacterState GameStateController::SelectCharacter;


GameStateController::GameStateController()
{
	Instance = this;
	if (!Offsets::Instance->GameStateOffset)
		return;

	//UpdateStates();
}

std::map<std::wstring, GameState> GameStateController::ReadHashMap(uintptr_t pointer)
{
	std::map<std::wstring, GameState> result;
	std::stack<GameStateHashNode *> stack;

	auto startNode = RemoteMemoryObject::ReadObject<GameStateHashNode>(pointer);
	auto item = startNode->Root();
	stack.push(item);

	int stackCounter = 100;
	while (stack.size() != 0 && stackCounter-- > 0)
	{
		auto node = stack.top();
		stack.pop();

		if (!node->IsNull())
			result[node->key] = node->value1;

		auto prev = node->Previous();
		if (!prev->IsNull())
			stack.push(prev);

		auto next = node->Next();
		if (!next->IsNull())
			stack.push(next);

		delete node;
	}

	delete startNode;

	return result;
}

bool GameStateController::GameStateActive(uintptr_t stateAddress)
{
	if (!Instance)
		return false;

	auto address = Instance->Address + 0x20;
	auto start = *(uintptr_t *)address;
	auto last = *(uintptr_t *)(address + 0x10);

	for (uintptr_t addr = start; addr < last; addr += 0x10)
	{
		if (*(uintptr_t *)addr == stateAddress)
			return true;
	}
	return false;
}

void GameStateController::Update()
{
	UpdateStates();
	InGameState.Update();
}

void GameStateController::UpdateStates()
{
	Address = *(uintptr_t *)Offsets::Instance->GameStateOffset;
	AllGameStates = ReadHashMap(Address + 0x48);
	//for each (auto state in AllGameStates)
	//{
	//	Log(L"%s: 0x%p %d", state.first.c_str(), state.second.Address, GameStateActive(state.second.Address) ? 1 : 0);
	//}
	PreGameStatePtr = AllGameStates[L"PreGameState"].Address;
	LoginStatePtr = AllGameStates[L"LoginState"].Address;
	SelectCharacterStatePtr = AllGameStates[L"SelectCharacterState"].Address;
	WaitingStatePtr = AllGameStates[L"WaitingState"].Address;
	InGameStatePtr = AllGameStates[L"InGameState"].Address;
	LoadingState.Address = AllGameStates[L"AreaLoadingState"].Address;
	InGameState.Address = InGameStatePtr;
}
