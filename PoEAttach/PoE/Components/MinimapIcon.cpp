#include "stdafx.h"
#include "MinimapIcon.h"
#include "Common.h"
#include "Patch.h"
#include "WorldItemData.h"

MinimapIcon::MinimapIcon()
{
	isValid = false;
	canGoto = 1;
}

bool MinimapIcon::Update(uintptr_t addrPointer)
{
	Address = addrPointer;
	if (!Address)
		return false;

	isValid = *(BYTE *)(Address + DATA_MINIMAPICON_ISVALID);
	if (!isValid)
		return false;

	iconName = ReadStringU(**(uintptr_t **)(Address + 0x08));
	if (*(uintptr_t *)(Address + DATA_MINIMAPICON_POSOFFSET1))
	{
		pos.Update(*(uintptr_t *)(*(uintptr_t *)(Address + DATA_MINIMAPICON_POSOFFSET1) + DATA_MINIMAPICON_POSOFFSET2));
	}
	else
	{
		pos.gridPos.x = *(int *)(Address + DATA_MINIMAPICON_POSOFFSET3);
		pos.gridPos.y = *(int *)(Address + DATA_MINIMAPICON_POSOFFSET3 + DATA_MINIMAPICON_POSOFFSET4);
	}

	if (iconName.compare(L"StashPlayer") == 0)
	{
		name = L"Stash";
	}
	else if (iconName.compare(L"Waypoint") == 0)
	{
		name = WAYPOINT_NAME;
	}
	else if (iconName.compare(L"NPC") == 0)	//Address + 0x28
	{
		if (*(uintptr_t *)(Address + 0x28))
		{
			name = ReadStringU(*(uintptr_t *)(*(uintptr_t *)(Address + 0x28) + 0x08));
		}
	}
	else if (iconName.compare(L"Entrance") == 0 || iconName.compare(L"Portal") == 0 || iconName.compare(L"EntrancePortal") == 0)	//Address + 0x58
	{
		if (*(uintptr_t *)(Address + 0x30))
		{
			OtherData entity;
			entity.Update(*(uintptr_t *)(*(uintptr_t *)(Address + 0x30) + 0x08));
			name = entity.render.name;
			path = entity.path;
		}
		else if (*(uintptr_t *)(Address + 0x58))
		{
			OtherData entity;
			entity.Update(*(uintptr_t *)(*(uintptr_t *)(Address + 0x58) + 0x08));
			name = entity.render.name;
			path = entity.path;
		}
	}
	else if (iconName.compare(L"QuestObject") == 0)	//Address + 0x30
	{
		if (*(uintptr_t *)(Address + 0x30))
		{
			OtherData entity;
			entity.Update(*(uintptr_t *)(*(uintptr_t *)(Address + 0x30) + 0x08));
			name = entity.render.name;
			path = entity.path;
		}
	}
	else if (iconName.compare(L"QuestItem") == 0)	//Address + 0x60
	{
		if (*(uintptr_t *)(Address + DATA_MINIMAPICON_POSOFFSET1))
		{
			WorldItemData worlditem;
			worlditem.Update(*(uintptr_t *)(Address + DATA_MINIMAPICON_POSOFFSET1));
			name = worlditem.item.displayName;
			path = worlditem.item.path;
		}
	}

	return true;
}

const MinimapIcon &  MinimapIcon::operator = (const MinimapIcon &other)
{
	this->Address = other.Address;
	this->iconName = other.iconName;
	this->isValid = other.isValid;
	this->pos = other.pos;
	if (!other.name.empty())
		this->name = other.name;
	if (!other.path.empty())
		this->path = other.path;

	return *this;
}