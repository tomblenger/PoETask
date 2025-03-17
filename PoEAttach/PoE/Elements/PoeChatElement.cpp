#include "stdafx.h"
#include "PoeChatElement.h"

PoeChatElement::PoeChatElement()
{
	totalMessageCount = 0;
}

void PoeChatElement::Update(uintptr_t addrPointer)
{
	if (!Element::Update(addrPointer))
		return;

	totalMessageCount = childCount;
 	GetChildren(Address);
}