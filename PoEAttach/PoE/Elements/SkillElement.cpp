#include "stdafx.h"
#include "SkillElement.h"

SkillElement::SkillElement()
{
	isValid = false;
	skillid = 0;
	addr = 0;
}

void SkillElement::Update(uintptr_t addrPointer)
{
	if (!Element::Update(addrPointer))
		return;

	addr = *(uintptr_t *)(Address + offsetBuffers + 0x230);	//unknown3
   	isValid = addr != 0;
 	if (isValid)
	{
		skillid = *(WORD *)(addr + 0x10);
		if (*(uintptr_t *)(addr + 0x20))
			skillName = ReadStringU(**(uintptr_t **)(*(uintptr_t *)(addr + 0x20) + 0x08));
	}
}