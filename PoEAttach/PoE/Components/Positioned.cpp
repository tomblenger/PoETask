#include "stdafx.h"
#include "Positioned.h"
#include "Common.h"

Positioned::Positioned()
{
	gridPos.x = gridPos.y = 0;
	worldPos.x = worldPos.y = 0.f;
	rotation = 0.f;
	rotationDeg = 0.f;
	reaction = 0;
}

void Positioned::Update(uintptr_t addrPointer)
{
	Address = addrPointer;
	if (!Address)
		return;

	gridPos.x = *(int *)(Address + 0xEC);
	gridPos.y = *(int *)(Address + 0xF0);

	worldPos.x = *(float *)(Address + 0x118);
	worldPos.y = *(float *)(Address + 0x11C);

	rotation = *(float *)(Address + 0xF4);	// Radians, btw
	rotationDeg = rotation * 180.f / PI;

	reaction = *(BYTE *)(Address + 0x58);
}