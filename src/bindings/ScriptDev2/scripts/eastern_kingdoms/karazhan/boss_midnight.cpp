/* Copyright (C) 2006 - 2009 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/* ScriptData
SDName: Boss_Midnight
SD%Complete: 90
SDComment: Use SPELL_SUMMON_ATTUMEN and SPELL_SUMMON_ATTUMEN_MOUNTED instead of SummonCreature.
SDCategory: Karazhan
EndScriptData */

#include "precompiled.h"

enum
{
    SAY_MIDNIGHT_KILL           = -1532000,
    SAY_APPEAR1                 = -1532001,
    SAY_APPEAR2                 = -1532002,
    SAY_APPEAR3                 = -1532003,
    SAY_MOUNT                   = -1532004,
    SAY_KILL1                   = -1532005,
    SAY_KILL2                   = -1532006,
    SAY_DISARMED                = -1532007,
    SAY_DEATH                   = -1532008,
    SAY_RANDOM1                 = -1532009,
    SAY_RANDOM2                 = -1532010,

    SPELL_SHADOWCLEAVE          = 29832,
    SPELL_INTANGIBLE_PRESENCE   = 29833,
    SPELL_BERSERKER_CHARGE      = 26561,                    //Only when mounted
    SPELL_SUMMON_ATTUMEN        = 29714,
    SPELL_SUMMON_ATTUMEN_MOUNTED= 29799,

    MOUNTED_DISPLAYID           = 16040,                    // should use creature 16152 instead of changing displayid

    //Attumen (TODO: Use the summoning spell instead of creature id. It works , but is not convenient for us)
    SUMMON_ATTUMEN              = 15550
};

struct MANGOS_DLL_DECL boss_midnightAI : public ScriptedAI
{
    boss_midnightAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    uint64 m_uiAttumenGUID;
    uint8  m_uiPhase;
    uint32 m_uiMount_Timer;

    void Reset()
    {
        m_uiPhase = 1;
        m_uiAttumenGUID = 0;
        m_uiMount_Timer = 0;

        me->SetVisibility(VISIBILITY_ON);
    }

    void KilledUnit(Unit* pVictim)
    {
        if (m_uiPhase == 2)
        {
            if (Unit* pUnit = Unit::GetUnit(*me, m_uiAttumenGUID))
                DoScriptText(SAY_MIDNIGHT_KILL, pUnit);
        }
    }

    void Mount(Unit* pAttumen)
    {
        DoScriptText(SAY_MOUNT, pAttumen);
        m_uiPhase = 3;

        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        pAttumen->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

        float fAngle = me->GetAngle(pAttumen);
        float fDistance = me->GetDistance2d(pAttumen);

        float fNewX = me->GetPositionX() + cos(fAngle)*(fDistance/2) ;
        float fNewY = me->GetPositionY() + sin(fAngle)*(fDistance/2) ;
        float fNewZ = 50.0f;

        me->GetMotionMaster()->Clear();
        me->GetMotionMaster()->MovePoint(0, fNewX, fNewY, fNewZ);

        fDistance += 10.0f;
        fNewX = me->GetPositionX() + cos(fAngle)*(fDistance/2);
        fNewY = me->GetPositionY() + sin(fAngle)*(fDistance/2);

        pAttumen->GetMotionMaster()->Clear();
        pAttumen->GetMotionMaster()->MovePoint(0, fNewX, fNewY, fNewZ);

        m_uiMount_Timer = 1000;
    }

    void SetMidnight(Creature *, uint64);                   //Below ..

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;

        switch(m_uiPhase)
        {
            case 1:
                if ((me->GetHealth()*100)/me->GetMaxHealth() < 95)
                {
                    m_uiPhase = 2;

                    if (Creature* pAttumen = me->SummonCreature(SUMMON_ATTUMEN, 0.0f, 0.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 30000))
                    {
                        m_uiAttumenGUID = pAttumen->GetGUID();
                        pAttumen->AI()->AttackStart(me->getVictim());
                        SetMidnight(pAttumen, me->GetGUID());

                        switch(urand(0, 2))
                        {
                            case 0: DoScriptText(SAY_APPEAR1, pAttumen); break;
                            case 1: DoScriptText(SAY_APPEAR2, pAttumen); break;
                            case 2: DoScriptText(SAY_APPEAR3, pAttumen); break;
                        }
                    }
                }
                break;
            case 2:
                if ((me->GetHealth()*100)/me->GetMaxHealth() < 25)
                {
                    if (Unit *pAttumen = Unit::GetUnit(*me, m_uiAttumenGUID))
                        Mount(pAttumen);
                }
                break;
            case 3:
                if (m_uiMount_Timer)
                {
                    if (m_uiMount_Timer <= diff)
                    {
                        m_uiMount_Timer = 0;
                        me->SetVisibility(VISIBILITY_OFF);
                        me->GetMotionMaster()->MoveIdle();

                        if (Unit *pAttumen = Unit::GetUnit(*me, m_uiAttumenGUID))
                        {
                            pAttumen->SetDisplayId(MOUNTED_DISPLAYID);
                            pAttumen->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

                            if (pAttumen->getVictim())
                            {
                                pAttumen->GetMotionMaster()->MoveChase(pAttumen->getVictim());
                                pAttumen->SetUInt64Value(UNIT_FIELD_TARGET, pAttumen->getVictim()->GetGUID());
                            }
                            pAttumen->SetFloatValue(OBJECT_FIELD_SCALE_X,1);
                        }
                    }
                    else
                        m_uiMount_Timer -= diff;
                }
                break;
        }

        if (m_uiPhase != 3)
            DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_midnight(Creature* pCreature)
{
    return new boss_midnightAI(pCreature);
}

struct MANGOS_DLL_DECL boss_attumenAI : public ScriptedAI
{
    boss_attumenAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
        m_uiPhase = 1;

        m_uiCleaveTimer = urand(10000, 16000);
        m_uiCurseTimer = 30000;
        m_uiRandomYellTimer = urand(30000, 60000);          //Occasionally yell
        m_uiChargeTimer = 20000;
        m_uiResetTimer = 0;
    }

    uint64 m_uiMidnightGUID;
    uint8  m_uiPhase;
    uint32 m_uiCleaveTimer;
    uint32 m_uiCurseTimer;
    uint32 m_uiRandomYellTimer;
    uint32 m_uiChargeTimer;                                 //only when mounted
    uint32 m_uiResetTimer;

    void Reset()
    {
        m_uiResetTimer = 2000;
    }

    void KilledUnit(Unit* pVictim)
    {
        DoScriptText(urand(0, 1) ? SAY_KILL1 : SAY_KILL2, me);
    }

    void SpellHit(Unit* pSource, const SpellEntry* pSpell)
    {
        if (pSpell->Mechanic == MECHANIC_DISARM)
            DoScriptText(SAY_DISARMED, me);
    }

    void JustDied(Unit* pVictim)
    {
        DoScriptText(SAY_DEATH, me);

        if (Unit* pMidnight = Unit::GetUnit(*me, m_uiMidnightGUID))
            pMidnight->DealDamage(pMidnight, pMidnight->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
    }

    void UpdateAI(const uint32 diff)
    {
        if (m_uiResetTimer)
        {
            if (m_uiResetTimer <= diff)
            {
                m_uiResetTimer = 0;

                if (Unit *pMidnight = Unit::GetUnit(*me, m_uiMidnightGUID))
                {
                    pMidnight->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    pMidnight->SetVisibility(VISIBILITY_ON);
                }
                m_uiMidnightGUID = 0;

                me->SetVisibility(VISIBILITY_OFF);
                me->DealDamage(me, me->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
            }
            else
                m_uiResetTimer -= diff;
        }

        //Return since we have no target
        if (!CanDoSomething())
            return;

        if (me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE))
            return;

        if (m_uiCleaveTimer < diff)
        {
            DoCastVictim( SPELL_SHADOWCLEAVE);
            m_uiCleaveTimer = urand(10000, 16000);
        }
        else
            m_uiCleaveTimer -= diff;

        if (m_uiCurseTimer < diff)
        {
            DoCastVictim( SPELL_INTANGIBLE_PRESENCE);
            m_uiCurseTimer = 30000;
        }
        else
            m_uiCurseTimer -= diff;

        if (m_uiRandomYellTimer < diff)
        {
            DoScriptText(urand(0, 1) ? SAY_RANDOM1 : SAY_RANDOM2, me);
            m_uiRandomYellTimer = urand(30000, 60000);
        }
        else
            m_uiRandomYellTimer -= diff;

        if (me->GetDisplayId() == MOUNTED_DISPLAYID)
        {
            if (m_uiChargeTimer < diff)
            {
                Unit *target;
                std::list<HostileReference *> t_list = me->getThreatManager().getThreatList();
                std::vector<Unit *> target_list;
                for(std::list<HostileReference *>::iterator itr = t_list.begin(); itr!= t_list.end(); ++itr)
                {
                    target = Unit::GetUnit(*me, (*itr)->getUnitGuid());
                    if (target && !target->IsWithinDist(me, ATTACK_DISTANCE, false))
                        target_list.push_back(target);
                    target = NULL;
                }
                if (target_list.size())
                    target = *(target_list.begin()+rand()%target_list.size());

                DoCast(target, SPELL_BERSERKER_CHARGE);
                m_uiChargeTimer = 20000;
            }
            else
                m_uiChargeTimer -= diff;
        }
        else
        {
            if ((me->GetHealth()*100)/me->GetMaxHealth() < 25)
            {
                Creature *pMidnight = (Creature*)Unit::GetUnit(*me, m_uiMidnightGUID);

                if (pMidnight && pMidnight->GetTypeId() == TYPEID_UNIT)
                {
                    ((boss_midnightAI*)(pMidnight->AI()))->Mount(me);
                    me->SetHealth(pMidnight->GetHealth());
                    DoResetThreat();
                }
            }
        }

        DoMeleeAttackIfReady();
    }
};

void boss_midnightAI::SetMidnight(Creature* pAttumen, uint64 uiValue)
{
    ((boss_attumenAI*)pAttumen->AI())->m_uiMidnightGUID = uiValue;
}

CreatureAI* GetAI_boss_attumen(Creature* pCreature)
{
    return new boss_attumenAI(pCreature);
}

void AddSC_boss_attumen()
{
    Script* newscript;

    newscript = new Script;
    newscript->Name = "boss_attumen";
    newscript->GetAI = &GetAI_boss_attumen;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "boss_midnight";
    newscript->GetAI = &GetAI_boss_midnight;
    newscript->RegisterSelf();
}
