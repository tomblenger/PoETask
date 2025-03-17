#pragma once
#include "Element.h"
//#include "Inventory.h"

class StashElement : public Element
{
// protected:
// 	void GetVisibleStash();
public:
	StashElement();
	void		Update(uintptr_t addrPointer);

// 	uintptr_t	totalStashes;
// 	bool		hasScrollBar;
// 	Element		exitButton;
// 	Element		stashTitlePanel;
// 	Element		stashInventoryPanel;
// 	Element		viewAllStashButton;
// 	Element		viewAllStashPanel;
// 	Element		moveStashTabLabelsLeft_Button;
// 	Element		moveStashTabLabelsRight_Button;
	int			indexVisibleStash;

//	std::vector<Inventory>	visibleStashs;
};