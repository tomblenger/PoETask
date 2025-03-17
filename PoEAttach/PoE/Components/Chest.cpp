#include "stdafx.h"
#include "Chest.h"

Chest::Chest()
{
	isOpened = false;
	isLocked = false;
// 	isStrongbox = false;
// 	quality = 0;
// 	strongboxData = 0;
// 	destroyingAfterOpen = false;
// 	isLarge = false;
// 	stompable = false;
// 	openOnDamage = false;
// 	openWhenDeamonsDie = false;
}

void Chest::Update(uintptr_t addrPointer)
{
	Address = addrPointer;
	if (!Address)
		return;

	isOpened = *(BYTE *)(Address + 0x78) == 1;
	isLocked = *(BYTE *)(Address + 0x79) > 0;
// 	isStrongbox = *(BYTE *)(Address + 0x98) > 0;
// 	quality = *(BYTE *)(Address + 0x5C);
// 	strongboxData = *(uintptr_t *)(Address + 0x20);
// 	destroyingAfterOpen = *(BYTE *)(strongboxData + 0x20) == 1;
// 	isLarge = *(BYTE *)(strongboxData + 0x21) == 1;
// 	stompable = *(BYTE *)(strongboxData + 0x22) == 1;
// 	openOnDamage = *(BYTE *)(strongboxData + 0x25) == 1;
// 	openWhenDeamonsDie = *(BYTE *)(strongboxData + 0x28) == 1;
}