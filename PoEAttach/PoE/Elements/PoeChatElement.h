#pragma once
#include "Element.h"

class PoeChatElement : public Element
{
public:
	PoeChatElement();
	void						Update(uintptr_t addrPointer);

	uintptr_t					totalMessageCount;
};