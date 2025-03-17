#pragma once
#include <map>
#include <string>
#include "RemoteMemoryObject.h"
#include "Offsets.h"
#include "IngameState.h"
#include "SelectCharacterState.h"

class GameState : public RemoteMemoryObject
{
public:
	void Update() { stateName = ReadNativeString(Address + 0x10); }

	std::wstring stateName;
};

class GameStateHashNode : public RemoteMemoryObject
{
public:
	static GameStateHashNode * ReadObject(uintptr_t address) 
	{ 
		auto result = RemoteMemoryObject::ReadObject<GameStateHashNode>(address);  
		if (!result->IsNull())
			result->Update(); 
		return result; 
	}

	inline GameStateHashNode * Previous() { return ReadObject(Address); }
	inline GameStateHashNode * Root() { return ReadObject(Address + 0x8); }
	inline GameStateHashNode * Next() { return ReadObject(Address + 0x10); }
	inline bool IsNull() { return Address != 0 && *(BYTE *)(Address + 0x19) != 0; }

	void Update()
	{
		key = ReadNativeString(Address + 0x20);
		value1.Address = *(uintptr_t *)(Address + 0x40);
		value1.Update();
	}
	
	std::wstring key;
	GameState value1;
};

class AreaLoadingState : public GameState
{
public:
	inline bool IsLoading() { return *(uintptr_t *)(Address + 0xD8) == 1; }
	inline std::wstring AreaName() { return ReadStringU(*(uintptr_t *)(Address + 0x1F0)); }
};

class GameStateController : public RemoteMemoryObject
{
public:
	GameStateController();
	std::map<std::wstring, GameState> ReadHashMap(uintptr_t pointer);
	void Update();
	void UpdateStates();

	static bool GameStateActive(uintptr_t stateAddress);
	static bool IsPreGame() { return GameStateActive(PreGameStatePtr); }
	static bool IsLoginState() { return GameStateActive(LoginStatePtr); }
	static bool IsSelectCharacterState() { return GameStateActive(SelectCharacterStatePtr); }
	static bool IsWaitingState() { return GameStateActive(WaitingStatePtr); }//This happens after selecting character, maybe other cases
	static bool IsInGameState() { return GameStateActive(InGameStatePtr) && !IsLoading(); }//In game, with selected character
	static bool IsLoading() { return LoadingState.IsLoading(); }

	static GameStateController *Instance;
	static uintptr_t PreGameStatePtr;
	static uintptr_t LoginStatePtr;
	static uintptr_t SelectCharacterStatePtr;
	static uintptr_t WaitingStatePtr;
	static uintptr_t InGameStatePtr;
	static AreaLoadingState LoadingState;
	static IngameState InGameState;
	static SelectCharacterState SelectCharacter;

	std::map<std::wstring, GameState> AllGameStates;
};