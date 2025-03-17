#include "stdafx.h"
#include "Element.h"

void Element::GetChildren(uintptr_t Address)
{
	children.clear();
	if (*(uintptr_t *)(Address + childOffset + 8) != 0 && *(uintptr_t *)(Address + childOffset) != 0 && childCount < 1000)
	{
		for (int i = 0; i < childCount; i++)
		{
			Element element;
			element.Update(*(uintptr_t *)(*(uintptr_t *)(Address + childOffset) + i * 8));
			children.push_back(element);
		}
	}
}

bool Element::IsVisibleElement(uintptr_t parent)
{
	uintptr_t addr = parent;
	
	while (addr)
	{		
		if ((*(BYTE *)(addr + 0x111) & 4) != 4)
			return false;

		if (*(uintptr_t *)(addr + 0x90 + offsetBuffers) == addr)
			break;
		addr = *(uintptr_t *)(addr + 0x90 + offsetBuffers);
		if (!addr)
			break;
	}
	return true;
}

Element::Element()
{
	childCount = 0;
	isVisibleLocal = false;
	root = 0;
	parent = 0;
	x = 0;
	y = 0;
	scale = 0;
	width = 0;
	height = 0;
	isHighlighted = false;
	isVisible = false;
}

bool Element::Update(uintptr_t addrPointer)
{
	Address = addrPointer;
	if (!Address)
		return false;

	childCount = (*(uintptr_t *)(Address + childOffset + 8 + offsetBuffers) - *(uintptr_t *)(Address + childOffset + offsetBuffers)) / 8;
	isVisibleLocal = (*(BYTE *)(Address + 0x111) & 4) == 4;
	root = *(uintptr_t *)(*(uintptr_t *)(Address + 0x88 + offsetBuffers) + 0xE8);
	parent = *(uintptr_t *)(Address + 0x90 + offsetBuffers);
	x = *(float *)(Address + 0x98 + offsetBuffers);
	y = *(float *)(Address + 0x9C + offsetBuffers);
	scale = *(float *)(Address + 0xB0 + offsetBuffers);	//0x108
	width = *(float *)(Address + 0x130 + offsetBuffers);
	height = *(float *)(Address + 0x134 + offsetBuffers); 	
 	isHighlighted = *(BYTE *)(Address + 0x178) > 0;
 	GetChildren(Address);

	isVisible = isVisibleLocal && IsVisibleElement(parent);

	return true;
}