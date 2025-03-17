#include "stdafx.h"
#include "Component.h"
#include "Entity.h"

Entity Component::Owner()
{
	Entity owner;
	owner.Update(*(uintptr_t *)(Address + 8));
	return owner;
}
