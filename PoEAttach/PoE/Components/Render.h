#pragma once
#include "Component.h"

struct Vector3
{
	float	x;
	float	y;
	float	z;
};

class Render : public Component
{
public:
	Render();
	void			Update(uintptr_t addrPointer);

	float			x;
	float			y;
	float			z;
	Vector3			pos;
	std::wstring	name;
	Vector3			bounds;
	Vector3			meshRoration;
	float			terrainHeight;
};