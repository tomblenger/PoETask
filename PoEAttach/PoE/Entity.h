#pragma once

#include <map>
#include "RemoteMemoryObject.h"
#include "Patch.h"
//#include "Positioned.h"

class Entity : public RemoteMemoryObject
{
public:
	Entity();
	std::wstring MetaData();
	std::map<std::string, uintptr_t> GetComponents();
	virtual bool Update(uintptr_t address, bool onlyPath = false);

	inline static UINT GetID(uintptr_t addr) { return *(UINT *)(addr + DATA_OFFSET_ENTITY_ID); }
	
	
	std::wstring path;
	UINT id;
	std::map<std::string, uintptr_t> components;
	//int inventoryId;
	//bool isValid;
	//Positioned positionedComp;
	//bool isHostile;

};