#pragma once
#include "Component.h"

class Targetable : public Component
{
public:
	Targetable();
	void	Update(uintptr_t addrPointer);

	bool	isTargetable;
	bool	isTargeted;
};