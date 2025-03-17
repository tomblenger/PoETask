#pragma once

#include <vector>
#include <string>
#include "RemoteMemoryObject.h"

class Element : public RemoteMemoryObject
{
protected:
	void					GetChildren(uintptr_t Address);
	bool					IsVisibleElement(uintptr_t parent);

public:
	Element();
	bool					Update(uintptr_t addrPointer);	

	const int				offsetBuffers = 0;
	const int				childOffset = 0x38;

	uintptr_t				childCount;
	bool					isVisibleLocal;
	uintptr_t				root;
	uintptr_t				parent;
	float					x;
	float					y;
	float					scale;
	float					width;
	float					height;
	bool					isHighlighted;
	std::vector<Element>	children;
	bool					isVisible;
};

