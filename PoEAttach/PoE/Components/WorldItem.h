#pragma once
#include "Entity.h"
#include "Component.h"

class WorldItem : public Component
{
public:
	void	Update(uintptr_t addrPointer);
	Entity	itemEntity;
};