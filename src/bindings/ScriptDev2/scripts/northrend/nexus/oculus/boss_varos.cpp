/* Script Data Start
SDName: Boss varos
SDAuthor: LordVanMartin
SD%Complete:
SDComment:
SDCategory:
Script Data End */

/*** SQL START ***
update creature_template set scriptname = '' where entry = '';
*** SQL END ***/
#include "precompiled.h"
#include "def_oculus.h"

enum Spells
{
    SPELL_ENERGIZE_CORES                      = 50785, //Damage 5938 to 6562, effec2 Triggers 54069, effect3 Triggers 56251
    SPELL_ENERGIZE_CORES_TRIGGER_1            = 54069,
    SPELL_ENERGIZE_CORES_TRIGGER_2            = 56251,
    SPELL_ENERGIZE_CORES_2                    = 59372, //Damage 9025 to 9975, effect2 Triggers 54069, effect 56251
    SPELL_CALL_AZURE_RING_CAPTAIN             = 51002, //Effect    Send Event (12229)
    SPELL_CALL_AZURE_RING_CAPTAIN_2           = 51006, //Effect    Send Event (10665)
    SPELL_CALL_AZURE_RING_CAPTAIN_3           = 51007, //Effect    Send Event (18454)
    SPELL_CALL_AZURE_RING_CAPTAIN_4           = 51008, //Effect    Send Event (18455)
    SPELL_CALL_AMPLIFY_MAGIC                  = 51054,
    SPELL_CALL_AMPLIFY_MAGIC_2                = 59371
};
//not in db
enum Yells
{
    SAY_AGGRO                              = -1578022,
    SAY_KILL_1                             = -1578023,
    SAY_KILL_2                             = -1578024,
    SAY_DEATH                              = -1578025,
    SAY_STRIKE_1                           = -1578026,
    SAY_STRIKE_2                           = -1578027,
    SAY_STRIKE_3                           = -1578028,
    SAY_SPAWN                              = -1578029
};

struct MANGOS_DLL_DECL boss_varosAI : public ScriptedAI
{
    boss_varosAI(Creature *c) : ScriptedAI(c)
    {
        pInstance = (ScriptedInstance*)c->GetInstanceData();
		m_bIsHeroic = c->GetMap()->IsRegularDifficulty();
    }
    
	bool m_bIsHeroic;

    void Reset()
    {
        if (pInstance)
            pInstance->SetData(DATA_VAROS_EVENT, NOT_STARTED);
    }
    void EnterCombat(Unit* who)
    {
        DoScriptText(SAY_AGGRO, me);
        
        if (pInstance)
            pInstance->SetData(DATA_VAROS_EVENT, IN_PROGRESS);
    }
    void AttackStart(Unit* who) {}
    void MoveInLineOfSight(Unit* who) {}
    void UpdateAI(const uint32 diff)
    {
        //Return since we have no target
        if (!CanDoSomething())
            return;

        DoMeleeAttackIfReady();
    }
    void JustDied(Unit* killer)
    {
        DoScriptText(SAY_DEATH, me);
        
        if (pInstance)
            pInstance->SetData(DATA_VAROS_EVENT, DONE);
//		GiveEmblemsToGroup(m_bIsHeroic ? HEROISME : 0,1,true);
    }
    void KilledUnit(Unit *victim)
    {
        if (victim == me)
            return;
        switch(urand(0,2))
		{
			case 0:
				DoScriptText(SAY_KILL_1, me);
				break;
			case 1: 
				DoScriptText(SAY_KILL_2, me);
				break;
		}
    }
};

CreatureAI* GetAI_boss_varos(Creature* pCreature)
{
    return new boss_varosAI (pCreature);
}

void AddSC_boss_varos()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_varos";
    newscript->GetAI = &GetAI_boss_varos;
    newscript->RegisterSelf();
}
