#include "stdafx.h"
#include "IngameUIElements.h"
#include "Common.h"
#include "Patch.h"

void IngameUIElements::Update(uintptr_t addrPointer)
{
	Address = addrPointer;
	if (!Address)
		return;

 	skillBar.Update(*(uintptr_t *)(Address + 0x370));
	skillBarSkills.clear();
	for (int i = 0; i < 8; i++)
	{
		SkillElement skillelement;
		skillelement.Update(skillBar.children[i].Address);
		skillBarSkills.push_back(skillelement);
	}
	stashElement.Update(*(uintptr_t *)(Address + 0x520));
	questData.Update(*(uintptr_t *)(Address + 0x200));
	interaction.Update(*(uintptr_t *)(Address + 0x620));	//npc대화창 isVisible?childCount=2, childCount=3
	purchaseItem.Update(*(uintptr_t *)(Address + 0x638));	//npc구입창
	sellItem.Update(*(uintptr_t *)(Address + 0x640));	//npc판매창
	inventory.Update(*(uintptr_t *)(Address + 0x518));	//인벤
	reward.Update(*(uintptr_t *)(Address + 0x630));		//보상
	passiveSkillPanel.Update(*(uintptr_t *)(Address + 0x548));		//패시브스킬창문 sub_2DE7A0
	passiveSkillUp.Update(*(uintptr_t *)(Address + 0x330));		//패시브스킬업그레이드 아이콘
	skipTutorial.Update(*(uintptr_t *)(Address + DATA_OFFSET_SKIPTUTORIAL_LAYOUT));
	worldmap.Update(*(uintptr_t *)(Address + 0x580));	//텔포가능한 월드맵
	deadPanel.Update(*(uintptr_t *)(Address + 0x808));	//부활창문
	areaInstancePanel.Update(*(uintptr_t *)(Address + 0x7B8));	//혹은 0x5D0 	// 지역 인스턴스 창
	specialProcMonsterPanel.Update(*(uintptr_t *)(Address + 0x3D0));	//몬스터를 도와주기 또는 죽이기 선택창

	//Log(L"대화:%d/%d 상자:%d 보상:%d 패시브스킬패널:%d 스킬업그레이드:%d 월드맵:%d %p %p", interaction.isVisible, interaction.childCount, stashElement.isVisible,
	//	reward.isVisible, passiveSkillPanel.isVisible, passiveSkillUp.isVisible, worldmap.isVisible, worldmap.Address, worldmap.children[2].Address);

	teleportFieldIds.clear();
	getAllChild(worldmap.Address, 1);

// 	for (uintptr_t addr = Address; addr < Address + 0x1000; addr += 8)
// 	{
// 		if (((*(uintptr_t *)addr) >> 32 == 0x1E1))// || ((*(uintptr_t *)addr) >> 32 == 0x1C3))
// 		{
// 			Log(L"%p %08X %d", *(uintptr_t *)addr, addr - Address, *(BYTE *)(*(uintptr_t *)addr + 0x111) & 4);
// 			//Log(L"%p %08X", *(uintptr_t *)addr, addr - 0x0000016DA06AFE00);
// 		}
// 	}
}

void IngameUIElements::getAllChild(uintptr_t addr, int depth)
{	
	Element	element;
	if (element.Update(addr))
	{
		for (int i = 0; i < element.children.size(); i++)
		{
			if (depth == 12 && element.children.size() < 25 && element.children.size() > 7)
			{
				uintptr_t addr1 = element.children[i].Address;
				if (*(uintptr_t *)(addr1 + 0x138))
				{
					std::wstring strname = ReadStringU(*(uintptr_t *)(*(uintptr_t *)(addr1 + 0x138) + 0x48));
					if (strname.find(L"WorldPanelTownPinIcon") != -1 || strname.find(L"WorldPanelActivatedWaypointPinIcon") != -1)
					{
						teleportFieldIds.push_back(*(DWORD *)(*(uintptr_t *)(*(uintptr_t *)(addr1 + DATA_OFFSET_WORLDMAP_TELEPORT1) + DATA_OFFSET_WORLDMAP_TELEPORT2) + DATA_OFFSET_WORLDMAP_TELEPORT3));
					}
				}
				//Log(L"%d %p %p", depth, addr, element.children[i].Address);
			}
			if(depth < 12)
				getAllChild(element.children[i].Address, depth + 1);
		}
	}
}