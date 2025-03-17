#pragma once

#include "Component.h"

class AttributeRequirements : public Component
{
public:
	AttributeRequirements();
	void	Update(uintptr_t addrPointer);

	int		strength;
	int		dexterity;
	int		intelligence;
};
