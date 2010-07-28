#ifndef MANGOS_PRIESTSPELLHANDLER_H
#define MANGOS_PRIESTSPELLHANDLER_H

#include <Policies/Singleton.h>
#include <Common.h>

class MANGOS_DLL_SPEC PriestSpellHandler
{
	public:
		void HandleEffectWeaponDamage(Spell* spell, int32 &spell_bonus, bool &weaponDmgMod, float &totalDmgPctMod);
		//void HandleDummyAuraProc(Unit* u, Spell* dummy, uint32 &trig_sp_id);
};

#define sPriestSpellHandler MaNGOS::Singleton<PriestSpellHandler>::Instance()
#endif