/* Copyright (C) 2006 - 2010 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 * This program is free software licensed under GPL version 2
 * Please see the included DOCS/LICENSE.TXT for more information */

#ifndef DEF_ICECROWN_CITADEL_H
#define DEF_ICECROWN_CITADEL_H

enum
{
    MAX_ENCOUNTER               = 12,

    TYPE_MARROWGAR              = 0,
    TYPE_DEATHWHISPER           = 1,
	TYPE_BATTLE_OF_CANNONS		= 2,
    TYPE_SAURFANG               = 3,
	TYPE_FESTERGUT				= 4,
	TYPE_ROTFACE				= 5,
	TYPE_PUTRICIDE				= 6,
	TYPE_PRINCE_COUNCIL			= 7,
	TYPE_LANATHEL				= 8,
	TYPE_DREAMWALKER			= 9,
	TYPE_SINDRAGOSA				= 10,
	TYPE_LICHKING				= 11,

    NPC_MARROWGAR               = 36612,
    NPC_DEATHWHISPER            = 36855,
    NPC_SAURFANG                = 37813,
	NPC_FESTERGUT				= 36626,
	NPC_ROTFACE					= 36627,
	NPC_PUTRICIDE				= 36678,
	NPC_PRINCE_KELESETH			= 37972,
	NPC_PRINCE_VALANAR			= 37970,
	NPC_PRINCE_TALDARAM			= 37973,
	NPC_LANATHEL				= 37955,
	NPC_DREAMWALKER				= 36789,
	NPC_SINDRAGOSA				= 36853,
	NPC_LICHKING				= 36597,

	DATA_PRINCE_VALANAR			= 20,
	DATA_PRINCE_KELESETH		= 21,
	DATA_PRINCE_TALDARAM		= 22,

    GO_MARROWGAR_ICE_1          = 201910,
    GO_MARROWGAR_ICE_2          = 201911,
    GO_DEATHWHISPER_GATE        = 201563,
    GO_DEATHWHISPER_ELEVATOR    = 202220,
    GO_SAURFANG_DOOR            = 201825,
	GO_MARROWGAR_DOOR			= 201857,
	GO_FESTERGUT_DOOR			= 201371,
	GO_ROTFACE_DOOR				= 201370,
	GO_PUTRICIDE_DOOR			= 201372,
	GO_BLOODWING_DOOR			= 201920,
	GO_PRINCECOUNCIL_DOOR		= 201376,
	GO_LANATHEL_DOOR_1			= 201378,
	GO_LANATHEL_DOOR_2			= 201377,
	GO_FROSTWING_DOOR			= 201919,
	GO_DREAMWALKER_DOOR			= 201375,
	GO_DREAMWALKER_EXIT_DOOR	= 201374,
	GO_SINDRAGOSA_DOOR			= 201373,

	GO_PLAGUE_SIGIL				= 202182,
	GO_BLOOD_SIGIL				= 202183,
	GO_FROST_SIGIL				= 202181,

	GO_PLAGUE_ORANGE_TUBE		= 201617,
	GO_PLAGUE_ORANGE_DOOR		= 201613,

	GO_SAURFANG_CACHE_10		= 202239,
    GO_SAURFANG_CACHE_25		= 202240,
    GO_SAURFANG_CACHE_10_H		= 202238,
    GO_SAURFANG_CACHE_25_H		= 202241,
};

#endif
