#include "stdafx.h"
#include "Life.h"
#include "Common.h"

Life::Life()
{
	maxHP = 1;
	reservedFlatHP = 0;
	curHP = 1;
	reservedPercentHP = 0;
	maxMana = 1;
	reservedFlatMana = 0;
	curMana = 1;
	reservedPercentMana = 0;
	maxES = 0;
	curES = 0;
 	hpPercentage = 0;
 	mpPercentage = 0;
 	esPercentage = 0;
	buffStart = 0;
	buffEnd = 0;
	maxBuffCount = 512;
}

void Life::Update(uintptr_t addrPointer)
{
	Address = addrPointer;
	if (!Address)
		return;

	maxHP = *(int *)(Address + 0x154);
	reservedFlatHP = *(int *)(Address + 0x158);
	curHP = *(int *)(Address + 0x15C);
	reservedPercentHP = *(int *)(Address + 0x160);
	maxMana = *(int *)(Address + 0xBC);
	reservedFlatMana = *(int *)(Address + 0xC0);
	curMana = *(int *)(Address + 0xC4);
	reservedPercentMana = *(int *)(Address + 0xC8);
	maxES = *(int *)(Address + 0xF4);
	curES = *(int *)(Address + 0xFC);
  	hpPercentage = curHP / (float)(maxHP - reservedFlatHP - round(reservedPercentHP * 0.01 * maxHP));
  	mpPercentage = curMana / (float)(maxMana - reservedFlatMana - round(reservedPercentMana * 0.01 * maxMana));
  	esPercentage = maxES != 0 ? curES / (float)maxES : 0;
	buffStart = *(uintptr_t *)(Address + 0x80);
	buffEnd = *(uintptr_t *)(Address + 0x88);

 	buffs.clear();
 	if (buffEnd - buffStart > 0 && buffEnd - buffStart < maxBuffCount * 8)
 	{
		for (uintptr_t addr = buffStart; addr < buffEnd; addr += 0x08)
		{
			if (*(uintptr_t *)addr != 0)
			{
				Buff buff;
				buff.Update(*(uintptr_t *)(*(uintptr_t *)addr + 0x08));
				buffs.push_back(buff);
			}
		}
	}
}