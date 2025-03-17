#pragma once
#include "Component.h"
#include "StatsResource.h"

class Stats : public Component
{
public:
	Stats();
	void			Update(uintptr_t addrPointer);

	int				id;
	int				val;
	std::wstring	name;
};