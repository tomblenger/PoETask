#pragma once

#include "Component.h"
#include "Common.h"

class Positioned : public Component
{
public:
	Positioned();
	void Update(uintptr_t addrPointer);

	IVector		gridPos;
	FVector		worldPos;
	float		rotation;
	float		rotationDeg;
	BYTE		reaction;
};