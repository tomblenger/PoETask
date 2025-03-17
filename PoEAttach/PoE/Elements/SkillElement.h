#pragma once
#include "Element.h"

class SkillElement : public Element
{
public:
	SkillElement();
	void			Update(uintptr_t addrPointer);

	bool			isValid;
	WORD			skillid;
	std::wstring	skillName;
	uintptr_t		addr;
};