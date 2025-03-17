#pragma once

#include <vector>
#include "Component.h"
#include "Buff.h"

class Life : public Component
{
public:
	Life();
	void				Update(uintptr_t addrPointer);

	int					maxHP;
	int					reservedFlatHP;
	int					curHP;
	int					reservedPercentHP;
	int					maxMana;
	int					reservedFlatMana;
	int					curMana;
	int					reservedPercentMana;
	int					maxES;
	int					curES;
	float				hpPercentage;
	float				mpPercentage;
	float				esPercentage;
	uintptr_t			buffStart;
	uintptr_t			buffEnd;
	int					maxBuffCount;
	std::vector<Buff>	buffs;
};