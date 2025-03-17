#pragma once

#include "Component.h"

class ItemStack : Component
{
public:
	ItemStack();
	void Update(uintptr_t addrPointer);

	int Size;
	int MaxStackSize;
};