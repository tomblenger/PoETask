#pragma once

#include "Component.h"

class Armour : public Component
{
public:
	Armour();
	void Update(uintptr_t addrPointer);

	int	EvasionScore;
	int AmourScore;
	int EnergyShieldScore;
};
