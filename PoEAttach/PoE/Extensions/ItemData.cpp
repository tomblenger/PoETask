#include "stdafx.h"
#include "ItemData.h"
#include "Common.h"
#include "ItemResource.h"

bool ItemData::Update(uintptr_t address)
{
	if (!Entity::Update(address, true))
		return false;

	if (components.count("Mods") > 0)
	{
		mods.Update(components["Mods"]);
	}
	if (components.count("Charges") > 0)
	{
		charges.Update(components["Charges"]);
	}
	if (components.count("Stack") > 0)
	{
		stack.Update(components["Stack"]);
	}
	if (components.count("Sockets") > 0)
	{
		itemSocket.Update(components["Sockets"]);
	}
	if (components.count("SkillGem") > 0)
	{
		skillGem.Update(components["SkillGem"]);
	}
	if (components.count("Armour") > 0)
	{
		armour.Update(components["Armour"]);
	}
	if (components.count("AttributeRequirements") > 0)
	{
		attribute.Update(components["AttributeRequirements"]);
	}
	if (components.count("Weapon") > 0)
	{
		weapon.Update(components["Weapon"]);
	}
	if (components.count("Quality") > 0)
	{
		quality.Update(components["Quality"]);
	}
	if (components.count("Base"))
	{
		base.update(components["Base"]);
	}

	GetItemDetailInfo();

	return true;
}

void ItemData::GetItemDetailInfo()
{
	if (mods.struniqueName != L"")
	{
		displayName = mods.struniqueName + L" " + base.ItemName;
	}
	else
	{
		if (quality.ItemQuality > 0)
		{
			displayName = L"Superior " + base.ItemName;
		}
		else
		{
			displayName = base.ItemName;
		}
	}

	itemWidth = base.ItemCellsSizeX;
	itemHeight = base.ItemCellsSizeY;
}

void ItemData::GetItemInfoFromResource(std::wstring strItemPath)
{
	auto ItemInfoPath = GetItemResource(strItemPath);

	if (ItemInfoPath != NULL)
	{
		itemName = ItemInfoPath->szName;

		if (mods.struniqueName != L"")
		{
			displayName = mods.struniqueName + L" " + itemName;
		}
		else
		{
			if (quality.ItemQuality > 0)
			{
				displayName = L"Superior " + itemName;
			}
			else
			{
				displayName = itemName;
			}
		}
		
		itemWidth = ItemInfoPath->bCellWidth;
		itemHeight = ItemInfoPath->bCellHeight;
	}
}