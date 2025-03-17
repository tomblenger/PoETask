#pragma once

#include "Component.h"

class Weapon : public Component
{
public:
	Weapon();
	void	Update(uintptr_t addrPointer);

	int		DamageMin;
	int		DamageMax;
	int		CritChance;
};