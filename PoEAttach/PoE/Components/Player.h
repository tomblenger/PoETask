#pragma once
#include "Component.h"

class Player : public Component
{
public:
	Player();
	void	Update(uintptr_t addrPointer);

	std::wstring	name;
	int		xp;
	int		strength;
	int		dexterity;
	int		intelligence;
	int		level;
};