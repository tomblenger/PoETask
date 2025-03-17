#include "stdafx.h"
#include "Transitionable.h"

Transitionable::Transitionable()
{
	flag1 = 0;
	flag2 = 0;
}

void Transitionable::Update(uintptr_t addrPointer)
{
	Address = addrPointer;
	if (!Address)
		return;

	flag1 = *(BYTE *)(Address + 0x120);		//0:작용불가 1: 작용가능 2:이미 작용됨
	flag2 = *(BYTE *)(Address + 0x124);
}