#include "stdafx.h"
#include "Player.h"

Player::Player()
{
	xp = 0;
	strength = 0;
	dexterity = 0;
	intelligence = 0;
	level = 1;
}

void Player::Update(uintptr_t addrPointer)
{
	Address = addrPointer;
	if (!Address)
		return;

	name = ReadNativeString(Address + 0x58);
	xp = *(int *)(Address + 0x80);
	strength = *(int *)(Address + 0x84);
	dexterity = *(int *)(Address + 0x88);
	intelligence = *(int *)(Address + 0x8C);
	level = *(int *)(Address + 0x90);
}