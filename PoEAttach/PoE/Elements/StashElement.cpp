#include "stdafx.h"
#include "StashElement.h"

StashElement::StashElement()
{
// 	totalStashes = 0;
// 	hasScrollBar = false;
	indexVisibleStash = 0;
}

// void StashElement::GetVisibleStash()
// {
// 	visibleStashs.clear();
// 	for (int i = 0; i < totalStashes; i++)
// 	{
// 		if(stashInventoryPanel.children[i].childCount == 0)
// 			continue;
// 		Inventory inventory;
// 		inventory.Update(stashInventoryPanel.children[i].children[0].children[0].Address);
// 		inventory.index = i;
// 		visibleStashs.push_back(inventory);
// 	}
// }

void StashElement::Update(uintptr_t addrPointer)
{
	if (!Element::Update(addrPointer))
		return;

//   	stashInventoryPanel.Update(*(uintptr_t *)(*(uintptr_t *)(Address + 0x2D8) + 0x438));
//   	totalStashes = stashInventoryPanel.childCount;
//   	hasScrollBar = totalStashes > 30;
//  	exitButton.Update(*(uintptr_t *)(Address + 0x2B8));	//or (10, A88) or (708, AA0) or (998, 100) or better (AA0, 708)
//  	stashTitlePanel.Update(*(uintptr_t *)(*(uintptr_t *)(Address + 0x2D8) + 0x428));
//  	viewAllStashButton.Update(*(uintptr_t *)(*(uintptr_t *)(Address + 0x2D8) + 0x440));
//  	viewAllStashPanel.Update(*(uintptr_t *)(*(uintptr_t *)(Address + 0x2D8) + 0x448));
//  	moveStashTabLabelsLeft_Button.Update(*(uintptr_t *)(*(uintptr_t *)(Address + 0x2D8) + 0x450));
//  	moveStashTabLabelsRight_Button.Update(*(uintptr_t *)(*(uintptr_t *)(Address + 0x2D8) + 0x458));
 	indexVisibleStash = *(int *)(*(uintptr_t *)(Address + 0x2D8) + 0x480);

//	Log(L"-- %d %d %f %f %f %f %f", isVisible, indexVisibleStash, x, y, scale, width, height);

// 	GetVisibleStash();
// 	for (int i = 0; i < visibleStashs.size(); i++)
// 	{
// 		Log(L"%d %d %d %d", visibleStashs[i].index, visibleStashs[i].itemCount, visibleStashs[i].totalBoxesInInventoryRow, visibleStashs[i].invType);
// 	}
}