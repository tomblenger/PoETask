#include "stdafx.h"
#include "Skill.h"

DoSkillInfo g_pSkillList_Witch = 
{	//0:normal, 1:buff, 2:orb
	8,
	{
		{ 0x8883, 5, 0, L"HeraldOfThunder", 1, L"herald_of_thunder", 0 },
		{ 0x8004, 5, 0, L"BloodstainedBanner", 1, L"bloodstained_banner_buff_aura", 0 },
		{ 0x8009, 5, 0, L"Clarity", 1, L"player_aura_mana_regen", 0 },
		{ 0x8882, 50, 8, L"OrbOfStorms", 2, L"", 0 },
		{ 0x8008, 3, 7, L"ConduitSigil", 0, L"", 0 },
		{ 0x8402, 3, 7, L"FreezingPulse", 0, L"", 0 },
		{ 0x8002, 3, 7, L"Fireball", 0, L"", 0 },
		{ 0x4000, 3, 0, L"PlayerMelee", 0, L"", 0 },
	}
};

DoForwardSkillInfo g_pForwardSkillList_Witch =
{
	1,
	{
		{ 50, 13, L"IceDash", 0 },
	}
};
