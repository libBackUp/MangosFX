#ifndef MANGOS_HUNTERSPELLHANDLER_H
#define MANGOS_HUNTERSPELLHANDLER_H

#include <Policies/Singleton.h>
#include <Common.h>

class MANGOS_DLL_SPEC HunterSpellHandler
{
	public:
		void HandleEffectWeaponDamage(Spell* spell, int32 &spell_bonus, bool &weaponDmgMod);
};

#define sHunterSpellHandler MaNGOS::Singleton<HunterSpellHandler>::Instance()
#endif