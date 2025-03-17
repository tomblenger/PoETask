#pragma once

#include "Component.h"

class Base : public Component
{
public:
	Base();
	void			update(uintptr_t addrPointer);

	std::wstring	ItemName;
	int				ItemCellsSizeX;
	int				ItemCellsSizeY;
	bool			IsCorrupted;
	bool			IsShaper;
	bool			IsElder;
};
