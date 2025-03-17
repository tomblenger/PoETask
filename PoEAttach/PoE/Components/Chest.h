#pragma once
#include "Component.h"

class Chest : public Component
{
public:
	Chest();
	void		Update(uintptr_t addrPointer);

	bool		isOpened;
	bool		isLocked;
// 	bool		isStrongbox;
// 	BYTE		quality;
// 	uintptr_t	strongboxData;
// 	bool		destroyingAfterOpen;
// 	bool		isLarge;
// 	bool		stompable;
// 	bool		openOnDamage;
// 	bool		openWhenDeamonsDie;
};