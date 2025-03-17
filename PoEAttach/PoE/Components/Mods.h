#pragma once

#include "Component.h"
#include "ItemMod.h"
#include <vector>

class Mods : public Component
{
public:
	Mods();
	void						Update(uintptr_t addrPointer);
	std::vector<ItemMod>		GetMods(DWORD dwStartOffset, DWORD dwEndOffset);

	std::wstring				struniqueName;
	bool						bIdentified;
	int							nItemRarity;
	int							nItemLevel;
	int							nItemRequireLevel;
	bool						bIsUsable;
	bool						bIsMirrored;

	std::vector<ItemMod>		implicitMods;
	std::vector<ItemMod>		explicitMods;
};