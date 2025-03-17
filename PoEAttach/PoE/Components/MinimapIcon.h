#pragma once

#include "Component.h"
#include "Positioned.h"
#include "OtherData.h"

class MinimapIcon : public Component
{
public:
	MinimapIcon();
	bool	Update(uintptr_t addrPointer);
	const MinimapIcon &  operator = (const MinimapIcon &other);

	std::wstring	iconName;
	BYTE			isValid;
	Positioned		pos;
	std::wstring	name;
	std::wstring	path;
	BYTE			canGoto;
};