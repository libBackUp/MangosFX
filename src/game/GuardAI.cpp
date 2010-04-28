/*
 * Copyright (C) 2005-2010 MaNGOS <http://getmangos.com/>
 *
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

#include "GuardAI.h"
#include "Errors.h"
#include "Creature.h"
#include "Player.h"
#include "ObjectAccessor.h"
#include "World.h"

int GuardAI::Permissible(const Creature *creature)
{
    if( creature->isGuard())
        return PERMIT_BASE_SPECIAL;

    return PERMIT_BASE_NO;
}

GuardAI::GuardAI(Creature *c) : CreatureAI(c), i_victimGuid(0), i_state(STATE_NORMAL), i_tracker(TIME_INTERVAL_LOOK)
{
}

void GuardAI::MoveInLineOfSight(Unit *u)
{
    // Ignore Z for flying creatures
    if (!me->canFly() && me->GetDistanceZ(u) > CREATURE_Z_ATTACK_RANGE)
        return;

    if (!me->getVictim() && u->isTargetableForAttack() &&
        ( u->IsHostileToPlayers() || me->IsHostileTo(u) /*|| u->getVictim() && me->IsFriendlyTo(u->getVictim())*/ ) &&
        u->isInAccessablePlaceFor(me))
    {
        float attackRadius = me->GetAttackDistance(u);
        if (me->IsWithinDistInMap(u,attackRadius))
        {
            //Need add code to let guard support player
            AttackStart(u);
            u->RemoveSpellsCausingAura(SPELL_AURA_MOD_STEALTH);
        }
    }
}

void GuardAI::EnterEvadeMode()
{
    if (!me->isAlive())
    {
        DEBUG_LOG("Creature stopped attacking because he's dead [guid=%u]", me->GetGUIDLow());
        me->StopMoving();
        me->GetMotionMaster()->MoveIdle();

        i_state = STATE_NORMAL;

        i_victimGuid = 0;
        me->CombatStop(true);
        me->DeleteThreatList();
        return;
    }

    Unit* victim = ObjectAccessor::GetUnit(*me, i_victimGuid );

    if (!victim)
    {
        DEBUG_LOG("Creature stopped attacking, no victim [guid=%u]", me->GetGUIDLow());
    }
    else if (!victim->isAlive())
    {
        DEBUG_LOG("Creature stopped attacking, victim is dead [guid=%u]", me->GetGUIDLow());
    }
    else if (victim->HasStealthAura())
    {
        DEBUG_LOG("Creature stopped attacking, victim is in stealth [guid=%u]", me->GetGUIDLow());
    }
    else if (victim->isInFlight())
    {
        DEBUG_LOG("Creature stopped attacking, victim is in flight [guid=%u]", me->GetGUIDLow());
    }
    else
    {
        DEBUG_LOG("Creature stopped attacking, victim out run him [guid=%u]", me->GetGUIDLow());
    }

    me->RemoveAllAuras();
    me->DeleteThreatList();
    i_victimGuid = 0;
    me->CombatStop(true);
    i_state = STATE_NORMAL;

    // Remove ChaseMovementGenerator from MotionMaster stack list, and add HomeMovementGenerator instead
    if (me->GetMotionMaster()->GetCurrentMovementGeneratorType() == CHASE_MOTION_TYPE)
        me->GetMotionMaster()->MoveTargetedHome();
}

void GuardAI::UpdateAI(const uint32 /*diff*/)
{
    // update i_victimGuid if i_creature.getVictim() !=0 and changed
    if (!me->SelectHostileTarget() || !me->getVictim())
        return;

    i_victimGuid = me->getVictim()->GetGUID();

    if (me->isAttackReady())
    {
        if (me->IsWithinDistInMap(me->getVictim(), ATTACK_DISTANCE))
        {
            me->AttackerStateUpdate(me->getVictim());
            me->resetAttackTimer();
        }
    }
}

bool GuardAI::IsVisible(Unit *pl) const
{
    return me->IsWithinDist(pl,sWorld.getConfig(CONFIG_SIGHT_GUARDER))
        && pl->isVisibleForOrDetect(me,me,true);
}

void GuardAI::AttackStart(Unit *u)
{
    if( !u )
        return;

    //    DEBUG_LOG("Creature %s tagged a victim to kill [guid=%u]", i_creature.GetName(), u->GetGUIDLow());
    if(me->Attack(u,true))
    {
        i_victimGuid = u->GetGUID();
        me->AddThreat(u);
        me->SetInCombatWith(u);
        u->SetInCombatWith(me);

        me->GetMotionMaster()->MoveChase(u);
    }
}

void GuardAI::JustDied(Unit *killer)
{
    if(Player* pkiller = killer->GetCharmerOrOwnerPlayerOrPlayerItself())
        me->SendZoneUnderAttackMessage(pkiller);
}
