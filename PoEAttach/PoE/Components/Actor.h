#pragma once

#include <vector>
#include "Component.h"
#include "ActorSkill.h"

class Actor : public Component
{
public:
	Actor();
	void	Update(uintptr_t addrPointer);
	std::vector<ActorSkill>	ReadActorSkills(uintptr_t startAddress, uintptr_t endAddress);

	enum ActionFlags
	{
		None = 0,
		UsingAbility = 2,

		//Actor is currently playing the "attack" animation, and therefor locked in a cooldown before any other action.
		AbilityCooldownActive = 16,
		Dead = 64,
		Moving = 128,

		/// actor is in the washed up state and false otherwise.
		WashedUpState = 256,
		HasMines = 2048
	};

	WORD		actionID;
	bool	isMoving;
	bool	isAttacking;
	std::vector<ActorSkill>	actorSkills;
};