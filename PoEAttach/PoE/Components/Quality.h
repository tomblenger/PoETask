#pragma once

#include "Component.h"

class Quality : public Component
{
public:
	Quality();
	void	Update(uintptr_t addrPointer);

	int		ItemQuality;
};
