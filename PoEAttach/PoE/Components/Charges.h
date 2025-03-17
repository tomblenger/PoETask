#pragma once

#include "Component.h"


class Charges : public Component
{
public:
	Charges();

	void Update(uintptr_t	addrPointer);

	int	CurrentCharges;
	int ChargesPerUse;
	int ChargesMax;

};