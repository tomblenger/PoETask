#pragma once

#include "Component.h"
#include "Transitionable.h"

class Transitionable : public Component
{
public:
	Transitionable();
	void	Update(uintptr_t addrPointer);

	BYTE	flag1;
	BYTE	flag2;
};