#pragma once

#include "Entity.h"
#include "Positioned.h"
#include "Render.h"
//#include "Targetable.h"
#include "Transitionable.h"

class OtherData : public Entity
{
public:
	bool			Update(uintptr_t address);

	Positioned		positionedComp;
	Render			render;
	Transitionable	transitionable;
//	Targetable		targetable;
};