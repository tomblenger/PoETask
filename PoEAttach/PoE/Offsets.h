#pragma once

#include <string>
#include "Common.h"

class Offsets
{
public:
	Offsets();
	void DoPatternScans();
	inline int IgsOffsetDelta() { return IgsDelta + IgsOffset; }


	static Offsets *Instance;

	uintptr_t AreaChangeCount;
	uintptr_t Base;
	uintptr_t FileRoot;
	uintptr_t GameStateOffset;
	std::string ExeName;
	int IgsDelta;
	int IgsOffset;
};

