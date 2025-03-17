#include "stdafx.h"
#include "Entity.h"
#include "Common.h"
#include "Patch.h"


Entity::Entity()
{
	id = 0;
	//inventoryId = 0;
	//isValid = false;
	//isHostile = false;
}

std::wstring Entity::MetaData()
{
	size_t splitIndex = (int)path.find(L"@");
	if (splitIndex != std::string::npos)
		return path.substr(0, splitIndex);
	return path;
}

std::map<std::string, uintptr_t> Entity::GetComponents()
{
	std::map<std::string, uintptr_t> dictionary;
	if (!Address || !*(uintptr_t *)(Address + DATA_OFFSET_ENTITY_COMPONENT1) || !*(uintptr_t *)(*(uintptr_t *)(Address + DATA_OFFSET_ENTITY_COMPONENT1) + DATA_OFFSET_ENTITY_COMPONENT2))
		return dictionary;

	uintptr_t componentLookup = *(uintptr_t *)(*(uintptr_t *)(*(uintptr_t *)(Address + DATA_OFFSET_ENTITY_COMPONENT1) + DATA_OFFSET_ENTITY_COMPONENT2) + 0x30);
	if (!componentLookup)
		return dictionary;
	// the first address is a base object that doesn't contain a component, so read the first component
	uintptr_t addr = *(uintptr_t *)(componentLookup);
	while (addr != componentLookup && addr != 0 && addr != -1)
	{
		std::string name = ReadString(*(uintptr_t *)(addr + 0x10));
		std::string nameStart = name;
		//if (!string.IsNullOrWhiteSpace(name))
		//{
		//	char[] arr = name.ToCharArray();
		//	arr = Array.FindAll(arr, (c = > (char.IsLetterOrDigit(c) || char.IsWhiteSpace(c) || c == '-')));
		//	name = new string(arr);
		//}
		//if (String.IsNullOrWhiteSpace(name) || name != nameStart)
		//{
		//	break;
		//}
		uintptr_t ComponentList = *(uintptr_t *)(Address + DATA_OFFSET_ENTITY_COMPONENT1 + 0x8);
		if (!ComponentList)
			return dictionary;
		uintptr_t componentAddress = *(uintptr_t *)(ComponentList + *(int *)(addr + 0x18) * 8);
		if (dictionary.count(name) == 0 && !name.empty())
			dictionary[name] = componentAddress;
		addr = *(uintptr_t *)(addr);
	}
	return dictionary;
}

bool Entity::Update(uintptr_t addrPointer, bool onlyPath/* = false*/)
{
	this->Address = addrPointer;
	if (!Address)
		return false;

	this->components = GetComponents();
	this->path = ReadNativeString(*(uintptr_t *)(Address + DATA_OFFSET_ENTITY_COMPONENT1) + 0x18);
	if (!onlyPath)
	{
		/// 0x65004D = "Me"(4 bytes) from word Metadata
		//this->isValid = this->Address != 0 && **(int **)(this->Address + 0x18) == 0x65004D;
		this->id = GetID(Address);// << 32 ^ Address;
		//this->inventoryId = *(int *)(Address + 0x58);
		//positionedComp.Update(*(uintptr_t *)(Address + 0x50));
		//isHostile = (positionedComp.reaction & 0x7F) != 1;
	}

	return true;
}