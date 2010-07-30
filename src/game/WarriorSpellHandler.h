#ifndef MANGOS_WARRIORSPELLHANDLER_H
#define MANGOS_WARRIORSPELLHANDLER_H

#include <Policies/Singleton.h>
#include <Common.h>

class MANGOS_DLL_SPEC WarriorSpellHandler
{
	public:
		void HandleEffectWeaponDamage(Spell* spell, int32 &spell_bonus, bool &weaponDmgMod, float &totalDmgPctMod);
		//void HandleDummyAuraProc(Unit* u, Spell* dummy, uint32 &trig_sp_id);
};

#define sWarriorSpellHandler MaNGOS::Singleton<WarriorSpellHandler>::Instance()
#endif