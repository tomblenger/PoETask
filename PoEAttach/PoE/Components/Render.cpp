#include "stdafx.h"
#include "Render.h"

Render::Render()
{
	x = 0;
	y = 0;
	z = 0;
	pos.x = pos.y = pos.z = 0;
	bounds.x = bounds.y = bounds.z = 0;
	meshRoration.x = meshRoration.y = meshRoration.z = 0;;
	terrainHeight = 0;
}

void Render::Update(uintptr_t addrPointer)
{
	Address = addrPointer;
	if (!Address)
		return;

	pos.x = x = *(float *)(Address + 0x78);
	pos.y = y = *(float *)(Address + 0x7C);
	pos.z = z = *(float *)(Address + 0x80);

	name = ReadNativeString(Address + 0x98);

	bounds.x = *(float *)(Address + 0x84);
	bounds.y = *(float *)(Address + 0x88);
	bounds.z = *(float *)(Address + 0x8C);

	meshRoration.x = *(float *)(Address + 0xB8);
	meshRoration.y = *(float *)(Address + 0xBC);
	meshRoration.z = *(float *)(Address + 0xC0);

	terrainHeight = *(float *)(Address + 0xD4);
}