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

	flag1 = *(BYTE *)(Address + 0x120);		//0:�ۿ�Ұ� 1: �ۿ밡�� 2:�̹� �ۿ��
	flag2 = *(BYTE *)(Address + 0x124);
}