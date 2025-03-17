#pragma once

#include "Component.h"

class SkillGem : Component
{
public:
	SkillGem();
	void	Update(uintptr_t addrPointer);

	int		GemLevel;
	int		CurrentGemExp;
	int		GemExpPerlevel;
	int		MaxGemExp;
};
