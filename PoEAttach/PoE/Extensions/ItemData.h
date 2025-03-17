#pragma once

#include "Entity.h"
#include "Mods.h"
#include "Charges.h"
#include "Stack.h"
#include "Sockets.h"
#include "SkillGem.h"
#include "Armour.h"
#include "AttributeRequirements.h"
#include "Weapon.h"
#include "Quality.h"
#include "Base.h"

class ItemData : public Entity
{
public:
	bool Update(uintptr_t address);
	void					GetItemDetailInfo();
	void					GetItemInfoFromResource(std::wstring strItemPath);

	Mods					mods;
	Charges					charges;
	ItemStack				stack;
	Sockets					itemSocket;
	SkillGem				skillGem;
	Armour					armour;
	AttributeRequirements	attribute;
	Weapon					weapon;
	Quality					quality;
	Base					base;

	std::wstring			displayName;
	std::wstring			uniqueName;
	std::wstring			itemName;
	BYTE					itemWidth;
	BYTE					itemHeight;
};