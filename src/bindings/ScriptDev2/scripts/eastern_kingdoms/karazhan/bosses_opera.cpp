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
SDName: Bosses_Opera
SD%Complete: 90
SDComment: Oz, Hood, and RAJ event implemented. RAJ event requires more testing.
SDCategory: Karazhan
EndScriptData */

#include "precompiled.h"
#include "karazhan.h"

/***********************************/
/*** OPERA WIZARD OF OZ EVENT *****/
/*********************************/

#define SAY_DOROTHEE_DEATH          -1532025
#define SAY_DOROTHEE_SUMMON         -1532026
#define SAY_DOROTHEE_TITO_DEATH     -1532027
#define SAY_DOROTHEE_AGGRO          -1532028

#define SAY_ROAR_AGGRO              -1532029
#define SAY_ROAR_DEATH              -1532030
#define SAY_ROAR_SLAY               -1532031

#define SAY_STRAWMAN_AGGRO          -1532032
#define SAY_STRAWMAN_DEATH          -1532033
#define SAY_STRAWMAN_SLAY           -1532034

#define SAY_TINHEAD_AGGRO           -1532035
#define SAY_TINHEAD_DEATH           -1532036
#define SAY_TINHEAD_SLAY            -1532037
#define EMOTE_RUST                  -1532038

#define SAY_CRONE_AGGRO             -1532039
#define SAY_CRONE_AGGRO2            -1532040
#define SAY_CRONE_DEATH             -1532041
#define SAY_CRONE_SLAY              -1532042

/**** Spells ****/
// Dorothee
#define SPELL_WATERBOLT         31012
#define SPELL_SCREAM            31013
#define SPELL_SUMMONTITO        31014

// Tito
#define SPELL_YIPPING           31015

// Strawman
#define SPELL_BRAIN_BASH        31046
#define SPELL_BRAIN_WIPE        31069
#define SPELL_BURNING_STRAW     31075

// Tinhead
#define SPELL_CLEAVE            31043
#define SPELL_RUST              31086

// Roar
#define SPELL_MANGLE            31041
#define SPELL_SHRED             31042
#define SPELL_FRIGHTENED_SCREAM 31013

// Crone
#define SPELL_CHAIN_LIGHTNING   32337

// Cyclone
#define SPELL_KNOCKBACK         32334
#define SPELL_CYCLONE_VISUAL    32332

/** Creature Entries **/
#define CREATURE_TITO           17548
#define CREATURE_CYCLONE        18412
#define CREATURE_CRONE          18168

void SummonCroneIfReady(ScriptedInstance* pInstance, Creature* pCreature)
{
    pInstance->SetData(DATA_OPERA_OZ_DEATHCOUNT, SPECIAL);  // Increment DeathCount

    if (pInstance->GetData(DATA_OPERA_OZ_DEATHCOUNT) == 4)
    {
        if (Creature* pCrone = pCreature->SummonCreature(CREATURE_CRONE, -10891.96, -1755.95, pCreature->GetPositionZ(), 4.64, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, HOUR*2*IN_MILLISECONDS))
        {
            if (pCreature->getVictim())
                pCrone->AI()->AttackStart(pCreature->getVictim());
        }
    }
};

struct MANGOS_DLL_DECL boss_dorotheeAI : public LibDevFSAI
{
    boss_dorotheeAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
        AddEventOnTank(SPELL_SCREAM,15000,30000);
    }

    uint32 AggroTimer;

    uint32 WaterBoltTimer;
    uint32 SummonTitoTimer;

    bool SummonedTito;
    bool TitoDied;

    void Reset()
    {
		ResetTimers();
        AggroTimer = 500;

        WaterBoltTimer = 5000;
        SummonTitoTimer = 47500;

        SummonedTito = false;
        TitoDied = false;
    }

    void Aggro(Unit* who)
    {
        DoScriptText(SAY_DOROTHEE_AGGRO, me);
    }

    void JustReachedHome()
    {
        me->ForcedDespawn();
    }

    void SummonTito();                                      // See below

    void JustDied(Unit* killer)
    {
        DoScriptText(SAY_DOROTHEE_DEATH, me);

        if (pInstance)
            SummonCroneIfReady(pInstance, me);
    }

    void AttackStart(Unit* who)
    {
        if (me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
            return;

        ScriptedAI::AttackStart(who);
    }

    void MoveInLineOfSight(Unit* who)
    {
        if (me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
            return;

        ScriptedAI::MoveInLineOfSight(who);
    }

    void UpdateAI(const uint32 diff)
    {
        if (AggroTimer)
        {
            if (AggroTimer <= diff)
            {
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                AggroTimer = 0;
            }else AggroTimer -= diff;
        }

        if (!CanDoSomething())
            return;

        if (WaterBoltTimer < diff)
        {
            DoCast(SelectUnit(SELECT_TARGET_RANDOM, 0), SPELL_WATERBOLT);
            WaterBoltTimer = TitoDied ? 1500 : 5000;
        }else WaterBoltTimer -= diff;

        if (!SummonedTito)
        {
            if (SummonTitoTimer < diff)
                SummonTito();
            else SummonTitoTimer -= diff;
        }
        
        UpdateEvent(diff);

        DoMeleeAttackIfReady();
    }
};

struct MANGOS_DLL_DECL mob_titoAI : public LibDevFSAI
{
    mob_titoAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {
        InitIA();
        AddEventOnTank(SPELL_YIPPING,10000,10000);
    }

    uint64 DorotheeGUID;

    void Reset()
    {
		ResetTimers();
        DorotheeGUID = 0;
     }

    void JustDied(Unit* killer)
    {
        if (DorotheeGUID)
        {
            Creature* Dorothee = ((Creature*)Unit::GetUnit((*me), DorotheeGUID));
            if (Dorothee && Dorothee->isAlive())
            {
                ((boss_dorotheeAI*)Dorothee->AI())->TitoDied = true;
                DoScriptText(SAY_DOROTHEE_TITO_DEATH, Dorothee);
            }
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;

        UpdateEvent(diff);

        DoMeleeAttackIfReady();
    }
};

void boss_dorotheeAI::SummonTito()
{
    if (Creature* pTito = me->SummonCreature(CREATURE_TITO, 0.0f, 0.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000))
    {
        DoScriptText(SAY_DOROTHEE_SUMMON, me);

        ((mob_titoAI*)pTito->AI())->DorotheeGUID = me->GetGUID();
        pTito->AI()->AttackStart(me->getVictim());

        SummonedTito = true;
        TitoDied = false;
    }
}

struct MANGOS_DLL_DECL boss_strawmanAI : public LibDevFSAI
{
    boss_strawmanAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
        AddEventOnTank(SPELL_BRAIN_BASH,5000,15000);
        AddEvent(SPELL_BRAIN_WIPE,7000,20000);
    }

    uint32 AggroTimer;

    void Reset()
    {
		ResetTimers();
        AggroTimer = 13000;
    }

    void AttackStart(Unit* who)
    {
        if (me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
            return;

        ScriptedAI::AttackStart(who);
    }

    void MoveInLineOfSight(Unit* who)
    {
        if (me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
            return;

        ScriptedAI::MoveInLineOfSight(who);
    }

    void Aggro(Unit* who)
    {
        DoScriptText(SAY_STRAWMAN_AGGRO, me);
    }

    void JustReachedHome()
    {
        me->ForcedDespawn();
    }

    void SpellHit(Unit* caster, const SpellEntry *Spell)
    {
        if ((Spell->SchoolMask == SPELL_SCHOOL_MASK_FIRE) && !urand(0, 1))
        {
            /*
                if (not direct damage(aoe,dot))
                    return;
            */

            DoCastMe( SPELL_BURNING_STRAW, true);
        }
    }

    void JustDied(Unit* killer)
    {
        DoScriptText(SAY_STRAWMAN_DEATH, me);

        if (pInstance)
            SummonCroneIfReady(pInstance, me);
    }

    void KilledUnit(Unit* victim)
    {
        DoScriptText(SAY_STRAWMAN_SLAY, me);
    }

    void UpdateAI(const uint32 diff)
    {
        if (AggroTimer)
        {
            if (AggroTimer <= diff)
            {
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                AggroTimer = 0;
            }else AggroTimer -= diff;
        }

        if (!CanDoSomething())
            return;
        
        UpdateEvent(diff);

        DoMeleeAttackIfReady();
    }
};

struct MANGOS_DLL_DECL boss_tinheadAI : public LibDevFSAI
{
    boss_tinheadAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
        AddEventOnTank(SPELL_CLEAVE,5000,5000);
    }

    uint32 AggroTimer;
    uint32 RustTimer;

    uint8 RustCount;

    void Reset()
    {
        AggroTimer = 15000;
        RustTimer   = 30000;

        RustCount   = 0;
    }

    void Aggro(Unit* who)
    {
        DoScriptText(SAY_TINHEAD_AGGRO, me);
    }

    void JustReachedHome()
    {
        me->ForcedDespawn();
    }

    void AttackStart(Unit* who)
    {
        if (me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
            return;

        ScriptedAI::AttackStart(who);
    }

    void MoveInLineOfSight(Unit* who)
    {
        if (me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
            return;

        ScriptedAI::MoveInLineOfSight(who);
    }

    void JustDied(Unit* killer)
    {
        DoScriptText(SAY_TINHEAD_DEATH, me);

        if (pInstance)
            SummonCroneIfReady(pInstance, me);
    }

    void KilledUnit(Unit* victim)
    {
        DoScriptText(SAY_TINHEAD_SLAY, me);
    }

    void UpdateAI(const uint32 diff)
    {
        if (AggroTimer)
        {
            if (AggroTimer <= diff)
            {
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                AggroTimer = 0;
            }else AggroTimer -= diff;
        }

        if (!CanDoSomething())
            return;

        if (RustCount < 8)
        {
            if (RustTimer < diff)
            {
                ++RustCount;
                DoScriptText(EMOTE_RUST, me);
                DoCastMe( SPELL_RUST);
                RustTimer = 6000;
            }else RustTimer -= diff;
        }
        
        UpdateEvent(diff);

        DoMeleeAttackIfReady();
    }
};

struct MANGOS_DLL_DECL boss_roarAI : public ScriptedAI
{
    boss_roarAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint32 AggroTimer;
    uint32 MangleTimer;
    uint32 ShredTimer;
    uint32 ScreamTimer;

    void Reset()
    {
        AggroTimer = 20000;
        MangleTimer = 5000;
        ShredTimer  = 10000;
        ScreamTimer = 15000;
    }

    void MoveInLineOfSight(Unit* who)
    {
        if (me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
            return;

        ScriptedAI::MoveInLineOfSight(who);
    }

    void AttackStart(Unit* who)
    {
        if (me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
            return;

        ScriptedAI::AttackStart(who);
    }

    void Aggro(Unit* who)
    {
        DoScriptText(SAY_ROAR_AGGRO, me);
    }

    void JustReachedHome()
    {
        me->ForcedDespawn();
    }

    void JustDied(Unit* killer)
    {
        DoScriptText(SAY_ROAR_DEATH, me);

        if (m_pInstance)
            SummonCroneIfReady(m_pInstance, me);
    }

    void KilledUnit(Unit* victim)
    {
        DoScriptText(SAY_ROAR_SLAY, me);
    }

    void UpdateAI(const uint32 diff)
    {
        if (AggroTimer)
        {
            if (AggroTimer <= diff)
            {
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                AggroTimer = 0;
            }else AggroTimer -= diff;
        }

        if (!CanDoSomething())
            return;

        if (MangleTimer < diff)
        {
            DoCastVictim( SPELL_MANGLE);
            MangleTimer = urand(5000, 8000);
        }else MangleTimer -= diff;

        if (ShredTimer < diff)
        {
            DoCastVictim( SPELL_SHRED);
            ShredTimer = urand(10000, 15000);
        }else ShredTimer -= diff;

        if (ScreamTimer < diff)
        {
            DoCastVictim( SPELL_FRIGHTENED_SCREAM);
            ScreamTimer = urand(20000, 30000);
        }else ScreamTimer -= diff;

        DoMeleeAttackIfReady();
    }
};

struct MANGOS_DLL_DECL boss_croneAI : public ScriptedAI
{
    boss_croneAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint32 CycloneTimer;
    uint32 ChainLightningTimer;

    void Reset()
    {
        CycloneTimer = 30000;
        ChainLightningTimer = 10000;
    }

    void JustReachedHome()
    {
        me->ForcedDespawn();
    }

    void Aggro(Unit* who)
    {
        DoScriptText(urand(0, 1) ? SAY_CRONE_AGGRO : SAY_CRONE_AGGRO2, me);

        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
    }

    void JustDied(Unit* killer)
    {
        DoScriptText(SAY_CRONE_DEATH, me);

        if (m_pInstance)
        {
            m_pInstance->SetData(TYPE_OPERA, DONE);

            if (GameObject* pLDoor = m_pInstance->instance->GetGameObject(m_pInstance->GetData64(DATA_GO_STAGEDOORLEFT)))
                pLDoor->SetGoState(GO_STATE_ACTIVE);
            if (GameObject* pRDoor = m_pInstance->instance->GetGameObject(m_pInstance->GetData64(DATA_GO_STAGEDOORRIGHT)))
                pRDoor->SetGoState(GO_STATE_ACTIVE);
            if (GameObject* pSideEntrance = m_pInstance->instance->GetGameObject(m_pInstance->GetData64(DATA_GO_SIDE_ENTRANCE_DOOR)))
                pSideEntrance->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_LOCKED);
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;

        if (me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

        if (CycloneTimer < diff)
        {
            Creature* Cyclone = DoSpawnCreature(CREATURE_CYCLONE, rand()%10, rand()%10, 0, 0, TEMPSUMMON_TIMED_DESPAWN, 15000);
            if (Cyclone)
                Cyclone->CastSpell(Cyclone, SPELL_CYCLONE_VISUAL, true);
            CycloneTimer = 30000;
        }else CycloneTimer -= diff;

        if (ChainLightningTimer < diff)
        {
            DoCastVictim( SPELL_CHAIN_LIGHTNING);
            ChainLightningTimer = 15000;
        }else ChainLightningTimer -= diff;

        DoMeleeAttackIfReady();
    }
};

struct MANGOS_DLL_DECL mob_cycloneAI : public ScriptedAI
{
    mob_cycloneAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    uint32 MoveTimer;

    void Reset()
    {
        MoveTimer = 1000;
    }

    void MoveInLineOfSight(Unit* who) { }

    void UpdateAI(const uint32 diff)
    {
        if (!me->HasAura(SPELL_KNOCKBACK, 0))
            DoCastMe( SPELL_KNOCKBACK, true);

        if (MoveTimer < diff)
        {
            float x,y,z;
            me->GetPosition(x,y,z);
            float PosX, PosY, PosZ;
            me->GetRandomPoint(x,y,z,10, PosX, PosY, PosZ);
            me->GetMotionMaster()->MovePoint(0, PosX, PosY, PosZ);
            MoveTimer = urand(5000, 8000);
        }else MoveTimer -= diff;
    }
};

CreatureAI* GetAI_boss_dorothee(Creature* pCreature)
{
    return new boss_dorotheeAI(pCreature);
}

CreatureAI* GetAI_boss_strawman(Creature* pCreature)
{
    return new boss_strawmanAI(pCreature);
}

CreatureAI* GetAI_boss_tinhead(Creature* pCreature)
{
    return new boss_tinheadAI(pCreature);
}

CreatureAI* GetAI_boss_roar(Creature* pCreature)
{
    return new boss_roarAI(pCreature);
}

CreatureAI* GetAI_boss_crone(Creature* pCreature)
{
    return new boss_croneAI(pCreature);
}

CreatureAI* GetAI_mob_tito(Creature* pCreature)
{
    return new mob_titoAI(pCreature);
}

CreatureAI* GetAI_mob_cyclone(Creature* pCreature)
{
    return new mob_cycloneAI(pCreature);
}

/**************************************/
/**** Opera Red Riding Hood Event ****/
/************************************/

/**** Yells for the Wolf ****/
#define SAY_WOLF_AGGRO                  -1532043
#define SAY_WOLF_SLAY                   -1532044
#define SAY_WOLF_HOOD                   -1532045
#define SOUND_WOLF_DEATH                9275                //Only sound on death, no text.

/**** Spells For The Wolf ****/
#define SPELL_LITTLE_RED_RIDING_HOOD    30768
#define SPELL_TERRIFYING_HOWL           30752
#define SPELL_WIDE_SWIPE                30761

#define GOSSIP_GRANDMA                  "What phat lewtz you have grandmother?"

/**** The Wolf's Entry ****/
#define CREATURE_BIG_BAD_WOLF           17521

bool GossipHello_npc_grandmother(Player* pPlayer, Creature* pCreature)
{
    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_GRANDMA, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
    pPlayer->SEND_GOSSIP_MENU(8990, pCreature->GetGUID());

    return true;
}

bool GossipSelect_npc_grandmother(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF)
    {
        if (Creature* pBigBadWolf = pCreature->SummonCreature(CREATURE_BIG_BAD_WOLF, 0.0f, 0.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, HOUR*2*IN_MILLISECONDS))
            pBigBadWolf->AI()->AttackStart(pPlayer);

        pCreature->ForcedDespawn();
    }

    return true;
}

struct MANGOS_DLL_DECL boss_bigbadwolfAI : public ScriptedAI
{
    boss_bigbadwolfAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint32 ChaseTimer;
    uint32 FearTimer;
    uint32 SwipeTimer;

    uint64 HoodGUID;
    float TempThreat;

    bool IsChasing;

    void Reset()
    {
        ChaseTimer = 30000;
        FearTimer = urand(25000, 35000);
        SwipeTimer = 5000;

        HoodGUID = 0;
        TempThreat = 0;

        IsChasing = false;
    }

    void Aggro(Unit* who)
    {
        DoScriptText(SAY_WOLF_AGGRO, me);
    }

    void JustReachedHome()
    {
        me->ForcedDespawn();
    }

    void JustDied(Unit* killer)
    {
        DoPlaySoundToSet(me, SOUND_WOLF_DEATH);

        if (m_pInstance)
        {
            m_pInstance->SetData(TYPE_OPERA, DONE);

            if (GameObject* pLDoor = m_pInstance->instance->GetGameObject(m_pInstance->GetData64(DATA_GO_STAGEDOORLEFT)))
                pLDoor->SetGoState(GO_STATE_ACTIVE);
            if (GameObject* pRDoor = m_pInstance->instance->GetGameObject(m_pInstance->GetData64(DATA_GO_STAGEDOORRIGHT)))
                pRDoor->SetGoState(GO_STATE_ACTIVE);
            if (GameObject* pSideEntrance = m_pInstance->instance->GetGameObject(m_pInstance->GetData64(DATA_GO_SIDE_ENTRANCE_DOOR)))
                pSideEntrance->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_LOCKED);
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;

        DoMeleeAttackIfReady();

        if (ChaseTimer < diff)
        {
            if (!IsChasing)
            {
                Unit* target = SelectUnit(SELECT_TARGET_RANDOM, 0);
                if (target && target->GetTypeId() == TYPEID_PLAYER)
                {
                    DoScriptText(SAY_WOLF_HOOD, me);
                    DoCast(target, SPELL_LITTLE_RED_RIDING_HOOD, true);

                    TempThreat = me->getThreatManager().getThreat(target);
                    if (TempThreat)
                        me->getThreatManager().modifyThreatPercent(target, -100);

                    HoodGUID = target->GetGUID();
                    me->AddThreat(target, 1000000.0f);
                    ChaseTimer = 20000;
                    IsChasing = true;
                }
            }
            else
            {
                IsChasing = false;

                if (Unit* target = Unit::GetUnit((*me), HoodGUID))
                {
                    HoodGUID = 0;
                    if (me->getThreatManager().getThreat(target))
                        me->getThreatManager().modifyThreatPercent(target, -100);

                    me->AddThreat(target, TempThreat);
                    TempThreat = 0;
                }

                ChaseTimer = 40000;
            }
        }else ChaseTimer -= diff;

        if (IsChasing)
            return;

        if (FearTimer < diff)
        {
            DoCastVictim( SPELL_TERRIFYING_HOWL);
            FearTimer = urand(25000, 35000);
        }else FearTimer -= diff;

        if (SwipeTimer < diff)
        {
            DoCastVictim( SPELL_WIDE_SWIPE);
            SwipeTimer = urand(25000, 30000);
        }else SwipeTimer -= diff;
    }
};

CreatureAI* GetAI_boss_bigbadwolf(Creature* pCreature)
{
    return new boss_bigbadwolfAI(pCreature);
}

/**********************************************/
/******** Opera Romeo and Juliet Event *******/
/********************************************/

/**** Speech *****/
#define SAY_JULIANNE_AGGRO              -1532046
#define SAY_JULIANNE_ENTER              -1532047
#define SAY_JULIANNE_DEATH01            -1532048
#define SAY_JULIANNE_DEATH02            -1532049
#define SAY_JULIANNE_RESURRECT          -1532050
#define SAY_JULIANNE_SLAY               -1532051

#define SAY_ROMULO_AGGRO                -1532052
#define SAY_ROMULO_DEATH                -1532053
#define SAY_ROMULO_ENTER                -1532054
#define SAY_ROMULO_RESURRECT            -1532055
#define SAY_ROMULO_SLAY                 -1532056

/***** Spells For Julianne *****/
#define SPELL_BLINDING_PASSION          30890
#define SPELL_DEVOTION                  30887
#define SPELL_ETERNAL_AFFECTION         30878
#define SPELL_POWERFUL_ATTRACTION       30889
#define SPELL_DRINK_POISON              30907

/***** Spells For Romulo ****/
#define SPELL_BACKWARD_LUNGE            30815
#define SPELL_DARING                    30841
#define SPELL_DEADLY_SWATHE             30817
#define SPELL_POISON_THRUST             30822

/**** Other Misc. Spells ****/
#define SPELL_UNDYING_LOVE              30951
#define SPELL_RES_VISUAL                24171

/*** Misc. Information ****/
#define CREATURE_ROMULO                 17533
#define ROMULO_X                        -10900
#define ROMULO_Y                        -1758

enum RAJPhase
{
    PHASE_JULIANNE      = 0,
    PHASE_ROMULO        = 1,
    PHASE_BOTH          = 2,
};

void PretendToDie(Creature* pCreature)
{
    pCreature->InterruptNonMeleeSpells(true);
    pCreature->RemoveAllAuras();
    pCreature->SetHealth(0);
    pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
    pCreature->GetMotionMaster()->MovementExpired(false);
    pCreature->GetMotionMaster()->MoveIdle();
    pCreature->SetStandState(UNIT_STAND_STATE_DEAD);
};

void Resurrect(Creature* target)
{
    target->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
    target->SetHealth(target->GetMaxHealth());
    target->SetStandState(UNIT_STAND_STATE_STAND);
    target->CastSpell(target, SPELL_RES_VISUAL, true);
    if (target->getVictim())
    {
        target->GetMotionMaster()->MoveChase(target->getVictim());
        target->AI()->AttackStart(target->getVictim());
    }
        else
            target->GetMotionMaster()->Initialize();
};

struct MANGOS_DLL_DECL boss_julianneAI : public ScriptedAI
{
    boss_julianneAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        EntryYellTimer = 1000;
        AggroYellTimer = 10000;
        IsFakingDeath = false;
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint32 EntryYellTimer;
    uint32 AggroYellTimer;

    uint64 RomuloGUID;
    uint32 Phase;

    uint32 BlindingPassionTimer;
    uint32 DevotionTimer;
    uint32 EternalAffectionTimer;
    uint32 PowerfulAttractionTimer;
    uint32 SummonRomuloTimer;
    uint32 ResurrectTimer;
    uint32 DrinkPoisonTimer;
    uint32 ResurrectSelfTimer;

    bool IsFakingDeath;
    bool SummonedRomulo;
    bool RomuloDead;

    void Reset()
    {
        RomuloGUID = 0;
        Phase = PHASE_JULIANNE;

        BlindingPassionTimer = 30000;
        DevotionTimer = 15000;
        EternalAffectionTimer = 25000;
        PowerfulAttractionTimer = 5000;
        SummonRomuloTimer = 10000;
        ResurrectTimer = 10000;
        DrinkPoisonTimer = 0;
        ResurrectSelfTimer = 0;

        if (IsFakingDeath)
        {
            Resurrect(me);
            IsFakingDeath = false;
        }

        SummonedRomulo = false;
        RomuloDead = false;
    }

    void AttackStart(Unit* who)
    {
        if (me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
            return;

        ScriptedAI::AttackStart(who);
    }

    void MoveInLineOfSight(Unit* who)
    {
        if (me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
            return;

        ScriptedAI::MoveInLineOfSight(who);
    }

    void JustReachedHome()
    {
        me->ForcedDespawn();
    }

    void SpellHit(Unit* caster, const SpellEntry *Spell)
    {
        if (Spell->Id == SPELL_DRINK_POISON)
        {
            DoScriptText(SAY_JULIANNE_DEATH01, me);
            DrinkPoisonTimer = 2500;
        }
    }

    void DamageTaken(Unit* done_by, uint32 &damage);

    void JustDied(Unit* killer)
    {
        DoScriptText(SAY_JULIANNE_DEATH02, me);

        if (m_pInstance)
        {
            m_pInstance->SetData(TYPE_OPERA, DONE);

            if (GameObject* pLDoor = m_pInstance->instance->GetGameObject(m_pInstance->GetData64(DATA_GO_STAGEDOORLEFT)))
                pLDoor->SetGoState(GO_STATE_ACTIVE);
            if (GameObject* pRDoor = m_pInstance->instance->GetGameObject(m_pInstance->GetData64(DATA_GO_STAGEDOORRIGHT)))
                pRDoor->SetGoState(GO_STATE_ACTIVE);
            if (GameObject* pSideEntrance = m_pInstance->instance->GetGameObject(m_pInstance->GetData64(DATA_GO_SIDE_ENTRANCE_DOOR)))
                pSideEntrance->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_LOCKED);
        }
    }

    void KilledUnit(Unit* victim)
    {
        DoScriptText(SAY_JULIANNE_SLAY, me);
    }

    void UpdateAI(const uint32 diff);
};

struct MANGOS_DLL_DECL boss_romuloAI : public ScriptedAI
{
    boss_romuloAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
        EntryYellTimer = 8000;
        AggroYellTimer = 15000;
    }

    ScriptedInstance* m_pInstance;

    uint64 JulianneGUID;
    uint32 Phase;

    uint32 EntryYellTimer;
    uint32 AggroYellTimer;
    uint32 BackwardLungeTimer;
    uint32 DaringTimer;
    uint32 DeadlySwatheTimer;
    uint32 PoisonThrustTimer;
    uint32 ResurrectTimer;

    bool IsFakingDeath;
    bool JulianneDead;

    void Reset()
    {
        JulianneGUID = 0;
        Phase = PHASE_ROMULO;

        BackwardLungeTimer = 15000;
        DaringTimer = 20000;
        DeadlySwatheTimer = 25000;
        PoisonThrustTimer = 10000;
        ResurrectTimer = 10000;

        IsFakingDeath = false;
        JulianneDead = false;
    }

    void JustReachedHome()
    {
        me->ForcedDespawn();
    }

    void DamageTaken(Unit* done_by, uint32 &damage);

    void Aggro(Unit* who)
    {
        DoScriptText(SAY_ROMULO_AGGRO, me);

        if (JulianneGUID)
        {
            Creature* Julianne = ((Creature*)Unit::GetUnit((*me), JulianneGUID));
            if (Julianne && Julianne->getVictim())
            {
                me->AddThreat(Julianne->getVictim(), 1.0f);
            }
        }
    }

    void MoveInLineOfSight(Unit* who)
    {
        if (me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
            return;

        ScriptedAI::MoveInLineOfSight(who);
    }

    void JustDied(Unit* killer)
    {
        DoScriptText(SAY_ROMULO_DEATH, me);

        if (m_pInstance)
        {
            m_pInstance->SetData(TYPE_OPERA, DONE);

            if (GameObject* pLDoor = m_pInstance->instance->GetGameObject(m_pInstance->GetData64(DATA_GO_STAGEDOORLEFT)))
                pLDoor->SetGoState(GO_STATE_ACTIVE);
            if (GameObject* pRDoor = m_pInstance->instance->GetGameObject(m_pInstance->GetData64(DATA_GO_STAGEDOORRIGHT)))
                pRDoor->SetGoState(GO_STATE_ACTIVE);
            if (GameObject* pSideEntrance = m_pInstance->instance->GetGameObject(m_pInstance->GetData64(DATA_GO_SIDE_ENTRANCE_DOOR)))
                pSideEntrance->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_LOCKED);
        }
    }

    void KilledUnit(Unit* victim)
    {
        DoScriptText(SAY_ROMULO_SLAY, me);
    }

    void UpdateAI(const uint32 diff);
};

void boss_julianneAI::DamageTaken(Unit* done_by, uint32 &damage)
{
    if (damage < me->GetHealth())
        return;

    //anything below only used if incoming damage will kill

    if (Phase == PHASE_JULIANNE)
    {
        damage = 0;

        //this means already drinking, so return
        if (IsFakingDeath)
            return;

        me->InterruptNonMeleeSpells(true);
        DoCastMe( SPELL_DRINK_POISON);

        IsFakingDeath = true;
        return;
    }

    if (Phase == PHASE_ROMULO)
    {
        error_log("SD2: boss_julianneAI: cannot take damage in PHASE_ROMULO, why was i here?");
        damage = 0;
        return;
    }

    if (Phase == PHASE_BOTH)
    {
        //if this is true then we have to kill romulo too
        if (RomuloDead)
        {
            if (Creature* Romulo = ((Creature*)Unit::GetUnit((*me), RomuloGUID)))
            {
                Romulo->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                Romulo->GetMotionMaster()->Clear();
                Romulo->setDeathState(JUST_DIED);
                Romulo->CombatStop(true);
                Romulo->DeleteThreatList();
                Romulo->SetUInt32Value(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);
            }
            return;
        }

        //if not already returned, then romulo is alive and we can pretend die
        if (Creature* Romulo = ((Creature*)Unit::GetUnit((*me), RomuloGUID)))
        {
            PretendToDie(me);
            IsFakingDeath = true;
            ((boss_romuloAI*)Romulo->AI())->ResurrectTimer = 10000;
            ((boss_romuloAI*)Romulo->AI())->JulianneDead = true;
            damage = 0;
            return;
        }
    }

    error_log("SD2: boss_julianneAI: DamageTaken reach end of code, that should not happen.");
}

void boss_romuloAI::DamageTaken(Unit* done_by, uint32 &damage)
{
    if (damage < me->GetHealth())
        return;

    //anything below only used if incoming damage will kill

    if (Phase == PHASE_ROMULO)
    {
        DoScriptText(SAY_ROMULO_DEATH, me);
        PretendToDie(me);
        IsFakingDeath = true;
        Phase = PHASE_BOTH;

        if (Creature* Julianne = ((Creature*)Unit::GetUnit((*me), JulianneGUID)))
        {
            ((boss_julianneAI*)Julianne->AI())->RomuloDead = true;
            ((boss_julianneAI*)Julianne->AI())->ResurrectSelfTimer = 10000;
        }

        damage = 0;
        return;
    }

    if (Phase == PHASE_BOTH)
    {
        if (JulianneDead)
        {
            if (Creature* Julianne = ((Creature*)Unit::GetUnit((*me), JulianneGUID)))
            {
                Julianne->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                Julianne->GetMotionMaster()->Clear();
                Julianne->setDeathState(JUST_DIED);
                Julianne->CombatStop(true);
                Julianne->DeleteThreatList();
                Julianne->SetUInt32Value(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);
            }
            return;
        }

        if (Creature* Julianne = ((Creature*)Unit::GetUnit((*me), JulianneGUID)))
        {
            PretendToDie(me);
            IsFakingDeath = true;
            ((boss_julianneAI*)Julianne->AI())->ResurrectTimer = 10000;
            ((boss_julianneAI*)Julianne->AI())->RomuloDead = true;
            damage = 0;
            return;
        }
    }

    error_log("SD2: boss_romuloAI: DamageTaken reach end of code, that should not happen.");
}

void boss_julianneAI::UpdateAI(const uint32 diff)
{
    if (EntryYellTimer)
    {
        if (EntryYellTimer <= diff)
        {
            DoScriptText(SAY_JULIANNE_ENTER, me);
            EntryYellTimer = 0;
        }else EntryYellTimer -= diff;
    }

    if (AggroYellTimer)
    {
        if (AggroYellTimer <= diff)
        {
            DoScriptText(SAY_JULIANNE_AGGRO, me);
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            me->setFaction(16);
            AggroYellTimer = 0;
        }else AggroYellTimer -= diff;
    }

    if (DrinkPoisonTimer)
    {
        //will do this 2secs after spell hit. this is time to display visual as expected
        if (DrinkPoisonTimer <= diff)
        {
            PretendToDie(me);
            Phase = PHASE_ROMULO;
            SummonRomuloTimer = 10000;
            DrinkPoisonTimer = 0;
        }else DrinkPoisonTimer -= diff;
    }

    if (Phase == PHASE_ROMULO && !SummonedRomulo)
    {
        if (SummonRomuloTimer < diff)
        {
            if (Creature* pRomulo = me->SummonCreature(CREATURE_ROMULO, ROMULO_X, ROMULO_Y, me->GetPositionZ(), 0, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, HOUR*2*IN_MILLISECONDS))
            {
                RomuloGUID = pRomulo->GetGUID();
                ((boss_romuloAI*)pRomulo->AI())->JulianneGUID = me->GetGUID();
                ((boss_romuloAI*)pRomulo->AI())->Phase = PHASE_ROMULO;
                pRomulo->SetInCombatWithZone();

                //why?
                pRomulo->setFaction(16);
            }
            SummonedRomulo = true;
        }else SummonRomuloTimer -= diff;
    }

    if (ResurrectSelfTimer)
    {
        if (ResurrectSelfTimer <= diff)
        {
            Resurrect(me);
            Phase = PHASE_BOTH;
            IsFakingDeath = false;

            if (me->getVictim())
                AttackStart(me->getVictim());

            ResurrectSelfTimer = 0;
            ResurrectTimer = 1000;
        }else ResurrectSelfTimer -= diff;
    }

    if (CanDoSomething() || IsFakingDeath)
        return;

    if (RomuloDead)
    {
        if (ResurrectTimer < diff)
        {
            Creature* Romulo = ((Creature*)Unit::GetUnit((*me), RomuloGUID));
            if (Romulo && ((boss_romuloAI*)Romulo->AI())->IsFakingDeath)
            {
                DoScriptText(SAY_JULIANNE_RESURRECT, me);
                Resurrect(Romulo);
                ((boss_romuloAI*)Romulo->AI())->IsFakingDeath = false;
                RomuloDead = false;
                ResurrectTimer = 10000;
            }
        }else ResurrectTimer -= diff;
    }

    if (BlindingPassionTimer < diff)
    {
        DoCast(SelectUnit(SELECT_TARGET_RANDOM, 0), SPELL_BLINDING_PASSION);
        BlindingPassionTimer = urand(30000, 45000);
    } else BlindingPassionTimer -= diff;

    if (DevotionTimer < diff)
    {
        DoCastMe( SPELL_DEVOTION);
        DevotionTimer = urand(15000, 45000);
    } else DevotionTimer -= diff;

    if (PowerfulAttractionTimer < diff)
    {
        DoCast(SelectUnit(SELECT_TARGET_RANDOM, 0), SPELL_POWERFUL_ATTRACTION);
        PowerfulAttractionTimer = urand(5000, 30000);
    } else PowerfulAttractionTimer -= diff;

    if (EternalAffectionTimer < diff)
    {
        if (urand(0, 1) && SummonedRomulo)
        {
            Creature* Romulo = ((Creature*)Unit::GetUnit((*me), RomuloGUID));
            if (Romulo && Romulo->isAlive() && !RomuloDead)
                DoCast(Romulo, SPELL_ETERNAL_AFFECTION);
        } else DoCastMe( SPELL_ETERNAL_AFFECTION);

        EternalAffectionTimer = urand(45000, 60000);
    } else EternalAffectionTimer -= diff;

    DoMeleeAttackIfReady();
}

void boss_romuloAI::UpdateAI(const uint32 diff)
{
    if (CanDoSomething() || IsFakingDeath)
        return;

    if (JulianneDead)
    {
        if (ResurrectTimer < diff)
        {
            Creature* Julianne = ((Creature*)Unit::GetUnit((*me), JulianneGUID));
            if (Julianne && ((boss_julianneAI*)Julianne->AI())->IsFakingDeath)
            {
                DoScriptText(SAY_ROMULO_RESURRECT, me);
                Resurrect(Julianne);
                ((boss_julianneAI*)Julianne->AI())->IsFakingDeath = false;
                JulianneDead = false;
                ResurrectTimer = 10000;
            }
        } else ResurrectTimer -= diff;
    }

    if (BackwardLungeTimer < diff)
    {
        Unit* target = SelectUnit(SELECT_TARGET_RANDOM, 1);
        if (target && !me->HasInArc(M_PI, target))
        {
            DoCast(target, SPELL_BACKWARD_LUNGE);
            BackwardLungeTimer = urand(15000, 30000);
        }
    }else BackwardLungeTimer -= diff;

    if (DaringTimer < diff)
    {
        DoCastMe( SPELL_DARING);
        DaringTimer = urand(20000, 40000);
    }else DaringTimer -= diff;

    if (DeadlySwatheTimer < diff)
    {
        DoCast(SelectUnit(SELECT_TARGET_RANDOM, 0), SPELL_DEADLY_SWATHE);
        DeadlySwatheTimer = urand(15000, 25000);
    }else DeadlySwatheTimer -= diff;

    if (PoisonThrustTimer < diff)
    {
        DoCastVictim( SPELL_POISON_THRUST);
        PoisonThrustTimer = urand(10000, 20000);
    }else PoisonThrustTimer -= diff;

    DoMeleeAttackIfReady();
}

CreatureAI* GetAI_boss_julianne(Creature* pCreature)
{
    return new boss_julianneAI(pCreature);
}

CreatureAI* GetAI_boss_romulo(Creature* pCreature)
{
    return new boss_romuloAI(pCreature);
}

void AddSC_bosses_opera()
{
    Script* newscript;

    // Oz
    newscript = new Script;
    newscript->GetAI = &GetAI_boss_dorothee;
    newscript->Name = "boss_dorothee";
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->GetAI = &GetAI_boss_strawman;
    newscript->Name = "boss_strawman";
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->GetAI = &GetAI_boss_tinhead;
    newscript->Name = "boss_tinhead";
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->GetAI = &GetAI_boss_roar;
    newscript->Name = "boss_roar";
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->GetAI = &GetAI_boss_crone;
    newscript->Name = "boss_crone";
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->GetAI = &GetAI_mob_tito;
    newscript->Name = "mob_tito";
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->GetAI = &GetAI_mob_cyclone;
    newscript->Name = "mob_cyclone";
    newscript->RegisterSelf();

    // Hood
    newscript = new Script;
    newscript->pGossipHello = &GossipHello_npc_grandmother;
    newscript->pGossipSelect = &GossipSelect_npc_grandmother;
    newscript->Name = "npc_grandmother";
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->GetAI = &GetAI_boss_bigbadwolf;
    newscript->Name = "boss_bigbadwolf";
    newscript->RegisterSelf();

    // Romeo And Juliet
    newscript = new Script;
    newscript->GetAI = &GetAI_boss_julianne;
    newscript->Name = "boss_julianne";
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->GetAI = &GetAI_boss_romulo;
    newscript->Name = "boss_romulo";
    newscript->RegisterSelf();
}
