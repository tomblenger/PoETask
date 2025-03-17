#pragma once
#include "Component.h"

class Npc : public Component
{
public:
	Npc();
	void	Update(uintptr_t addrPointer);

	bool	hasIconOverhead;
	bool	isIgnoreHidden;
	bool	isMinimapLabelVisible;
};