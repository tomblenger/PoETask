#pragma once
#include "RemoteMemoryObject.h"

class Buff : public RemoteMemoryObject
{
public:
	Buff();
	void	Update(uintptr_t addrPointer);

	std::wstring	name;
	BYTE			charges;
	float			maxTime;
	float			timer;
};