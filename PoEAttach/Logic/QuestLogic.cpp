#include "stdafx.h"
#include "QuestLogic.h"
#include "GameLogic.h"
#include "Common.h"
#include "GameController.h"
#include "GameStateController.h"
#include "Offsets.h"
#include "EntityListWrapper.h"
#include "Entity.h"
#include "Patch.h"
#include "GameFunc.h"
#include "CalcPath.h"
#include "Skill.h"
#include "PoEAttach.h"
#include "ItemResource.h"
#include "IngameUIElements.h"
#include "WorldAreaResource.h"


std::vector<QuestInfo> g_pInfoQuest
{
	//Act1
	{ 0, 0, -1, L"Enemy at the Gate", QM_Hunt, 0, 0, L"Hungry Corpse", L"Dying Exile", L"The Twilight Strand" },
	{ 0, 0, 4, L"Enemy at the Gate", QM_Hunt, 0, 0, L"Hillock", L"", L"The Twilight Strand" },
	{ 0, 0, 3, L"Enemy at the Gate", QM_Reward, 0, 0, L"Tarkleigh", L"Freezing Pulse", L"Lioneye's Watch" },
	{ 0, 0, 1, L"Enemy at the Gate", QM_Reward, 0, 0, L"Tarkleigh", L"Freezing Pulse", L"Lioneye's Watch" },
	{ 0, 1, -1, L"Enemy at the Gate", QM_Interaction, IK_Npc, 0, L"Nessa", L"", L"Lioneye's Watch" },
	{ 0, 1, 5, L"Mercy Mission", QM_Interaction, IK_Npc, 0, L"Bestel", L"", L"Lioneye's Watch" },
	{ 0, 1, 4, L"Mercy Mission", QM_PickupItem, 0, 0, L"Medicine Chest", L"", L"The Tidal Island" },
	{ 0, 1, 3, L"Mercy Mission", QM_PickupItem, 0, 0, L"Medicine Chest", L"", L"The Tidal Island" },
	{ 0, 1, 2, L"Mercy Mission", QM_Reward, 0, 0, L"Nessa", L"Quicksilver Flask", L"Lioneye's Watch" },
	{ 1, 5, -1, L"A Dirty Job", QM_Reward, 0, 0, L"Nessa", L"Elemental Proliferation Support", L"Lioneye's Watch" },
	{ 1, 5, -1, L"A Dirty Job", QM_Interaction, IK_Npc, 0, L"Bestel", L"", L"Lioneye's Watch" },
	{ 0, 5, -1, L"A Dirty Job", QM_Interaction, IK_Npc, 0, L"Tarkleigh", L"", L"Lioneye's Watch" },
	{ 0, 5, 10, L"A Dirty Job", QM_FullScan, 0, 0, L"", L"", L"The Mud Flats" },
	{ 0, 5, 7, L"A Dirty Job", QM_FullScan, 0, 0, L"", L"", L"The Fetid Pool" },
	{ 0, 5, 6, L"A Dirty Job", QM_Hunt, 0, 0, L"", L"", L"The Fetid Pool" },
	{ 0, 5, 5, L"A Dirty Job", QM_Hunt, 0, 0, L"", L"", L"The Fetid Pool" },
	{ 0, 5, 4, L"A Dirty Job", QM_Hunt, 0, 0, L"", L"", L"The Fetid Pool" },
	{ 0, 5, 3, L"A Dirty Job", QM_Reward, 0, 0, L"Tarkleigh", L"Book of Regrets", L"Lioneye's Watch" },
	{ 0, 3, 7, L"Breaking Some Eggs", QM_ItemsAndWall, 0, 0, L"Strange Glyph Wall", L"Haliotis Glyph@Roseus Glyph@Ammonite Glyph", L"The Mud Flats" },
	{ 0, 3, 6, L"Breaking Some Eggs", QM_ItemsAndWall, 0, 0, L"Strange Glyph Wall", L"Haliotis Glyph@Roseus Glyph@Ammonite Glyph", L"The Mud Flats" },
	{ 0, 3, 4, L"Breaking Some Eggs", QM_ItemsAndWall, 0, 0, L"Strange Glyph Wall", L"Haliotis Glyph@Roseus Glyph@Ammonite Glyph", L"The Mud Flats" },
	{ 0, 3, 3, L"Breaking Some Eggs", QM_ItemsAndWall, 0, 0, L"Strange Glyph Wall", L"Haliotis Glyph@Roseus Glyph@Ammonite Glyph", L"The Mud Flats" },
	{ 0, 3, 2, L"Breaking Some Egg", QM_Reward, 0, 0, L"Tarkleigh", L"Clarity", L"Lioneye's Watch" },
	{ 0, 8, 5, L"The Caged Brute", QM_Interaction, IK_Npc, 0, L"Tarkleigh", L"", L"Lioneye's Watch" },
	{ 0, 8, 6, L"The Caged Brute", QM_Hunt, 0, 0, L"Brutus, Lord Incarcerator", L"", L"The Upper Prison", 2 },
	{ 0, 8, 4, L"The Caged Brute", QM_Hunt, 0, 0, L"Brutus, Lord Incarcerator", L"", L"The Upper Prison", 2 },
	{ 0, 8, 3, L"The Caged Brute", QM_Hunt, 0, 0, L"Brutus, Lord Incarcerator", L"", L"The Upper Prison", 2 },
	{ 0, 8, 1, L"The Caged Brute", QM_Reward, 0, 0, L"Tarkleigh", L"Orb of Storms", L"Lioneye's Watch" },
	{ 0, 6, 5, L"The Dweller of the Deep", QM_Hunt, 0, 0, L"The Dweller of the Deep", L"", L"The Flooded Depths" },
	{ 0, 6, 3, L"The Dweller of the Deep", QM_Interaction, IK_Npc, 0, L"Tarkleigh", L"", L"Lioneye's Watch" },
	{ 0, 13, -1, L"The Siren's Cadence", QM_Hunt, 0, 0, L"Merveil, the Siren", L"", L"The Cavern of Anger", 1 },
	{ 0, 13, 7, L"The Siren's Cadence", QM_Hunt, 0, 0, L"Merveil, the Siren", L"", L"The Cavern of Anger", 1 },
	{ 0, 13, 6, L"The Siren's Cadence", QM_Hunt, 0, 0, L"Merveil, the Siren", L"", L"The Cavern of Anger", 1 },
	{ 0, 13, 5, L"The Siren's Cadence", QM_Hunt, 0, 0, L"Merveil, the Twisted", L"", L"The Cavern of Anger", 1 },
	{ 0, 13, 3, L"The Siren's Cadence", QM_GotoArea, 0, 0, L"", L"", L"The Southern Forest",  },
	{ 0, 13, 1, L"The Siren's Cadence", QM_Reward, 0, 0, L"Nessa", L"Storm Burst", L"Lioneye's Watch" },
	{ 0, 15, 6, L"The Way Forward", QM_GotoArea, 0, 0, L"", L"", L"The Submerged Passage" },
	//{ 0, 15, 7, L"The Way Forward", QM_GotoArea, 0, 0, L"", L"", L"The Ship Graveyard" },
	//{ 0, 15, 5, L"The Way Forward", QM_GotoArea, 0, 0, L"", L"", L"The Southern Forest" },
	{ 0, 168, 3, L"Prisoner of Fate", QM_Interaction, IK_Npc, 0, L"Navali", L"", L"Lioneye's Watch" },
	// Act2
	{ 0, 16, 7, L"The Way Forward", QM_Interaction, IK_Npc, 0, L"Silk", L"", L"The Forest Encampment" },
	{ 0, 149, 4, L"Einhar's Hunt", QM_Interaction, IK_Npc, 0, L"Einhar, Beastmaster", L"", L"The Riverways" },
	{ 0, 149, 2, L"Einhar's Hunt", QM_FullScan, 0, 0, L"", L"", L"The Riverways" },
	{ 0, 149, 1, L"Einhar's Hunt", QM_Interaction, IK_Npc, 0, L"Einhar, Beastmaster", L"", L"The Riverways" },
	{ 0, 26, -1, L"Intruders in Black", QM_Interaction, IK_Npc, 0, L"Eramir", L"", L"The Forest Encampment" },
	{ 0, 26, 9, L"Deal with the Bandits", QM_InteractAndHunt, 0, 0, L"Alira Darktongue", L"", L"The Western Forest" },
	{ 0, 26, 6, L"Deal with the Bandits", QM_InteractAndHunt, 0, 0, L"Oak, Skullbreaker", L"", L"The Wetlands" },
	{ 0, 26, 5, L"Deal with the Bandits", QM_InteractAndHunt, 0, 0, L"Kraityn, Scarbearer", L"", L"The Broken Bridge" },
	{ 0, 26, 2, L"Deal with the Bandits", QM_Interaction, IK_Npc, 0, L"Eramir", L"", L"The Forest Encampment" },
	{ 0, 30, 9, L"Shadow of the Vaal", QM_Reward, 0, 0, L"Eramir", L"The Apex", L"The Forest Encampment" },
	{ 0, 16, 8, L"The Way Forward", QM_FullScan, 0, 0, L"", L"", L"The Western Forest" },	//
	{ 0, 16, 4, L"The Way Forward", QM_FullScan, 0, 0, L"", L"", L"The Western Forest" },
	{ 0, 16, 3, L"The Way Forward", QM_Hunt, 0, 0, L"Captain Arteri", L"", L"The Western Forest" },
	{ 0, 16, 1, L"The Way Forward", QM_Interaction, IK_Other, 0, L"Thaumetic Seal", L"", L"The Western Forest" },
	{ 0, 15, 3, L"The Way Forward", QM_Reward, 0, 0, L"Bestel", L"Book of Skill", L"Lioneye's Watch" },
	{ 0, 22, 4, L"The Great White Beast", QM_FullScan, 0, 0, L"", L"", L"The Den" },
	{ 0, 22, 3, L"The Great White Beast", QM_Hunt, 0, 0, L"The Great White Beast", L"", L"The Den" },
	{ 0, 22, 2, L"The Great White Beast", QM_Reward, 0, 0, L"Yeena", L"Quicksilver Flask", L"The Forest Encampment" },
	{ 0, 20, -1, L"Intruders in Black", QM_Interaction, IK_Npc, 0, L"Greust", L"", L"The Forest Encampment" },
	{ 0, 20, 6, L"Intruders in Black", QM_FullScan, 0, 0, L"", L"", L"The Chamber of Sins Level 2" },
	{ 0, 20, 5, L"Intruders in Black", QM_Hunt, 0, 0, L"", L"Fidelitas, the Mourning", L"The Chamber of Sins Level 2" },
	{ 0, 20, 4, L"Intruders in Black", QM_Interaction, IK_Npc, 0, L"Helena", L"", L"The Chamber of Sins Level 2" },
	{ 0, 20, 3, L"Intruders in Black", QM_PickupItem, 0, 0, L"Baleful Gem", L"", L"The Chamber of Sins Level 2" },
	{ 0, 20, 2, L"Intruders in Black", QM_Interaction, IK_Npc, 0, L"Helena", L"", L"The Forest Encampment" },
	{ 0, 20, 1, L"Intruders in Black", QM_Reward, 0, 0, L"Greust", L"Herald of Thunder", L"The Forest Encampment" },
	{ 0, 23, -1, L"Sharp and Cruel", QM_Interaction, IK_Npc, 0, L"Helena", L"", L"The Forest Encampment" },
	{ 0, 23, 8, L"Sharp and Cruel", QM_Interaction, IK_Npc, 0, L"Silk", L"", L"The Forest Encampment" },
	{ 0, 23, 7, L"Sharp and Cruel", QM_FullScan, 0, 0, L"", L"", L"The Weaver's Chambers", 1 },
	{ 0, 23, 6, L"Sharp and Cruel", QM_Hunt, 0, 0, L"The Weaver", L"", L"The Weaver's Chambers", 1 },
	{ 0, 23, 5, L"Sharp and Cruel", QM_Reward, 0, 0, L"Silk", L"Faster Casting Support", L"The Forest Encampment" },
	{ 0, 23, 4, L"Sharp and Cruel", QM_Reward, 0, 0, L"Silk", L"Faster Casting Support", L"The Forest Encampment" },
	{ 0, 23, 3, L"Sharp and Cruel", QM_Reward, 0, 0, L"Silk", L"Faster Casting Support", L"The Forest Encampment" },
	{ 0, 23, 2, L"Sharp and Cruel", QM_Reward, 0, 0, L"Silk", L"Faster Casting Support", L"The Forest Encampment" },
	{ 0, 24, -1, L"The Root of the Problem", QM_Interaction, IK_Npc, 0, L"Helena", L"", L"The Forest Encampment", 0, 20 },
	{ 0, 24, 11, L"The Root of the Problem", QM_Interaction, IK_Npc, 0, L"Helena", L"", L"The Forest Encampment", 0, 20 },
	{ 0, 24, 10, L"The Root of the Problem", QM_PickupItem, 0, 0, L"Baleful Gem", L"", L"The Chamber of Sins Level 2" },
	{ 0, 24, 7, L"The Root of the Problem", QM_Interaction, IK_Other, 0, L"Tree Roots", L"", L"The Wetlands" },
	{ 0, 24, 6, L"The Root of the Problem", QM_FullScan, 0, 0, L"", L"", L"The Northern Forest" },
	{ 0, 30, 17, L"Shadow of the Vaal", QM_Interaction, IK_Npc, 0, L"Yeena", L"", L"The Forest Encampment" },
	{ 0, 30, 16, L"Shadow of the Vaal", QM_Interaction, IK_Npc, 0, L"Eramir", L"", L"The Forest Encampment" },
	{ 0, 30, 15, L"Shadow of the Vaal", QM_FullScan, 0, 0, L"", L"", L"The Caverns" },
	{ 0, 30, 14, L"Shadow of the Vaal", QM_FullScan, 0, 0, L"", L"", L"The Ancient Pyramid", 3 },
	{ 0, 30, 12, L"Shadow of the Vaal", QM_Interaction, IK_Other, 0, L"Dark Altar", L"", L"The Ancient Pyramid", 3 },
	{ 0, 30, 5, L"Shadow of the Vaal", QM_Hunt, 0, 0, L"Vaal Oversoul", L"", L"The Ancient Pyramid", 3 },
	//Act3
	{ 0, 32, -1, L"Lost in Love", QM_Interaction, IK_Npc, 1, L"Clarissa", L"", L"The City of Sarn" },
	{ 0, 32, 12, L"Lost in Love", QM_Interaction, IK_Npc, 1, L"Clarissa", L"", L"The City of Sarn" },
	{ 0, 32, 9, L"Lost in Love", QM_Interaction, IK_Other, 0, L"Tolman", L"", L"The Crematorium" },
	{ 0, 32, 7, L"Lost in Love", QM_Reward, IK_Npc, 0, L"Clarissa", L"Sewer Keys", L"The Sarn Encampment" },
	{ 0, 32, 4, L"Lost in Love", QM_Reward, IK_Npc, 0, L"Maramoa", L"Bane", L"The Sarn Encampment" },
	{ 0, 37, -1, L"The Gemling Queen", QM_Interaction, IK_Npc, 0, L"Grigor", L"", L"The Sarn Encampment" },
	{ 0, 37, 1, L"The Gemling Queen", QM_Interaction, IK_Npc, 0, L"Lady Dialla", L"", L"The Solaris Temple Level 2" },
	{ 0, 38, 4, L"The Ribbon Spool", QM_FullScan, 0, 0, L"", L"", L"The Battlefront" },
	{ 0, 38, 3, L"The Ribbon Spool", QM_Interaction, IK_Npc, 0, L"Lady Dialla", L"Lapis Amulet", L"The Solaris Temple Level 2" },
	{ 0, 38, 2, L"The Ribbon Spool", QM_Interaction, IK_Npc, 0, L"Lady Dialla", L"Lapis Amulet", L"The Solaris Temple Level 2" },
	{ 0, 39, 9, L"Fiery Dust", QM_FullScan, 0, 0, L"", L"", L"The Docks" },
	{ 0, 39, 8, L"Fiery Dust", QM_FullScan, 0, 0, L"", L"", L"The Docks" },
	{ 0, 39, 7, L"Fiery Dust", QM_Interaction, IK_Npc, 0, L"Lady Dialla", L"Infernal Talc", L"The Solaris Temple Level 2" },
	{ 0, 39, 5, L"Fiery Dust", QM_Interaction, IK_Npc, 0, L"Lady Dialla", L"Infernal Talc", L"The Solaris Temple Level 2" },
	{ 0, 39, 4, L"Fiery Dust", QM_Interaction, IK_Npc, 0, L"Lady Dialla", L"Infernal Talc", L"The Solaris Temple Level 2" },
	{ 0, 39, 3, L"Fiery Dust", QM_Interaction, IK_Other, 0, L"Undying Blockage", L"", L"The Sewers" },
	{ 0, 39, 2, L"Fiery Dust", QM_Interaction, IK_Other, 0, L"Undying Blockage", L"", L"The Sewers" },
	{ 0, 39, 1, L"Fiery Dust", QM_Interaction, IK_Other, 0, L"Undying Blockage", L"", L"The Sewers" },
	{ 0, 40, 5, L"Sever the Right Hand", QM_Hunt, IK_Monster, 0, L"General Gravicius", L"", L"The Ebony Barracks" ,0,37},
	{ 0, 40, 4, L"Sever the Right Hand", QM_Hunt, IK_Monster, 0, L"General Gravicius", L"", L"The Ebony Barracks" },
	{ 0, 40, 3, L"Sever the Right Hand", QM_Hunt, IK_Monster, 0, L"General Gravicius", L"", L"The Ebony Barracks" },
	{ 0, 40, 2, L"Sever the Right Hand", QM_Hunt, IK_Monster, 0, L"General Gravicius", L"", L"The Ebony Barracks" },
	{ 0, 40, 1, L"Sever the Right Hand", QM_Interaction, IK_Npc, 0, L"Maramoa", L"Flameblast", L"The Sarn Encampment" },
	{ 0, 45, -1, L"Piety's Pets", QM_Interaction, IK_Npc, 0, L"Grigor", L"", L"The Sarn Encampment" },
	{ 0, 45, 5, L"Piety's Pets", QM_FullScan, 0, 0, L"", L"", L"The Lunaris Temple Level 2", 1 },
	{ 0, 45, 4, L"Piety's Pets", QM_Hunt, IK_Monster, 0, L"Piety", L"", L"The Lunaris Temple Level 2", 1 },
	{ 0, 45, 3, L"Piety's Pets", QM_Interaction, IK_Npc, 0, L"Grigor", L"", L"The Sarn Encampment"},
	{ 0, 45, 2, L"Piety's Pets", QM_Reward, IK_Npc, 0, L"Grigor", L"Book of Skill", L"The Sarn Encampment" },
	{ 0, 46, 8, L"The Sceptre of God", QM_PickupItem, 0, 0, L"Tower Key", L"", L"The Lunaris Temple Level 2", 1 },
	{ 0, 46, 7, L"The Sceptre of God", QM_Interaction, IK_Other, 0, L"Locked Door", L"", L"The Imperial Gardens" },
	{ 0, 46, 6, L"The Sceptre of God", QM_Interaction, IK_Npc_Spec, 2, L"Scion", L"", L"The Upper Sceptre of God", 3 },
	{ 0, 46, 6, L"The Sceptre of God", QM_Interaction, IK_Other, 0, L"Steel Cage", L"", L"The Upper Sceptre of God", 3 },
	{ 0, 46, 6, L"The Sceptre of God", QM_FullScan, 0, 0, L"", L"", L"The Upper Sceptre of God", 3 },
	{ 0, 46, 5, L"The Sceptre of God", QM_FullScan, 0, 0, L"", L"", L"The Upper Sceptre of God", 3 },
	{ 0, 46, 4, L"The Sceptre of God", QM_Hunt, IK_Monster, 0, L"Dominus, High Templar@Dominus, Ascendant", L"", L"The Upper Sceptre of God", 3 },
	{ 0, 46, 3, L"The Sceptre of God", QM_Interaction, IK_Npc, 0, L"Lady Dialla", L"", L"The Upper Sceptre of God", 3 },
	//Act4
 	{ 0, 49, -1, L"The Eternal Nightmare", QM_FullScan, 0, 0, L"", L"", L"Highgate" },
 	{ 0, 49, 27, L"The Eternal Nightmare", QM_FullScan, 0, 0, L"", L"", L"Highgate" },
 	{ 0, 49, 26, L"The Eternal Nightmare", QM_Interaction, IK_Npc, 0, L"Lady Dialla", L"", L"Highgate" },
 	{ 0, 49, 25, L"The Eternal Nightmare", QM_Interaction, IK_Npc, 0, L"Oyun", L"", L"Highgate" },			
 	{ 0, 51, 7, L"Breaking the Seal", QM_FullScan, 0, 0, L"", L"", L"The Dried Lake" },
 	{ 0, 51, 6, L"Breaking the Seal", QM_Hunt, IK_Monster, 0, L"Voll, Emperor of Purity", L"", L"The Dried Lake" },
 	{ 0, 51, 5, L"Breaking the Seal", QM_FullScan, 0, 0, L"", L"", L"The Dried Lake" },
 	{ 0, 51, 4, L"Breaking the Seal", QM_Interaction, IK_Other, 0, L"Deshret's Seal", L"", L"Highgate" },
 	{ 0, 51, 2, L"Breaking the Seal", QM_Reward, IK_Npc, 0, L"Oyun", L"Summon Chaos Golem", L"Highgate" },
 	{ 0, 51, 1, L"Breaking the Seal", QM_Reward, IK_Npc, 0, L"Oyun", L"Summon Chaos Golem", L"Highgate" },
 	{ 0, 49, 23, L"The Eternal Nightmare", QM_Interaction, IK_Npc, 0, L"Lady Dialla", L"", L"Highgate" },
 	{ 0, 56, 3, L"An Indomitable Spirit", QM_Interaction, IK_Other, 0, L"Deshret's Spirit", L"", L"The Mines Level 2" },	
 	{ 0, 49, 22, L"The Eternal Nightmare", QM_Interaction, IK_Npc, 0, L"Lady Dialla", L"", L"The Crystal Veins" },
 	{ 0, 49, 21, L"The Eternal Nightmare", QM_Interaction, IK_Npc, 0, L"Lady Dialla", L"", L"The Crystal Veins" },
 	{ 0, 53, 6, L"The King of Fury", QM_FullScan, 0, 0, L"", L"", L"Kaom's Stronghold", 1 },
 	{ 0, 53, 5, L"The King of Fury", QM_Hunt, IK_Monster, 0, L"King Kaom", L"", L"Kaom's Stronghold", 1 },
 	{ 0, 53, 4, L"The King of Fury", QM_PickupItem, 0, 0, L"The Eye of Fury", L"", L"Kaom's Stronghold", 1 },
 	{ 0, 53, 3, L"The King of Fury", QM_Interaction, IK_Npc, 0, L"Lady Dialla", L"", L"The Crystal Veins" },
 	{ 0, 54, 6, L"The King of Desire", QM_FullScan, 0, 0, L"", L"", L"The Grand Arena", 7 },
 	{ 0, 54, 5, L"The King of Desire", QM_Hunt, IK_Monster, 0, L"Daresso, King of Swords", L"", L"The Grand Arena", 7 },
 	{ 0, 54, 4, L"The King of Desire", QM_PickupItem, 0, 0, L"The Eye of Desire", L"", L"The Grand Arena", 7 },	
 	{ 0, 56, 2, L"An Indomitable Spirit", QM_Reward, IK_Npc, 1, L"Tasuni", L"Book of Skill", L"Highgate" },
 	{ 0, 54, 3, L"The King of Desire", QM_Interaction, IK_Npc, 0, L"Lady Dialla", L"", L"The Crystal Veins" },
 	{ 0, 49, 19, L"The Eternal Nightmare", QM_Interaction, IK_Npc, 0, L"Lady Dialla", L"", L"The Crystal Veins" },
 	{ 0, 54, 1, L"The King of Desire", QM_Reward, IK_Npc, 0, L"Dialla", L"Increased Area of Effect Support", L"Highgate" },
 	{ 0, 49, 18, L"The Eternal Nightmare", QM_FullScan, 0, 0, L"", L"", L"The Belly of the Beast Level 2" },
 	{ 0, 49, 17, L"The Eternal Nightmare", QM_FullScan, 0, 0, L"", L"", L"The Belly of the Beast Level 2" },
 	{ 0, 49, 16, L"The Eternal Nightmare", QM_Hunt, IK_Monster, 0, L"Piety, the Abomination", L"", L"The Belly of the Beast Level 2" },
 	{ 0, 49, 15, L"The Eternal Nightmare", QM_Interaction, IK_Npc, 0, L"Piety", L"", L"The Belly of the Beast Level 2" },
 	{ 0, 55, 26, L"Corpus Malachus",  QM_Hunt, IK_Monster, 0, L"Shavronne of Umbra@Maligaro, The Inquisitor@Doedre Darktongue", L"Shavronne's Arena@Maligaro's Arena@Doedre's Arena", L"The Harvest" },
 	{ 0, 55, 25, L"Corpus Malachus",  QM_Hunt, IK_Monster, 0, L"Shavronne of Umbra@Maligaro, The Inquisitor@Doedre Darktongue", L"Shavronne's Arena@Maligaro's Arena@Doedre's Arena", L"The Harvest" },
 	{ 0, 55, 24, L"Corpus Malachus",  QM_Hunt, IK_Monster, 0, L"Shavronne of Umbra@Maligaro, The Inquisitor@Doedre Darktongue", L"Shavronne's Arena@Maligaro's Arena@Doedre's Arena", L"The Harvest" },
 	{ 0, 55, 23, L"Corpus Malachus",  QM_Hunt, IK_Monster, 0, L"Shavronne of Umbra@Maligaro, The Inquisitor@Doedre Darktongue", L"Shavronne's Arena@Maligaro's Arena@Doedre's Arena", L"The Harvest" },
 	{ 0, 55, 22, L"Corpus Malachus",  QM_Hunt, IK_Monster, 0, L"Shavronne of Umbra@Maligaro, The Inquisitor@Doedre Darktongue", L"Shavronne's Arena@Maligaro's Arena@Doedre's Arena", L"The Harvest" },
 	{ 0, 55, 21, L"Corpus Malachus",  QM_Hunt, IK_Monster, 0, L"Shavronne of Umbra@Maligaro, The Inquisitor@Doedre Darktongue", L"Shavronne's Arena@Maligaro's Arena@Doedre's Arena", L"The Harvest" },
 	{ 0, 55, 20, L"Corpus Malachus",  QM_Hunt, IK_Monster, 0, L"Shavronne of Umbra@Maligaro, The Inquisitor@Doedre Darktongue", L"Shavronne's Arena@Maligaro's Arena@Doedre's Arena", L"The Harvest" },
 	{ 0, 55, 19, L"Corpus Malachus",  QM_Hunt, IK_Monster, 0, L"Shavronne of Umbra@Maligaro, The Inquisitor@Doedre Darktongue", L"Shavronne's Arena@Maligaro's Arena@Doedre's Arena", L"The Harvest" },
 	{ 0, 55, 18, L"Corpus Malachus",  QM_Hunt, IK_Monster, 0, L"Shavronne of Umbra@Maligaro, The Inquisitor@Doedre Darktongue", L"Shavronne's Arena@Maligaro's Arena@Doedre's Arena", L"The Harvest" },
 	{ 0, 55, 17, L"Corpus Malachus",  QM_Hunt, IK_Monster, 0, L"Shavronne of Umbra@Maligaro, The Inquisitor@Doedre Darktongue", L"Shavronne's Arena@Maligaro's Arena@Doedre's Arena", L"The Harvest" },
 	{ 0, 55, 16, L"Corpus Malachus",  QM_Hunt, IK_Monster, 0, L"Shavronne of Umbra@Maligaro, The Inquisitor@Doedre Darktongue", L"Shavronne's Arena@Maligaro's Arena@Doedre's Arena", L"The Harvest" },
 	{ 0, 55, 15, L"Corpus Malachus",  QM_Hunt, IK_Monster, 0, L"Shavronne of Umbra@Maligaro, The Inquisitor@Doedre Darktongue", L"Shavronne's Arena@Maligaro's Arena@Doedre's Arena", L"The Harvest" },
 	{ 0, 55, 14, L"Corpus Malachus",  QM_Hunt, IK_Monster, 0, L"Shavronne of Umbra@Maligaro, The Inquisitor@Doedre Darktongue", L"Shavronne's Arena@Maligaro's Arena@Doedre's Arena", L"The Harvest" },
 	{ 0, 55, 13, L"Corpus Malachus",  QM_Hunt, IK_Monster, 0, L"Shavronne of Umbra@Maligaro, The Inquisitor@Doedre Darktongue", L"Shavronne's Arena@Maligaro's Arena@Doedre's Arena", L"The Harvest" },
 	{ 0, 55, 12, L"Corpus Malachus",  QM_Hunt, IK_Monster, 0, L"Shavronne of Umbra@Maligaro, The Inquisitor@Doedre Darktongue", L"Shavronne's Arena@Maligaro's Arena@Doedre's Arena", L"The Harvest" },
 	{ 0, 55, 11, L"Corpus Malachus",  QM_Hunt, IK_Monster, 0, L"Shavronne of Umbra@Maligaro, The Inquisitor@Doedre Darktongue", L"Shavronne's Arena@Maligaro's Arena@Doedre's Arena", L"The Harvest" },
 	{ 0, 55, 10, L"Corpus Malachus",  QM_Hunt, IK_Monster, 0, L"Shavronne of Umbra@Maligaro, The Inquisitor@Doedre Darktongue", L"Shavronne's Arena@Maligaro's Arena@Doedre's Arena", L"The Harvest" },
 	{ 0, 55, 9, L"Corpus Malachus",  QM_Hunt, IK_Monster, 0, L"Shavronne of Umbra@Maligaro, The Inquisitor@Doedre Darktongue", L"Shavronne's Arena@Maligaro's Arena@Doedre's Arena", L"The Harvest" },
 	{ 0, 55, 8, L"Corpus Malachus", QM_Hunt, IK_Monster, 0, L"Shavronne of Umbra@Maligaro, The Inquisitor@Doedre Darktongue", L"Shavronne's Arena@Maligaro's Arena@Doedre's Arena", L"The Harvest" },
 	{ 0, 55, 7, L"Corpus Malachus", QM_Hunt, IK_Monster, 0, L"Shavronne of Umbra@Maligaro, The Inquisitor@Doedre Darktongue", L"Shavronne's Arena@Maligaro's Arena@Doedre's Arena", L"The Harvest" },
 	{ 0, 55, 6, L"Corpus Malachus", QM_Hunt, IK_Monster, 0, L"Shavronne of Umbra@Maligaro, The Inquisitor@Doedre Darktongue", L"Shavronne's Arena@Maligaro's Arena@Doedre's Arena", L"The Harvest" },
 	{ 0, 55, 5, L"Corpus Malachus", QM_Hunt, IK_Monster, 0, L"Shavronne of Umbra@Maligaro, The Inquisitor@Doedre Darktongue", L"Shavronne's Arena@Maligaro's Arena@Doedre's Arena", L"The Harvest" },
 	{ 0, 55, 4, L"Corpus Malachus", QM_Hunt, IK_Monster, 0, L"Shavronne of Umbra@Maligaro, The Inquisitor@Doedre Darktongue", L"Shavronne's Arena@Maligaro's Arena@Doedre's Arena", L"The Harvest" },
 	{ 0, 55, 3, L"Corpus Malachus", QM_Hunt, IK_Monster, 0, L"Shavronne of Umbra@Maligaro, The Inquisitor@Doedre Darktongue", L"Shavronne's Arena@Maligaro's Arena@Doedre's Arena", L"The Harvest" },
 	{ 0, 55, 2, L"Corpus Malachus", QM_Hunt, IK_Monster, 0, L"Shavronne of Umbra@Maligaro, The Inquisitor@Doedre Darktongue", L"Shavronne's Arena@Maligaro's Arena@Doedre's Arena", L"The Harvest" },
 	{ 0, 55, 1, L"Corpus Malachus", QM_Interaction, IK_Npc, 0, L"Piety", L"", L"The Harvest" },
 	{ 0, 49, 13, L"The Eternal Nightmare", QM_Interaction, IK_Other, 0, L"The Black Core", L"", L"The Harvest" },
 	{ 0, 49, 13, L"The Eternal Nightmare", QM_FullScan, 0, 0, L"", L"", L"The Harvest", 1 },
 	{ 0, 49, 4, L"The Eternal Nightmare", QM_Hunt, IK_Monster, 0, L"Heart of the Beast@Malachai, The Nightmare", L"", L"The Harvest", 1 }, 
 	{ 0, 49, 2, L"The Eternal Nightmare", QM_Interaction, IK_Npc, 1, L"Tasuni", L"", L"Highgate" },
 	{ 0, 74, 3, L"Return to Oriath", QM_FullScan, 0, 0, L"", L"", L"The Ascent" },
 	{ 0, 74, 2, L"Return to Oriath", QM_FullScan, 0, 0, L"", L"", L"The Ascent" },	
	{ 0, 74, 1, L"Return to Oriath", QM_FullScan, 0, 0, L"", L"", L"The Ascent" },
	//Act5
	{ 0, 75, -1, L"Return to Oriath", QM_FullScan, 0, 0, L"", L"", L"The Slave Pens" },
	{ 0, 75, 6, L"Return to Oriath", QM_FullScan, 0, 0, L"", L"", L"The Slave Pens" },
	{ 0, 75, 3, L"Return to Oriath", QM_FullScan, 0, 0, L"", L"", L"Overseer's Tower" },
	{ 0, 75, 2, L"Return to Oriath", QM_Interaction, IK_Npc, 0, L"Lani", L"Topaz Ring", L"Overseer's Tower" },
	{ 0, 75, 1, L"Return to Oriath", QM_Interaction, IK_Npc, 0, L"Lani", L"Topaz Ring", L"Overseer's Tower" },
	{ 0, 77, -1, L"The Key to Freedom", QM_Interaction, IK_Npc, 0, L"Utula", L"", L"Overseer's Tower" },
	{ 0, 77, 5, L"The Key to Freedom", QM_FullScan, 0, 0, L"", L"", L"The Control Blocks" },
	{ 0, 77, 4, L"The Key to Freedom", QM_Hunt, IK_Monster, 0, L"Justicar Casticus", L"", L"The Control Blocks" },
	{ 0, 77, 3, L"The Key to Freedom", QM_Hunt, IK_Monster, 0, L"Justicar Casticus", L"", L"The Control Blocks" },
	{ 0, 77, 2, L"The Key to Freedom", QM_Interaction, IK_Npc, 0, L"Lani", L"Hallowed Life Flask", L"Overseer's Tower" },
	{ 0, 77, 1, L"The Key to Freedom", QM_Interaction, IK_Npc, 0, L"Lani", L"Hallowed Life Flask", L"Overseer's Tower" },
	{ 0, 79, 7, L"Death to Purity", QM_Interaction, IK_Other, 0, L"Templar Courts Entrance", L"", L"Oriath Square" },
	{ 0, 79, 6, L"Death to Purity", QM_FullScan, 0, 0, L"", L"", L"The Chamber of Innocence" },
	{ 0, 79, 5, L"Death to Purity", QM_FullScan, 0, 0, L"", L"", L"The Chamber of Innocence" , 1 },
	{ 0, 79, 4, L"Death to Purity", QM_Hunt, IK_Monster, 0, L"High Templar Avarius", L"", L"The Chamber of Innocence" , 1 },
	{ 0, 79, 3, L"Death to Purity", QM_Interaction, IK_Npc, 0, L"Sin", L"", L"The Chamber of Innocence" ,1 },
	{ 0, 79, 2, L"Death to Purity", QM_Interaction, IK_Npc, 0, L"Lani", L"Hazardous Research Cobalt Jewel", L"Overseer's Tower" },
	{ 0, 79, 1, L"Death to Purity", QM_Interaction, IK_Npc, 0, L"Lani", L"Hazardous Research Cobalt Jewel", L"Overseer's Tower" },
	{ 0, 82, 10, L"The Ravenous God", QM_FullScan, 0, 0, L"", L"", L"The Torched Courts" },
	{ 0, 82, 9, L"The Ravenous God", QM_FullScan, 0, 0, L"", L"", L"The Ruined Square" },
	{ 0, 82, 8, L"The Ravenous God", QM_FullScan, 0, 0, L"", L"", L"The Ossuary" },
	{ 0, 82, 7, L"The Ravenous God", QM_Interaction, IK_Other, 0, L"Tomb of the First Templar", L"", L"The Ossuary" },
	{ 0, 82, 6, L"The Ravenous God", QM_FullScan, 0, 0, L"", L"", L"The Cathedral Rooftop" },
	{ 0, 82, 5, L"The Ravenous God", QM_Interaction, IK_Other, 0, L"Cradle of Purity", L"", L"The Cathedral Rooftop" },
	{ 0, 82, 3, L"The Ravenous God", QM_Hunt, IK_Monster, 0, L"Kitava, the Insatiable", L"", L"The Cathedral Rooftop" },
	{ 0, 82, 2, L"The Ravenous God", QM_Interaction, IK_Npc, 1, L"Sin", L"", L"The Cathedral Rooftop" },
	{ 0, 82, 1, L"The Ravenous God", QM_Interaction, IK_Npc_Element, 0, L"Lilly Roth", L"", L"The Cathedral Rooftop" },
	//Act6
	{ 0, 85, 10, L"Essence of Umbra", QM_Interaction, IK_Npc, 0, L"Sin", L"", L"Lioneye's Watch" },
	{ 0, 88, -1, L"Fallen from Grace", QM_Interaction, IK_Npc, 0, L"Lilly Roth", L"", L"Lioneye's Watch" },
	{ 0, 88, 3, L"Fallen from Grace", QM_FullScan, 0, 0, L"", L"", L"The Twilight Strand" },
	{ 0, 88, 2, L"Fallen from Grace", QM_Reward, IK_Npc, 0, L"Lilly Roth", L"Book of Regrets", L"Lioneye's Watch" },
	{ 0, 85, 9, L"Essence of Umbra", QM_FullScan, 0, 0, L"", L"", L"Lioneye's Watch" },
};

std::vector<ObjectCacheInfo> g_pInfoObjectCache
{
	{ OCT_Other, L"", L"Metadata/QuestObjects/SouthBeachTownEntrance", L"", { 0, 0 } },
	{ OCT_Other, L"", L"Metadata/MiscellaneousObjects/AreaTransition", L"", { 0, 0 } },
	{ OCT_Other, L"", L"Metadata/MiscellaneousObjects/AreaTransitionMapMarker", L"", { 0, 0 } },
	{ OCT_Other, L"", L"Metadata/MiscellaneousObjects/AreaTransitionToggleable", L"", { 0, 0 } },
	{ OCT_Other, L"", L"Metadata/MiscellaneousObjects/Waypoint", L"", { 0, 0 } },
	{ OCT_Other, L"", L"Metadata/QuestObjects/MerveilExitTransition", L"", { 0, 0 } },
	{ OCT_Other, L"", L"Metadata/QuestObjects/Inca/IncaTreeTransition", L"", { 0, 0 } },
	{ OCT_Other, L"", L"Metadata/QuestObjects/Sewers/SewersLockedTransition", L"", { 0, 0 } },
	{ OCT_Other, L"", L"Metadata/QuestObjects/Inca/IncaPyramidTransition", L"", { 0, 0 } },
	{ OCT_Other, L"Strange Glyph Wall", L"Metadata/QuestObjects/WaterCave/GlyphWall", L"", { 0, 0 } },
	{ OCT_Other, L"", L"Metadata/QuestObjects/WaterCave/WaterCaveAreaTransition", L"", { 0, 0 } },
	{ OCT_Other, L"", L"Metadata/QuestObjects/Piety/PietyArenaPortal1", L"", { 0, 0 } },
	{ OCT_Other, L"", L"Metadata/QuestObjects/EpicDoor/EpicDoorTransition", L"", { 0, 0 } },
	{ OCT_Npc, L"Bestel", L"Metadata/NPC/Act1/Bestel", L"Lioneye's Watch", { 360, 247 } },
	{ OCT_Npc, L"Tarkleigh", L"Metadata/NPC/Act1/Tarkleigh", L"Lioneye's Watch", { 392, 190 } },
	{ OCT_Npc, L"Nessa", L"Metadata/NPC/Act1/Nessa", L"Lioneye's Watch", { 346, 270 } },
	{ OCT_Other, L"Waypoint", L"Metadata/MiscellaneousObjects/Waypoint", L"Lioneye's Watch", { 256, 169 } },
	{ OCT_Other, L"Stash", L"Metadata/MiscellaneousObjects/Stash", L"Lioneye's Watch", { 313, 279 } },
};

std::vector<WCHAR *> g_pInfoQuestReward
{
	L"Freezing Pulse",
	L"Quicksilver Flask",
	L"Elemental Proliferation Support",
	L"Book of Regrets",
	L"Clarity",
	L"Orb of Storms",
	L"Frostblink",
	L"Added Lightning Damage Support",
	L"Storm Burst",
	//Act2
	L"Book of Skill",
	L"Quicksilver Flask",
	L"Faster Casting Support",
	L"Herald of Thunder",
	L"The Apex",
	//Act3
	L"Sewer Keys",
	L"Bane",
//	L"Amber Amulet",						//From Lady Diallia = +30 Str
//	L"Jade Amulet"							//From Lady Diallia = +30 Dex
	L"Lapis Amulet",						//From Lady Diallia = +30 Int
//	L"Gold Amulet"							//From Lady Diallia = +20 Rarity Found Percent
	L"Infernal Talc",						//From Lady Diallia
	L"Flameblast",							//From Maramoa
	//Act4
	L"Summon Chaos Golem",
	L"Increased Area of Effect Support",
	//Act5
	L"Topaz Ring",
	L"Hallowed Life Flask",
	L"Hazardous Research Cobalt Jewel",
};

std::vector<SpecialAreaInfo> g_pInfoSpecialArea
{
	//Act1
	{ 
		L"The Upper Prison", 3, 0, 0, 
		{
			{ 0, 2, 1, L"Entrance", L"The Lower Prison",			L"Metadata/MiscellaneousObjects/AreaTransitionToggleable" },
			{ 0, 1, 0, L"Entrance", L"The Warden's Quarters",	L"Metadata/MiscellaneousObjects/AreaTransitionToggleable" },
			{ 1, 0, 1, L"Entrance", L"The Upper Prison",			L"Metadata/MiscellaneousObjects/AreaTransitionToggleable" },
			{ 1, 1, 1, L"Entrance", L"The Warden's Chambers",	L"Metadata/MiscellaneousObjects/AreaTransition" },
			{ 2, 0, 0, L"Entrance", L"The Warden's Quarters",	L"Metadata/QuestObjects/BossArenaEntranceTransition" },
			{ 2, 2, 1, L"Entrance", L"Prisoner's Gate",			L"Metadata/MiscellaneousObjects/AreaTransition" },
		}
	},
	{
		L"The Cavern of Anger", 2, 0, 0,
		{
			{ 0, 2, 1, L"Entrance", L"The Cavern of Wrath",		L"Metadata/MiscellaneousObjects/AreaTransition" },
			{ 0, 1, 1, L"Entrance", L"Merveil's Lair",			L"Metadata/MiscellaneousObjects/AreaTransition" },
			{ 1, 0, 1, L"Entrance", L"The Cavern of Anger",		L"Metadata/QuestObjects/BossArenaEntranceTransition" },
			{ 1, 2, 1, L"Entrance", L"The Southern Forest",		L"Metadata/QuestObjects/MerveilExitTransition" },
		}
	},
	//Act2
	{
		L"The Weaver's Chambers", 2, 0, 0,
		{
			{ 0, 2, 1, L"Entrance", L"The Western Forest",		L"Metadata/MiscellaneousObjects/AreaTransition" },
			{ 0, 1, 1, L"Entrance", L"The Weaver's Nest",		L"Metadata/MiscellaneousObjects/AreaTransition" },
			{ 1, 0, 1, L"Entrance", L"The Weaver's Chambers",	L"Metadata/QuestObjects/BossArenaEntranceTransition" },
		}
	},
	{
		L"The Ancient Pyramid", 4, 0, 0,
		{
			{ 0, 2, 1, L"Entrance", L"The Caverns",				L"Metadata/MiscellaneousObjects/AreaTransition" },
			{ 0, 1, 0, L"Entrance", L"Stairs",					L"Metadata/MiscellaneousObjects/AreaTransition" },
			{ 1, 0, 0, L"Entrance", L"Stairs",					L"Metadata/MiscellaneousObjects/AreaTransition" },
			{ 1, 1, 0, L"Entrance", L"Stairs",					L"Metadata/MiscellaneousObjects/AreaTransition" },
			{ 2, 0, 0, L"Entrance", L"Stairs",					L"Metadata/MiscellaneousObjects/AreaTransition" },
			{ 2, 1, 1, L"Entrance", L"Pyramid Apex",			L"Metadata/MiscellaneousObjects/AreaTransition" },
			{ 3, 0, 1, L"Entrance", L"The Ancient Pyramid",		L"Metadata/QuestObjects/BossArenaEntranceTransition" },
			{ 3, 2, 1, L"Entrance", L"The City of Sarn",		L"Metadata/QuestObjects/Inca/IncaPyramidTransition" },
		}
	},
	//Act3
	{
		L"The Lunaris Temple Level 2", 2, 0, 0,
		{
			{ 0, 2, 1, L"Entrance", L"The Lunaris Temple Level 1",	L"Metadata/MiscellaneousObjects/AreaTransition" },
			{ 0, 1, 0, L"Entrance", L"Portal",						L"Metadata/QuestObjects/Piety/PietyArenaPortal1" },
			{ 1, 0, 0, L"Entrance", L"Portal",						L"Metadata/QuestObjects/Piety/PietyArenaPortal2" },
		}
	},
	{
		L"The Sceptre of God", 3, 2, 0,
		{
			{ 0, 2, 1, L"Entrance", L"The Imperial Gardens",		L"Metadata/MiscellaneousObjects/AreaTransition" },
			{ 0, 1, 0, L"Entrance", L"Stairs",						L"Metadata/MiscellaneousObjects/AreaTransition" },
			{ 1, 0, 0, L"Entrance", L"Stairs",						L"Metadata/MiscellaneousObjects/AreaTransition" },
			{ 1, 1, 0, L"Entrance", L"Stairs",						L"Metadata/MiscellaneousObjects/AreaTransition" },
			{ 2, 0, 0, L"Entrance", L"Stairs",						L"Metadata/MiscellaneousObjects/AreaTransition" },
			{ 2, 2, 1, L"Entrance", L"The Upper Sceptre of God",	L"Metadata/MiscellaneousObjects/AreaTransition" },
		}
	},
	{
		L"The Upper Sceptre of God", 4, 0, 0,
		{
			{ 0, 2, 1, L"Entrance", L"The Sceptre of God",			L"Metadata/MiscellaneousObjects/AreaTransition" },
			{ 0, 1, 0, L"Entrance", L"Stairs",						L"Metadata/MiscellaneousObjects/AreaTransition" },
			{ 1, 0, 0, L"Entrance", L"Stairs",						L"Metadata/MiscellaneousObjects/AreaTransition" },
			{ 1, 1, 0, L"Entrance", L"Stairs",						L"Metadata/MiscellaneousObjects/AreaTransition" },
			{ 2, 0, 0, L"Entrance", L"Stairs",						L"Metadata/MiscellaneousObjects/AreaTransition" },
			{ 2, 1, 1, L"Entrance", L"Tower Rooftop",				L"Metadata/MiscellaneousObjects/AreaTransition" },
			{ 3, 0, 1, L"Entrance", L"The Upper Sceptre of God",	L"Metadata/QuestObjects/BossArenaEntranceTransition" },
		}
	},
	//Act4	
	{
		L"Kaom's Stronghold", 2, 0, 1,
		{
			{ 0, 2, 1, L"Entrance", L"Kaom's Dream",				L"Metadata/MiscellaneousObjects/AreaTransition" },
			{ 0, 1, 1, L"Entrance", L"Caldera of The King",			L"Metadata/MiscellaneousObjects/AreaTransition" },
			{ 1, 0, 1, L"Entrance", L"Kaom's Stronghold",			L"Metadata/MiscellaneousObjects/AreaTransitionMapMarkerTR" },
		}
	},
	{
		L"The Grand Arena", 8, 0, 0,
		{
			{ 0, 2, 1, L"Entrance", L"Daresso's Dream",				L"Metadata/MiscellaneousObjects/AreaTransition" },
			{ 0, 1, 0, L"Entrance", L"Arena",						L"Metadata/MiscellaneousObjects/AreaTransition" },
			{ 1, 0, 0, L"Entrance", L"Passageway",					L"Metadata/MiscellaneousObjects/AreaTransition" },
			{ 1, 1, 0, L"Entrance", L"Passageway",					L"Metadata/MiscellaneousObjects/AreaTransitionToggleableReverse" },
			{ 2, 0, 0, L"Entrance", L"Arena",						L"Metadata/MiscellaneousObjects/AreaTransition" },
			{ 2, 1, 0, L"Entrance", L"Arena",						L"Metadata/MiscellaneousObjects/AreaTransition" },
			{ 3, 0, 0, L"Entrance", L"Passageway",					L"Metadata/MiscellaneousObjects/AreaTransition" },
			{ 3, 1, 0, L"Entrance", L"Passageway",					L"Metadata/MiscellaneousObjects/AreaTransitionToggleableReverse" },
			{ 4, 0, 0, L"Entrance", L"Arena",						L"Metadata/MiscellaneousObjects/AreaTransition" },
			{ 4, 1, 0, L"Entrance", L"Arena",						L"Metadata/MiscellaneousObjects/AreaTransition" },
			{ 5, 0, 0, L"Entrance", L"Passageway",					L"Metadata/MiscellaneousObjects/AreaTransitionToggleableReverse" },
			{ 5, 1, 0, L"Entrance", L"Passageway",					L"Metadata/MiscellaneousObjects/AreaTransitionToggleableReverse" },
			{ 6, 0, 0, L"Entrance", L"Arena",						L"Metadata/MiscellaneousObjects/AreaTransition" },
			{ 6, 1, 0, L"Entrance", L"The Ring of Blades",			L"Metadata/MiscellaneousObjects/AreaTransition" },
			{ 7, 0, 1, L"Entrance", L"Passageway",					L"Metadata/MiscellaneousObjects/AreaTransitionMapMarkerTR" },
		}
	},
	{
		L"The Harvest", 2, 0, 0,
		{
			{ 0, 2, 1, L"Entrance", L"The Belly of the Beast Level 2",	L"Metadata/MiscellaneousObjects/AreaTransition" },
			{ 0, 1, 1, L"Entrance", L"The Black Core",					L"Metadata/QuestObjects/Act4/CoreTransition" },
			{ 1, 0, 1, L"Entrance", L"The Harvest",						L"Metadata/MiscellaneousObjects/AreaTransition" }, //?
		}
	},
	//Act5
	{
		L"The Chamber of Innocence", 2, 0, 0,
		{
			{ 0, 2, 1, L"Entrance", L"The Lunaris Temple Level 1",	L"Metadata/MiscellaneousObjects/AreaTransition" },
			{ 0, 1, 1, L"Entrance", L"Sanctum of Innocence",		L"Metadata/MiscellaneousObjects/AreaTransition" },
			{ 1, 0, 1, L"Entrance", L"The Chamber of Innocence",	L"Metadata/MiscellaneousObjects/AreaTransition" },
		}
	},
// 	{
// 		L"The Cathedral Rooftop", 2, 0,
// 		{
// 			{ 0, 2, 1, L"Entrance", L"The Ruined Square",			L"Metadata/MiscellaneousObjects/AreaTransition" },
// 			{ 0, 1, 1, L"Entrance", L"Cathedral Apex",				L"Metadata/MiscellaneousObjects/AreaTransition" },
// 			{ 1, 0, 1, L"Entrance", L"The Cathedral Rooftop",		L"Metadata/MiscellaneousObjects/AreaTransition" },
// 		}
// 	},
};

std::vector<std::wstring>	g_pAreasInfoSkipScroll
{
	L"The Southern Forest",
	L"The City of Sarn",
	L"The Aqueduct",
	L"The Slave Pens",
};

std::vector<ClickNpcInfo>	g_pInfoClickNpcs
{
	{L"Lilly Roth",0x01},
};

DWORD GameLogic::GetCurrentQuestIndex(bool checkArea/* = false*/)
{
	for (DWORD i = 0; i < g_pInfoQuest.size(); i++)
	{
		if (std::find(_setting.questsDone.begin(), _setting.questsDone.end(), i) != _setting.questsDone.end())
			continue;
		if (g_pInfoQuest[i].questStep == 0 || (g_pInfoQuest[i].questStep == -1))
			continue;
		if (checkArea && _worldArea->name != g_pInfoQuest[i].areaName)
			continue;
		for (DWORD j = 0; j < _questData->quests.size(); j++)
		{
			if (g_pInfoQuest[i].questKey == _questData->quests[j].key && g_pInfoQuest[i].questStep == _questData->quests[j].step)
			{
				// 이전 퀘스트를 진행하였는가를 확인한다
				if (g_pInfoQuest[i].preQuestKey)
				{
					bool flagPrequestDone = true;
					for (DWORD l = 0; l < i; l++)
					{
						if ((g_pInfoQuest[i].preQuestKey <= g_pInfoQuest[l].preQuestKey && g_pInfoQuest[l].preQuestKey < g_pInfoQuest[i].questKey))
						{
							for (DWORD k = 0; k < _questData->quests.size(); k++)
							{
								if (g_pInfoQuest[l].questKey == _questData->quests[k].key && !_questData->quests[k].isCompleted)
								{
									//Log(L"PreQuest %d %d", _questData->quests[k].key, _questData->quests[k].step);
									flagPrequestDone = false;
									break;
								}
							}
							if (!flagPrequestDone)
								break;
						}

						if (flagPrequestDone)
							return i;
					}

				}
				else
				{
					return i;
				}

			}
		}
	}

	if (!checkArea)
	{
		for (DWORD i = 0; i < g_pInfoQuest.size(); i++)
		{
			if (std::find(_setting.questsDone.begin(), _setting.questsDone.end(), i) != _setting.questsDone.end())
				continue;
			if (g_pInfoQuest[i].questStep != 0 && g_pInfoQuest[i].questStep != -1)
				continue;
			if (checkArea && _worldArea->name != g_pInfoQuest[i].areaName)
				continue;
			for (DWORD j = 0; j < _questData->quests.size(); j++)
			{
				if (g_pInfoQuest[i].questKey == _questData->quests[j].key && g_pInfoQuest[i].questStep == _questData->quests[j].step)
					return i;
			}
		}
	}

	return NOINDEX;
}

DWORD GameLogic::GetMinimapIconCacheIndex(std::wstring iconName, std::wstring objectName/* = L""*/, std::wstring path/* = L""*/, bool checkCanGoto/* = false*/)
{
	DWORD i, dwIndex = NOINDEX;
	float dist, distMin = 1000000000.f;

	for (i = 0; i < _map.iconsCache.size(); i++)
	{
		if (_map.iconsCache[i].iconName != iconName)
			continue;
		if (_map.iconsCache[i].name != objectName)
			continue;
		if (!path.empty() && _map.iconsCache[i].path != path)
			continue;
		if (checkCanGoto && !_map.iconsCache[i].canGoto)
			continue;

		dist = DIST(_localPlayer->positionedComp.gridPos, _map.iconsCache[i].pos.gridPos);
		if (dist < distMin)
		{
			distMin = dist;
			dwIndex = i;
		}
	}

	return dwIndex;
}

void GameLogic::LoadObjectCache()
{
	DWORD i, j, dwIndex;

	// 없어진 아이콘 추가
	for (j = 0; j < _map.iconsCache.size(); j++)
	{
		dwIndex = NOINDEX;
		for (i = 0; i < _serverData->minimapIcons.size(); i++)
		{
			auto icon = &GameStateController::InGameState.serverData.minimapIcons[i];
			if (_map.iconsCache[j].iconName == icon->iconName && _map.iconsCache[j].pos.gridPos == icon->pos.gridPos)
			{
				dwIndex = j;
				break;
			}
		}

		if (dwIndex == NOINDEX)
		{
			_map.iconsCache.erase(_map.iconsCache.begin() + j);
			j--;
		}
	}

	// 새로운 미니맵 아이콘 추가
	for (i = 0; i < _serverData->minimapIcons.size(); i++)
	{
		auto icon = &GameStateController::InGameState.serverData.minimapIcons[i];
		dwIndex = NOINDEX;
		for (j = 0; j < _map.iconsCache.size(); j++)
		{
			if (_map.iconsCache[j].iconName == icon->iconName && _map.iconsCache[j].pos.gridPos == icon->pos.gridPos)
			{
				dwIndex = j;
				break;
			}
		}

		if (dwIndex != NOINDEX)
		{
			_map.iconsCache[dwIndex] = *icon;
		}
		else
		{
			_map.iconsCache.push_back(*icon);
		}
	}

	// 특수 지역에서 입구들을 추가한다
	auto dwSpecialAreaIndex = GetSpecialAreaIndex(_worldArea->name);
	if (dwSpecialAreaIndex != NOINDEX)
	{		
		for (i = 0; i < _entityList->others.size(); i++)
		{
			bool isAlreadyExist = std::find(_map.entranceCache.begin(), _map.entranceCache.end(), _entityList->others[i].id) != _map.entranceCache.end();
			bool isSkip = std::find(_map.skipEntrances.begin(), _map.skipEntrances.end(), _entityList->others[i].id) != _map.skipEntrances.end();
			if (isAlreadyExist)		// 이미 추기되였으면 스킵한다
			{
				continue;
			}

			if (isSkip)	//스킵해야할 입구
			{
				//Log(L"skip enterance id - %d", _entityList->others[i].id);
				continue;
			}

			//dwIndex = NOINDEX;
			for (j = 0; j < g_pInfoSpecialArea[dwSpecialAreaIndex].minimapIcons.size(); j++)
			{
				if (g_pInfoSpecialArea[dwSpecialAreaIndex].minimapIcons[j].next != 0 && g_pInfoSpecialArea[dwSpecialAreaIndex].minimapIcons[j].next != 1)	// 올라가거나 내려가는 입구만 추가한다
					continue;
				if (_entityList->others[i].render.name == g_pInfoSpecialArea[dwSpecialAreaIndex].minimapIcons[j].szName &&
					_entityList->others[i].path == g_pInfoSpecialArea[dwSpecialAreaIndex].minimapIcons[j].szPath)
				{
					Log(L"Add new entrance in special area %d-[%s]", (int)_map.entranceCache.size(), _entityList->others[i].render.name.c_str());
					_map.entranceCache.push_back(_entityList->others[i].id);
					break;
				}
			}
			//if(dwIndex != NOINDEX)
			//	break;
		}
	}
}

void GameLogic::InitObjectCacheGoState()
{
	for (DWORD i = 0; i < _map.iconsCache.size(); i++)
	{
		if (FindTargetPath(_map.iconsCache[i].pos.gridPos))
			_map.iconsCache[i].canGoto = 1;
		else
			_map.iconsCache[i].canGoto = 0;
	}
}

DWORD GameLogic::GetSpecialAreaIndex(std::wstring areaName)
{
	for (DWORD i = 0; i < g_pInfoSpecialArea.size(); i++)
	{
		if (areaName == g_pInfoSpecialArea[i].szAreaName)
			return i;
	}

	return NOINDEX;
}

bool GameLogic::LoadMinimapIconTargetPath(DWORD dwIconIndex)
{
	if (dwIconIndex >= _map.iconsCache.size())
		return false;

	if (!LoadTargetPath(_map.iconsCache[dwIconIndex].pos.gridPos))
	{
		_map.iconsCache[dwIconIndex].canGoto = 0;
		return false;
	}
	
	return true;
}

bool GameLogic::RegisterWaypoint()
{
	if (!_worldArea->hasWaypoint)
		return false;

	// 거점이 등록되지 않은 경우 해당 경로를 찾아서 이동한다
	if (std::find(_ingameUi->teleportFieldIds.begin(), _ingameUi->teleportFieldIds.end(), _worldArea->worldAreaID) != _ingameUi->teleportFieldIds.end())
		return false;

	if (GotoWaypoint(true))
		return true;

	return false;
}

BYTE GameLogic::GotoWaypoint(bool isRegister/* = true*/, DWORD *outWaypointIndex/* = nullptr*/)
{
	DWORD dwIndex;
	float dist;

	// 매 지역에서 다음 지역까지 이동 경로를 설정한다
	if (_setting.targetPathType == TPT_None)
	{
		dwIndex = GetMinimapIconCacheIndex(WAYPOINT_NAME, WAYPOINT_NAME, L"", true);
		if (dwIndex != NOINDEX)
		{
			Log(L"W: Target Path -> [%s], %s", _map.iconsCache[dwIndex].name.c_str(), _map.iconsCache[dwIndex].path.c_str());
			if (LoadMinimapIconTargetPath(dwIndex))
			{
				_setting.targetPathType = TPT_GotoWaypoint;
				return 1;
			}
		}
	}

	dwIndex = GetNearestObjectByRender(_localPlayer->positionedComp.gridPos, 5.f, WAYPOINT_NAME, false);
	if (dwIndex != NOINDEX)
	{
		dist = DIST(_localPlayer->positionedComp.gridPos, _entityList->others[dwIndex].positionedComp.gridPos);
		if (isRegister)		// 등록만 하는 경우는 창을 닫는다
			_setting.firstOpenedWorldMap = 2;
		else
		{
			if (_ingameUi->worldmap.isVisible && dist < 2.f)
			{
				*outWaypointIndex = dwIndex;
				return 2;
			}
		}
		Log(L"W: Select [%s]", _entityList->others[dwIndex].render.name.c_str());
		SelectEntity(_entityList->others[dwIndex].id);
		SetDelay(20);
		return 1;
	}

	return 0;
}

bool GameLogic::ContinueDialog()
{
	if (GameStateController::InGameState.ingameUi.interaction.isVisible && GameStateController::InGameState.ingameUi.interaction.childCount == 3)
	{
		Log(L"Continue");
		PressKeyAlternate(VK_ESCAPE);
		SetDelay(10);
		return true;
	}

	return false;
}

bool GameLogic::ProcessReward()
{
	DWORD i, j;
	if (_ingameUi->reward.isVisible)
	{
		if (_controller->npcInventories.size() == 0)
			return true;
		InventoryHolder *pInven = &_controller->npcInventories[0];
		for (i = 0; i < pInven->inventory.slotItems.size(); i++)
		{
			InvenSlotItem *pItem = &pInven->inventory.slotItems[i];
			for (j = 0; j < g_pInfoQuestReward.size(); j++)
			{
				if (pItem->item.displayName == g_pInfoQuestReward[j])
					break;
			}
			if (j >= g_pInfoQuestReward.size())
				continue;

			int dstCellX, dstCellY;
			if (GetEmptyCellByItemData(&pItem->item, false, &dstCellX, &dstCellY))
			{
				Log(L"Receive reward [%s]", pItem->item.displayName.c_str());
				ReceiveReward(i, MainInventory1, dstCellX, dstCellY);
				SetDelay(10);
				return true;
			}
		}

		return true;
	}

	return false;
}

void GameLogic::QuestRun()
{
	DWORD dwIndex = NOINDEX, i, j = 0;
	float dist, distMin = 1000000000.f, range = 2.0f;
	QuestInfo *currentQuest;
	BYTE doQuest = 0;
	bool newFullPath = false;

	LoadObjectCache();

	// 맵이 새로 로딩된 경우
	if (_setting.newMapBlock)
	{
		_setting.newMapBlock = 0;
		_setting.prevEntranceCacheSize = 0;
		_setting.fullPathLoadCount = 0;
		newFullPath = true;
	}

	// 같은 지역에서 다른 부분으로 넘어온 경우
	if (_setting.selectedObjectID && GetNearestObjectByID(_localPlayer->positionedComp.gridPos, 10.f, _setting.selectedObjectID, false) == NOINDEX)
	{
		if (_setting.toPartIndex == NOINDEX - 1 && _map.entranceCache.size() >= 2)		// 잘못 올라온 경우 다시 내려가는 처리
		{
			_map.skipEntrances.push_back(_map.entranceCache[_map.entranceCache.size() - 2]);
			_map.entranceCache.pop_back(); //출구점
			_map.entranceCache.pop_back(); //입구점
			_setting.currentAreaPart--;
			Log(L"Skip exist entrance  - %d", (int)_map.entranceCache.size());
		}
		else
		{
			if (_setting.toPartIndex > _setting.currentAreaPart)
				_setting.currentAreaPart++;
			else if (_setting.toPartIndex < _setting.currentAreaPart)
				_setting.currentAreaPart--;
		}

		_setting.selectedObjectID = 0;
		_setting.prevEntranceCacheSize = (DWORD)_map.entranceCache.size();
		Log(L"Next Part = %d, To = %08X, PrevCount = %d", _setting.currentAreaPart, _setting.toPartIndex, _setting.prevEntranceCacheSize);
		_setting.fullPathLoadCount = 0;
		_setting.toPartIndex = NOINDEX;
		newFullPath = true;
	}

	if (newFullPath)
	{
		_setting.InitPath();
		LoadFullPath();
		InitObjectCacheGoState();
	}

	if (_setting.isDead)
	{
		Log(L"Target Path -> Death (%d, %d) [%s]", _setting.posDead.x, _setting.posDead.y, _worldArea->name.c_str());

		if (LoadTargetPath(_setting.posDead))
		{
			_setting.targetPathType = TPT_GotoDeadPos;
		}
		_setting.currentAreaPart = 0;

		_setting.InitDeath();
	}

	//_setting.currentQuestIndex = GetCurrentQuestIndex(true);
	//if (_setting.currentQuestIndex == NOINDEX)
	_setting.currentQuestIndex = GetCurrentQuestIndex(false);
	if (_setting.currentQuestIndex == NOINDEX)
	{
		Log(L"Can't find the current quest!");
		ToggleRun(0);
		return;
	}
	currentQuest = &g_pInfoQuest[_setting.currentQuestIndex];
	if (_setting.prevQuestIndex != _setting.currentQuestIndex)
	{
		if (_setting.prevQuestIndex == 0x20190906)
		{
			if (currentQuest->subMode == IK_Other)
			{
				_setting.prevQuestIndex = ++_setting.currentQuestIndex;
				currentQuest = &g_pInfoQuest[_setting.currentQuestIndex];
			}
			else if (currentQuest->subMode == IK_Npc_Spec)
			{
				BYTE bInterection = InteractionNpc(currentQuest->name);
				if (bInterection == 0)
				{
					_setting.currentQuestIndex = _setting.currentQuestIndex + currentQuest->reserve;
					_setting.prevQuestIndex = _setting.currentQuestIndex;
					currentQuest = &g_pInfoQuest[_setting.currentQuestIndex];
				}
				else if (bInterection == 2)
				{
					BYTE bOtherInterection = InteractionOther(g_pInfoQuest[_setting.currentQuestIndex + 1].name);
					if (bOtherInterection == 2)
					{
						_setting.currentQuestIndex = _setting.currentQuestIndex + currentQuest->reserve;
						_setting.prevQuestIndex = _setting.currentQuestIndex;
						currentQuest = &g_pInfoQuest[_setting.currentQuestIndex];
					}
					else
					{
						_setting.prevQuestIndex = ++_setting.currentQuestIndex;
						currentQuest = &g_pInfoQuest[_setting.currentQuestIndex];
					}
				}
			}
		}
		else
		{
			Log(L"New quest-%d [%s], %d, %d", _setting.currentQuestIndex, currentQuest->questName, currentQuest->questKey, currentQuest->questStep);
			_setting.prevQuestIndex = _setting.currentQuestIndex;
		}
	}

	if (_worldArea->name == currentQuest->areaName)
	{
		doQuest = 1;
		_setting.InitGlobalMove();
	}
	else
	{
		// 현재 설정되여 있는 대역경로가 맞지 않는 경우 재설정
		if (_setting.globalAreas.size() == 0 || wcscmp(g_pResWorldArea[_setting.globalAreas[_setting.globalAreas.size() - 1]].szName,  currentQuest->areaName))
			_setting.InitGlobalMove();

		// 목적지까지의 가기 위해 거치는 최단 경로 지역들을 얻는다
		if (!_setting.globalMove)
		{
			_setting.globalMove = 1;
			Log(L"Global move from [%s] to [%s]", _worldArea->name.c_str(), currentQuest->areaName);
			_setting.globalAreas = FindAreaPath(_worldArea->name, currentQuest->areaName);
			for (const auto& area : _setting.globalAreas)
				Log(L"%s", g_pResWorldArea[area].szName);
			if (_setting.globalAreas.empty())
			{
				Log(L"Can't go from [%s] to [%s]", _worldArea->name.c_str(), currentQuest->areaName);
				ToggleRun(0);
				return;
			}
		}
	}

	if (!_currentPath)
	{
		LoadFullPath();
		return;
	}

	if (_moveStep == NOINDEX)
	{
		distMin = 1000000000.f;
		dwIndex = NOINDEX;
		for (i = _currentPathSize - 1; (int)i >= 0; i--)
		{
			dist = DIST(_localPlayer->positionedComp.gridPos, _currentPath[i]);
			if (dist <= distMin)
			{
				distMin = dist;
				dwIndex = i;
			}
		}

		if (dwIndex == NOINDEX)
		{
			Log(L"Failed to find the current step on the path.");
			ToggleRun(0);
			return;
		}

		Log(L"MoveStep = %d", dwIndex);
		_moveStep = dwIndex;
	}

	if (_moveStep >= _currentPathSize)
	{
		if (_setting.targetPathType != TPT_None)
		{
			Log(L"Arrived last target point [%d].", _setting.targetPathType);
			_currentPath = _fullPath;
			_currentPathSize = _fullPathSize;
			_moveStep = NOINDEX;
			_setting.targetPathType = TPT_None;
			return;
		}
		_setting.fullPathLoadCount++;
		Log(L"Arrived full path last point. %d", _setting.fullPathLoadCount);
		_currentPath = nullptr;
		_map.findPathStep -= 10;
		if (_map.findPathStep < 20)
			_map.findPathStep = 20;

		return;
	}

	// 튜토리얼
	if (currentQuest->questKey == 0 && currentQuest->questStep == -1)
	{
		if (_ingameUi->skipTutorial.isVisible)
		{
			Log(L"Skip tutorial.");
			SkipTutorial(_serverData->Address, _ingameUi->skipTutorial.Address);
			SetDelay(10);
			return;
		}
	}

	if (UsePortion())
		return;

	if (UseBuffSkill())
		return;

	if (OpenSpecialDoors())
	{
		_setting.prevMoveTick = _tick;
		return;
	}

	if (OpenDoors())
		return;	

	// Attack near monster
	dwIndex = GetNearestMonster(_localPlayer->positionedComp.gridPos, 6.f);
	if (dwIndex != NOINDEX)
	{
		//TESTLOG(L"Attack_Near [%s, %d, %.1f]",
		//	_entityList->monsters[dwIndex].path.c_str(),
		//	_entityList->monsters[dwIndex].life.curHP,
		//	DIST(_localPlayer->positionedComp.gridPos, _entityList->monsters[dwIndex].positionedComp.gridPos));
		bool hasMonsterName = false;
		if (currentQuest->mode == QM_Hunt)			
		{
			auto monsterNames = SplitWideString(currentQuest->name, L'@');
			DWORD dwQuestMonster;
			for each (auto name in monsterNames)	//주변에 공격해야할 몬스터가 있는가를 확인한다.
			{
				dwQuestMonster = GetNearestMonster(_localPlayer->positionedComp.gridPos, 10.f, name);
				if (dwQuestMonster != NOINDEX)
				{
					hasMonsterName = true;
					break;
				}
			}
		}
		if(!hasMonsterName)
		{
			AttackTarget(_entityList->monsters[dwIndex].positionedComp.gridPos, 6.f, _entityList->monsters[dwIndex].id);
			return;
		}
	}

	if (ApplyPassiveSkills())
		return;

	if (CleanInven())
		return;

	if (AutoEquip())
	{
		_setting.prevMoveTick = _tick;
		return;
	}

	if (PickItems())
		return;

	if (OpenChests())
		return;


	//Log(L"Quest key = %d, step = %d, mode = %d, name = %s", 
	//	currentQuest->questKey, currentQuest->questStep, currentQuest->mode, currentQuest->name);

	if (ContinueDialog())
		return;

	// 튜토리얼 지역 특수 처리
	if (_worldArea->name == g_pInfoQuest[0].areaName)
	{
		dwIndex = GetNearestNpc(_localPlayer->positionedComp.gridPos, 7.f, g_pInfoQuest[0].szReserve);
		if (dwIndex != NOINDEX)
		{
			TESTLOG(L"Interact npc_tutorial [%s]", _entityList->npcs[dwIndex].name.c_str());
			SelectEntity(_entityList->npcs[dwIndex].id);
			SetDelay(20);
			return;
		}
	}

	// 보상 받기
	if (ProcessReward())
		return;

	if (currentQuest->mode != QM_Interaction || currentQuest->subMode != IK_Npc_Element)
	{
		if (GameStateController::InGameState.ingameUi.interaction.isVisible && GameStateController::InGameState.ingameUi.interaction.childCount == 2)
		{
			Log(L"Goodbye");
			PressKeyAlternate(VK_ESCAPE);
			SetDelay(10);
			return;
		}
	}

	// 마을에서 처리 (아이템 판매, 구매, 창고 맡기기)
	if (ProcessInTown())
		return;

	// 거점 등록
	if (RegisterWaypoint())
		return;

	BYTE gotoEntrace = 0;

	// 같은 지역에서 다른 부분 이동
	auto dwSpecialAreaIndex = GetSpecialAreaIndex(_worldArea->name);
	if (dwSpecialAreaIndex != NOINDEX)
	{
		gotoEntrace = 1;
	
		// 현재 부분지역 번호를 결정한다
		if (_setting.currentAreaPart >= g_pInfoSpecialArea[dwSpecialAreaIndex].partCount)
			_setting.currentAreaPart = 0;
	
		// 우선 갈수 있는 식별가능한 입구가 있는가를 판단한다
		for (i = 0; i < g_pInfoSpecialArea[dwSpecialAreaIndex].minimapIcons.size(); i++)
		{
			if (!g_pInfoSpecialArea[dwSpecialAreaIndex].minimapIcons[i].unique)
				continue;
	
			// 해당 입구까지 갈수있는가 판단
			dwIndex = GetMinimapIconCacheIndex(g_pInfoSpecialArea[dwSpecialAreaIndex].minimapIcons[i].szIconName, g_pInfoSpecialArea[dwSpecialAreaIndex].minimapIcons[i].szName, g_pInfoSpecialArea[dwSpecialAreaIndex].minimapIcons[i].szPath, true);
			if (dwIndex != NOINDEX)
			{
				if (_setting.currentAreaPart != g_pInfoSpecialArea[dwSpecialAreaIndex].minimapIcons[i].partIndex)
				{
					_setting.currentAreaPart = g_pInfoSpecialArea[dwSpecialAreaIndex].minimapIcons[i].partIndex;
					Log(L"Current Area Part1 = %d", _setting.currentAreaPart);
				}
				break;
			}
		}
		//if (_setting.currentAreaPart == NOINDEX)
		//{
		//	// 갈수 있는 입구가 있는가를 판단한다
		//	for (i = 0; i < g_pInfoSpecialArea[dwSpecialAreaIndex].minimapIcons.size(); i++)
		//	{
		//		if (g_pInfoSpecialArea[dwSpecialAreaIndex].minimapIcons[i].unique)
		//			continue;
		//	
		//		// 해당 입구까지 갈수있는가 판단
		//		dwIndex = GetMinimapIconCacheIndex(g_pInfoSpecialArea[dwSpecialAreaIndex].minimapIcons[i].szIconName, g_pInfoSpecialArea[dwSpecialAreaIndex].minimapIcons[i].szName, g_pInfoSpecialArea[dwSpecialAreaIndex].minimapIcons[i].szPath, true);
		//		if (dwIndex != NOINDEX)
		//		{
		//			_setting.currentAreaPart = g_pInfoSpecialArea[dwSpecialAreaIndex].minimapIcons[i].partIndex;
		//			break;
		//		}
		//	}
		//
		//	if (_setting.currentAreaPart != NOINDEX)
		//		_setting.currentAreaPart = 0;
		//
		//	Log(L"Current Area Part2 = %d", _setting.currentAreaPart);
		//}
	
		// 가려는 입구를 결정한다
		if (_setting.currentAreaPart != NOINDEX)
		{
			if (_setting.globalMove)		// 대역이동인 경우
			{
				DWORD dwAreaIndex = GetGlobalAreaIndexByName(_worldArea->name);
				if (dwAreaIndex == NOINDEX || dwAreaIndex + 1 >= _setting.globalAreas.size())		// Path find error
				{
					Log(L"Global Move failed2 [%s]", _worldArea->name.c_str());
					_setting.InitGlobalMove();
					return;
				}

				//목적지가 마을이 아니면서 현재 지역이 층으로 되여있고 텔포가 있는 경우 포탈을 사용하여 마을로 갔다가 목적지로 간다.
				if (_worldArea->hasWaypoint && g_pInfoSpecialArea[dwSpecialAreaIndex].usePortal && 
					_setting.currentAreaPart != g_pInfoSpecialArea[dwSpecialAreaIndex].waypointPartIndex && !g_pResWorldArea[_setting.globalAreas[_setting.globalAreas.size() - 1]].bIsTown)
				{
					if (UsePortalScroll())
						return;
				}
	
				// 대역 경로에서 다음 지역에 갈수있는 부분번호를 얻는다
				for (i = 0; i < g_pInfoSpecialArea[dwSpecialAreaIndex].minimapIcons.size(); i++)
				{
					if (g_pInfoSpecialArea[dwSpecialAreaIndex].minimapIcons[i].next != 2)
						continue;
					if (!wcscmp(g_pInfoSpecialArea[dwSpecialAreaIndex].minimapIcons[i].szName, g_pResWorldArea[_setting.globalAreas[dwAreaIndex + 1]].szName))
					{
						_setting.toPartIndex = g_pInfoSpecialArea[dwSpecialAreaIndex].minimapIcons[i].partIndex;
						break;
					}
				}
			}
			else		// 대역이동이 아닌경우 다음으로 간다
			{
				if (_worldArea->name == currentQuest->areaName)
				{
					// 목적하는 층과 다른곳이라면
					if (_setting.fullPathLoadCount >= 2 && _setting.currentAreaPart > 0 && _setting.prevEntranceCacheSize == _map.entranceCache.size())
					{
						_setting.toPartIndex = NOINDEX - 1;
					}
					else
						_setting.toPartIndex = currentQuest->areaPartIndex;
				}
				else
				{
					_setting.toPartIndex = _setting.currentAreaPart + 1;
				}
				if (_setting.toPartIndex != NOINDEX - 1 && _setting.toPartIndex >= g_pInfoSpecialArea[dwSpecialAreaIndex].partCount)
					_setting.toPartIndex = g_pInfoSpecialArea[dwSpecialAreaIndex].partCount - 1;
			}

			// 다른 부분지역으로 
			if (_setting.toPartIndex != NOINDEX && _setting.toPartIndex != _setting.currentAreaPart)
			{
				for (i = 0; i < _map.entranceCache.size(); i++)
				{
					if (_setting.toPartIndex == NOINDEX - 1)
					{
						if (i % 2 == 0)
							continue;
					}
					else
					{
						if (_setting.toPartIndex > _setting.currentAreaPart && i % 2 == 1)
							continue;
						if (_setting.toPartIndex < _setting.currentAreaPart && i % 2 == 0)
							continue;
					}

					dwIndex = GetNearestObjectByID(_localPlayer->positionedComp.gridPos, 100.f, _map.entranceCache[i], false);
					if (dwIndex != NOINDEX)
					{
						if (_setting.targetPathType == TPT_None)
						{
							if (LoadTargetPath(_entityList->others[dwIndex].positionedComp.gridPos))
							{
								Log(L"Part: Target Path -> [%s]", _entityList->others[dwIndex].render.name.c_str());
								_setting.targetPathType = TPT_GotoEntrance;
								return;
							}
						}

						if (DIST(_localPlayer->positionedComp.gridPos, _entityList->others[dwIndex].positionedComp.gridPos) < 5.f)
						{
							Log(L"Part: Select Entrance [%s]-%d", _entityList->others[dwIndex].render.name.c_str(), _setting.toPartIndex);
							SelectEntity(_entityList->others[dwIndex].id);
							_setting.selectedObjectID = _entityList->others[dwIndex].id;
							SetDelay(20);
							return;
						}
					}
					
				}
			}
		}
	}

	// 대역 이동
	if (_setting.globalMove)
	{
		gotoEntrace = 1;
		DWORD dwAreaIndex = GetGlobalAreaIndexByName(_worldArea->name);
		if (dwAreaIndex == NOINDEX || dwAreaIndex + 1 >= _setting.globalAreas.size())		// Path find error
		{
			Log(L"Global Move failed [%s]", _worldArea->name.c_str());
			_setting.InitGlobalMove();
			return;
		}

		// 대역 경로중에 마을이 있다면 
		for (i = (int)_setting.globalAreas.size() - 1; (int)i > (int)dwAreaIndex; i--)
		{
			if (std::find(g_pAreasInfoSkipScroll.begin(), g_pAreasInfoSkipScroll.end(), _worldArea->name) != g_pAreasInfoSkipScroll.end())
				continue;
			if (g_pResWorldArea[_setting.globalAreas[i]].bIsTown && g_pResWorldArea[_setting.globalAreas[i]].bAct == _worldArea->act)
				break;
		}
		if ((int)i > (int)dwAreaIndex)
		{
			if (UsePortalScroll())
				return;
		}

		// 현재 지역에 거점이 있다면 목표까지 거점을 이용한다
		if (_worldArea->hasWaypoint)
		{
			for (i = (int)_setting.globalAreas.size() - 1; (int)i > (int)dwAreaIndex; i--)
			{
				if (std::find(_ingameUi->teleportFieldIds.begin(), _ingameUi->teleportFieldIds.end(), g_pResWorldArea[_setting.globalAreas[i]].dwID) != _ingameUi->teleportFieldIds.end())
				{
					break;
				}
			}
			if ((int)i > (int)dwAreaIndex)		// 도중 지역중에 갈수 있는 거점이 있는 경우
			{
				BYTE gotoWayPoint = GotoWaypoint(false, &dwIndex);
				if (gotoWayPoint)
				{
					if (gotoWayPoint == 2)		// 거점이 선택된 경우
					{
						Log(L"W: Go to [%s]", g_pResWorldArea[_setting.globalAreas[i]].szName);
						ChangeAreaByWaypoint(_serverData->Address, _entityList->others[dwIndex].id, g_pResWorldArea[_setting.globalAreas[i]].dwID);
						auto dwNextSpecialIndex = GetSpecialAreaIndex(g_pResWorldArea[_setting.globalAreas[i]].szName);
						if (dwNextSpecialIndex != NOINDEX)
						{
							_setting.currentAreaPart = g_pInfoSpecialArea[dwNextSpecialIndex].waypointPartIndex;
							Log(L"Special Part = %d", _setting.currentAreaPart);
						}
						SetDelay(50);
					}
					return;
				}
			}
		}

		// 매 지역에서 다음 지역까지 이동 경로를 설정한다
		if (_setting.targetPathType == TPT_None)
		{
			dwIndex = GetMinimapIconCacheIndex(ENTRANCE_NAME, g_pResWorldArea[_setting.globalAreas[dwAreaIndex + 1]].szName, L"", true);
			if (dwIndex != NOINDEX)
			{
				Log(L"Global: Target Path -> [%s], %s", _map.iconsCache[dwIndex].name.c_str(), _map.iconsCache[dwIndex].path.c_str());
				if (LoadMinimapIconTargetPath(dwIndex))
				{
					_setting.targetPathType = TPT_GotoEntrance;
					return;
				}
			}
		}

		if (_setting.targetPathType == TPT_GotoEntrance && _moveStep == _currentPathSize - 1)
		{
			dwIndex = GetNearestObjectByRender(_localPlayer->positionedComp.gridPos, 5.f, g_pResWorldArea[_setting.globalAreas[dwAreaIndex + 1]].szName, false);
			if (dwIndex != NOINDEX)
			{
				Log(L"Global: Select Entrance [%s]", _entityList->others[dwIndex].render.name.c_str());
				SelectEntity(_entityList->others[dwIndex].id);
				SetDelay(50);
				return;
			}
		}
	}

	// 매 지역에서 이름이 없는 입구까지 이동
	if (gotoEntrace)
	{
		if (_setting.targetPathType == TPT_None)
		{
			dwIndex = GetMinimapIconCacheIndex(ENTRANCE_NAME, L"", L"", true);
			if (dwIndex != NOINDEX)
			{
				Log(L"Unknown Entrance: Target Path -> (%d, %d)", _map.iconsCache[dwIndex].pos.gridPos.x, _map.iconsCache[dwIndex].pos.gridPos.y);
				if (LoadMinimapIconTargetPath(dwIndex))
				{
					_setting.targetPathType = TPT_GotoEntrance;
					return;
				}
			}
		}
	}

	if (doQuest)
	{
		if (_setting.isQuestDone == 2 && currentQuest->isSkip)
		{
			_setting.questsDone.push_back(_setting.currentQuestIndex);
			_setting.InitQuest();
			return;
		}

		if (_setting.targetPathType == TPT_None)
		{
			// 목표 NPC까지의 경로를 적재한다
			if ((currentQuest->mode == QM_Interaction && currentQuest->subMode == IK_Npc) || currentQuest->mode == QM_Reward)
			{
				dwIndex = GetMinimapIconCacheIndex(NPC_NAME, currentQuest->name, L"", true);
				if (dwIndex != NOINDEX)
				{
					Log(L"Target Path - NPC -> [%s]", _map.iconsCache[dwIndex].name.c_str());
					if (LoadMinimapIconTargetPath(dwIndex))
					{
						_setting.targetPathType = TPT_GotoNpc;
						return;
					}
				}
			}
			else if (currentQuest->mode == QM_Interaction && currentQuest->subMode == IK_Other)
			{
				dwIndex = GetMinimapIconCacheIndex(QUESTOBJECT_NAME, currentQuest->name, L"", true);
				if (dwIndex != NOINDEX)
				{
					Log(L"Target Path - QuestObject -> [%s]", _map.iconsCache[dwIndex].name.c_str());
					if (LoadMinimapIconTargetPath(dwIndex))
					{
						_setting.targetPathType = TPT_GotoObject;
						return;
					}
				}
			}
			//else
			{
				dwIndex = GetMinimapIconCacheIndex(QUESTITEM_NAME, L"", L"", true);
				if (dwIndex == NOINDEX)
					dwIndex = GetMinimapIconCacheIndex(QUESTOBJECT_NAME, L"", L"", true);
				if (dwIndex != NOINDEX)
				{
					Log(L"Unknown Target Path - %s", _map.iconsCache[dwIndex].iconName.c_str());
					if (LoadMinimapIconTargetPath(dwIndex))
					{
						_setting.targetPathType = TPT_GotoObject;
						return;
					}
				}
			}
		}

		switch (currentQuest->mode)
		{
		case QM_WAYPOINT:
		{
			if (!_worldArea->hasWaypoint)	//waypoint를 가지고 있지 않는 맵이라면 다음 퀘스트수행
			{
				_setting.prevQuestIndex = _setting.currentQuestIndex + 1;
				return;
			}
			else
			{
				if (std::find(_ingameUi->teleportFieldIds.begin(), _ingameUi->teleportFieldIds.end(), _worldArea->worldAreaID) != _ingameUi->teleportFieldIds.end())
				{
					_setting.prevQuestIndex = _setting.currentQuestIndex + 1;
					return;
				}
			}
		}
		break;
		case QM_FullScan:
		{

		}
		break;
		case QM_Interaction:
		case QM_Reward:
		{
			if (currentQuest->subMode == IK_Npc || currentQuest->mode == QM_Reward)
			{
				dwIndex = GetNearestNpc(_localPlayer->positionedComp.gridPos, 7.f, currentQuest->name, currentQuest->reserve == 0);
				if (dwIndex != NOINDEX)
				{
					if (_setting.isQuestDone == 1 && currentQuest->isSkip && !_entityList->npcs[dwIndex].npc.hasIconOverhead)
					{
						_setting.isQuestDone = 2;
					}
					else
					{
						TESTLOG(L"Interact npc [%s]", _entityList->npcs[dwIndex].name.c_str());
						SelectEntity(_entityList->npcs[dwIndex].id);
						_setting.isQuestDone = 1;
						SetDelay(20);
					}

					return;
				}
			}
			else if (currentQuest->subMode == IK_Other)
			{
				if (InteractionOther(currentQuest->name) == 1)
				{
					return;
				}
				if (InteractionOther(currentQuest->name) == 2)
				{
					_setting.prevQuestIndex = 0x20190906;
					return;
				}
			}
			else if (currentQuest->subMode == IK_Npc_Spec)
			{
				_setting.prevQuestIndex = 0x20190906;
				return;
			}
			else if (currentQuest->subMode == IK_Chest)
			{
				dwIndex = GetNearestChest(_localPlayer->positionedComp.gridPos, 5.f, currentQuest->name);
				if (dwIndex != NOINDEX)
				{
					TESTLOG(L"Interact Chest [%s]", _entityList->chests[dwIndex].name.c_str());
					SelectEntity(_entityList->chests[dwIndex].id);
					SetDelay(20);
					return;
				}
			}
			else if (currentQuest->subMode == IK_Npc_Element)
			{
				dwIndex = GetNearestNpc(_localPlayer->positionedComp.gridPos, 7.f, currentQuest->name);
				if (dwIndex != NOINDEX)
				{
					if (GameStateController::InGameState.ingameUi.interaction.isVisible && GameStateController::InGameState.ingameUi.interaction.childCount == 2)
					{
						for (int i = 0; i < g_pInfoClickNpcs.size(); i++)
						{
							if (currentQuest->name == g_pInfoClickNpcs[i].TraderNpcName)
							{
								Log(L"Click %s Element", currentQuest->name);
								NpcElementClick(g_pInfoClickNpcs[i].NpcElement);
								SetDelay(10);
								return;
							}
						}
					}

					TESTLOG(L"Interact npc [%s]", _entityList->npcs[dwIndex].name.c_str());
					SelectEntity(_entityList->npcs[dwIndex].id);
					SetDelay(10);
					return;
				}
			}
		}
		break;
		case QM_Hunt:
		{
			if (!wcslen(currentQuest->szReserve))	//통과해야 할 문이 없다면 일반 사냥을 한다.
			{				
				auto monsterNames = SplitWideString(currentQuest->name, L'@');
				for each (auto name in monsterNames)	//주변에 공격해야할 몬스터가 있는가를 확인한다.
				{
					dwIndex = GetNearestMonster(_localPlayer->positionedComp.gridPos, 10.f, name);
					if (dwIndex != NOINDEX)
					{
						AttackTarget(_entityList->monsters[dwIndex].positionedComp.gridPos, 10.f, _entityList->monsters[dwIndex].id);
						return;
					}
				}
			}
			else //통과해야할 문이 있다면
			{
				DWORD monsterIdx = 0, enteranceIdx = 0;
				auto monsterNames = SplitWideString(currentQuest->name, L'@');
				for each (auto name in monsterNames)	//주변에 공격해야할 몬스터가 있는가를 확인한다.
				{
					dwIndex = GetNearestMonster(_localPlayer->positionedComp.gridPos, 10.f, name);
					if (dwIndex != NOINDEX)
						break;
					monsterIdx++;
				}
				if (monsterIdx >= monsterNames.size()) //공격해야할 몬스터가 없다면 일반 사냥을 한다.
				{
					dwIndex = GetNearestMonster(_localPlayer->positionedComp.gridPos, 6.f);
					if (dwIndex != NOINDEX)
					{
						AttackTarget(_entityList->monsters[dwIndex].positionedComp.gridPos, 10.f, _entityList->monsters[dwIndex].id);
						return;
					}
					break;
				}

				auto enteranceNames = SplitWideString(currentQuest->szReserve, L'@');
				for each (auto name in enteranceNames) //해당 몬스터방에 입장하여야 할 문을 통과하였는가를 검사한다.
				{
					if(monsterIdx == enteranceIdx) //현재 몬스터에 들어가는 문을 통과하지 못햇다면
					{
						if(!IsOpenedSpecialDoors(name))
							enteranceIdx = NOINDEX;
						break;
					}
					enteranceIdx++;
				}
				if (enteranceIdx == monsterIdx) //몬스터방문을 통과하였다면 공격한다.
				{
					dwIndex = GetNearestMonster(_localPlayer->positionedComp.gridPos, 10.f, monsterNames[monsterIdx]);
					if (dwIndex != NOINDEX)
					{
						AttackTarget(_entityList->monsters[dwIndex].positionedComp.gridPos, 10.f, _entityList->monsters[dwIndex].id);
						return;
					}
				}
			}
		}
		break;
		case QM_InteractAndHunt:
		{
			dwIndex = GetNearestMonster(_localPlayer->positionedComp.gridPos, 6.f, currentQuest->name);
			if (dwIndex != NOINDEX)
			{
				//TESTLOG(L"Attack_Quest [%s, %d, %.1f]",
				//	_entityList->monsters[dwIndex].path.c_str(),
				//	_entityList->monsters[dwIndex].life.curHP,
				//	DIST(_localPlayer->positionedComp.gridPos, _entityList->monsters[dwIndex].positionedComp.gridPos));
				//AttackTarget(_currentPath[_moveStep], 20.f, _entityList->monsters[dwIndex].id);
				AttackTarget(_entityList->monsters[dwIndex].positionedComp.gridPos, 20.f, _entityList->monsters[dwIndex].id);
				return;
			}
			else if (currentQuest->name[0])
			{
				if (_ingameUi->specialProcMonsterPanel.isVisible)
				{
					KillSpecialMonster(_serverData->Address, 0);
					return;
				}
				if (InteractionMonster(currentQuest->name, 7.f))
					return;
			}
		}
		break;
		case QM_GotoArea:
		{
			//dwIndex = GetNearestObjectByRender(_localPlayer->positionedComp.gridPos, 5.f, currentQuest->name, false);
			//if (dwIndex != NOINDEX)
			//{
			//	Log(L"SelectArea [%s]", _entityList->others[dwIndex].render.name.c_str());
			//	SelectEntity(_entityList->others[dwIndex].id);
			//	SetDelay(50);
			//	return;
			//}
		}
		break;
		case QM_PickupItem:
		{
			dwIndex = GetNearestWorldItem(_localPlayer->positionedComp.gridPos, 7.f, currentQuest->name);
			if (dwIndex != NOINDEX)
			{
				if (GetEmptyCellByItemData(&_entityList->worldItems[dwIndex].item, false))
				{
					Log(L"Pickup quest item [%s]", _entityList->worldItems[dwIndex].item.displayName.c_str());
					SelectEntity(_entityList->worldItems[dwIndex].id);
					SetDelay(10);
					return;
				}
			}
		}
		break;
		case QM_ItemsAndWall:
		{
			auto questItems = SplitWideString(currentQuest->szReserve, L'@');

			// 모든 퀘스트 아이템을 다 가지고 있는 경우 목적지로 간다
			bool hadAllItems = true;
			for each (auto item in questItems)
			{
				dwIndex = GetItemIndexByName(MainInventory1, item);
				if (dwIndex == NOINDEX)
				{
					hadAllItems = false;
					break;
				}
			}
			if (hadAllItems)
			{
				// 목표 object까지의 경로를 적재한다
				if (_setting.targetPathType == TPT_None)
				{
					dwIndex = GetMinimapIconCacheIndex(QUESTOBJECT_NAME, currentQuest->name, L"", true);
					if (dwIndex != NOINDEX)
					{
						Log(L"Target Path - Object -> [%s]", _map.iconsCache[dwIndex].name.c_str());
						if (LoadMinimapIconTargetPath(dwIndex))
						{
							_setting.targetPathType = TPT_GotoObject;
							return;
						}
					}
				}

				// 목표 object가 보이면 선택
				dwIndex = GetNearestObjectByRender(_localPlayer->positionedComp.gridPos, 5.f, currentQuest->name, false);
				if (dwIndex != NOINDEX)
				{
					TESTLOG(L"Select Object [%s]", _entityList->others[dwIndex].render.name.c_str());
					SelectEntity(_entityList->others[dwIndex].id);
					SetDelay(30);
					return;
				}
			}

			// 퀘스트아이템들을 줍는다
			for each (auto item in questItems)
			{
				dwIndex = GetNearestWorldItem(_localPlayer->positionedComp.gridPos, 7.f, item);
				if (dwIndex != NOINDEX)
				{
					if (GetEmptyCellByItemData(&_entityList->worldItems[dwIndex].item, false))
					{
						TESTLOG(L"Pickup quest item-2 [%s]", _entityList->worldItems[dwIndex].item.displayName.c_str());
						SelectEntity(_entityList->worldItems[dwIndex].id);
						SetDelay(10);
						return;
					}
				}
			}
			
		}
		break;
		default:
			break;
		}
	}

	dist = DIST(_localPlayer->positionedComp.gridPos, _currentPath[_moveStep]);
	if (dist > range)
	{
		// 이동
		TESTLOG(L"Move (%d, %d), (%d, %d) %.1f", _localPlayer->positionedComp.gridPos.x, _localPlayer->positionedComp.gridPos.y, _currentPath[_moveStep].x, _currentPath[_moveStep].y, dist);
		// 카오 처리
		if (DIST(_setting.prevMovePos, _localPlayer->positionedComp.gridPos) < 1.0f)
		{
			if (_setting.prevMoveTick && _tick > _setting.prevMoveTick + 10000)		// 10초 동안 이동하지 못하면 
			{
				Log(L"카오 발견 (%d, %d) %.1f", _localPlayer->positionedComp.gridPos.x, _localPlayer->positionedComp.gridPos.y, dist);
				if (_setting.targetPathType == TPT_None)
				{
					distMin = 100000.f;
					dwIndex = NOINDEX;
					for (i = _currentPathSize - 1; (int)i > _moveStep; i--)
					{
						dist = DIST(_localPlayer->positionedComp.gridPos, _currentPath[i]);
						if (dist <= distMin)
						{
							distMin = dist;
							dwIndex = i;
						}
					}

					Log(L"Chao: MoveStep = %d, DistMin = %.1f", dwIndex, distMin);
					_moveStep = dwIndex;
					if (dwIndex == NOINDEX || distMin > 10.f)
					{
						Log(L"Full Path error!");
						_currentPath = nullptr;
						_moveStep = NOINDEX;
					}
				}

				_setting.InitMove();
			}
			else if (_setting.prevMoveTick && _tick > _setting.prevMoveTick + 4000)		// 4초 동안 이동하지 못하면 
			{
				dist = DIST(_localPlayer->positionedComp.gridPos, _currentPath[_moveStep]);
				if (dist > 12.f)
				{
					if (LoadTargetPath(_currentPath[_moveStep]))
					{
						Log(L"Chao: Target Path %.1f", dist);
						_setting.targetPathType = TPT_ChaoPos;
						return;
					}
				}

				if (UseForwardSkill(_currentPath[_moveStep]))
				{
					Log(L"질주 사용");
					return;
				}
				IVector randomPos;
				randomPos.x = _currentPath[_moveStep].x - 15 + (rand() % 30);
				randomPos.y = _currentPath[_moveStep].y - 15 + (rand() % 30);
				MoveToPos(randomPos);
				Log(L"이동점 변경 - (%d, %d) %.1f, %.1f", randomPos.x, randomPos.y, DIST(_localPlayer->positionedComp.gridPos, randomPos), DIST(_currentPath[_moveStep], randomPos));
				SetDelay(10);
				return;
			}
		}		
		else
		{
			_setting.prevMovePos = _localPlayer->positionedComp.gridPos;
			_setting.prevMoveTick = _tick;
		}
		MoveToPos(_currentPath[_moveStep]);
		SetDelay(2);
		return;
	}

	_moveStep++;
}
