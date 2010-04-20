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
SDName: Howling_Fjord
SD%Complete: ?
SDComment: Quest support: 11221
SDCategory: Howling Fjord
EndScriptData */

#include "precompiled.h"

#define		   RUNNER_SNOW			 24211
#define        NPC_BABY_PROTO        24160
#define		   VEUVE_CLIVE			 23958

struct MANGOS_DLL_DECL proto_egg_AI : public ScriptedAI
{
    proto_egg_AI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    void Reset() {}
    void Aggro(Unit* pWho)
    { me->SetSpeedRate(MOVE_WALK,0.0f,true); }

    void JustDied(Unit* pKiller)
    {
        if(urand(0,3) == 1)
        {
            me->SummonCreature(NPC_BABY_PROTO,me->GetPositionX() ,me->GetPositionY(), me->GetPositionZ()+0.5, 0,TEMPSUMMON_TIMED_DESPAWN,60000);
        }
    }
};

struct MANGOS_DLL_DECL web_IA : public ScriptedAI
{
    web_IA(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    void Reset() {}

	void Aggro(Unit* pWho)
    { me->SetSpeedRate(MOVE_WALK,0.0f,true); }

	void JustDied(Unit* pKiller)
    {
        switch(urand(0,3))
        {
			case 1:
				me->SummonCreature(RUNNER_SNOW,me->GetPositionX() ,me->GetPositionY(), me->GetPositionZ()+0.5, 0,TEMPSUMMON_TIMED_DESPAWN,60000);
				((Player*) pKiller)->KilledMonsterCredit(RUNNER_SNOW,0);
				break;
			case 2:
			case 0:
				me->SummonCreature(VEUVE_CLIVE,me->GetPositionX() ,me->GetPositionY(), me->GetPositionZ()+0.5, 0,TEMPSUMMON_TIMED_DESPAWN,60000);
				break;
			default:
				break;
        }
    }
	
};

CreatureAI* GetAI_proto_egg(Creature* pCreature)
{
    return new proto_egg_AI(pCreature);
}

CreatureAI* GetAI_web(Creature* pCreature)
{
    return new web_IA(pCreature);
}

/*#######################
## Deathstalker Razael ##
#######################*/

#define GOSSIP_ITEM_DEATHSTALKER_RAZAEL "High Executor Anselm requests your report."
enum
{
   QUEST_REPORTS_FROM_THE_FIELD       = 11221,
   SPELL_RAZAEL_KILL_CREDIT           = 42756,
   GOSSIP_TEXTID_DEATHSTALKER_RAZAEL1 = 11562,
   GOSSIP_TEXTID_DEATHSTALKER_RAZAEL2 = 11564
};
bool GossipHello_npc_deathstalker_razael(Player* pPlayer, Creature* pCreature)
{
   if (pCreature->isQuestGiver())
       pPlayer->PrepareQuestMenu(pCreature->GetGUID());

   if (pPlayer->GetQuestStatus(QUEST_REPORTS_FROM_THE_FIELD) == QUEST_STATUS_INCOMPLETE)
   {
       pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_DEATHSTALKER_RAZAEL, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
       pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_DEATHSTALKER_RAZAEL1, pCreature->GetGUID());
   }
   else
       pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());
   return true;
}

bool GossipSelect_npc_deathstalker_razael(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
   switch(uiAction)
   {
       case GOSSIP_ACTION_INFO_DEF+1:
           pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_DEATHSTALKER_RAZAEL2, pCreature->GetGUID());
           pCreature->CastSpell(pPlayer, SPELL_RAZAEL_KILL_CREDIT, true);
            break;
   }
   return true;
}

/*#####################
## Dark Ranger Lyana ##
#####################*/

#define GOSSIP_ITEM_DARK_RANGER_LYANA "High Executor Anselm requests your report."

enum
{
   GOSSIP_TEXTID_DARK_RANGER_LYANA1    = 11586,
   GOSSIP_TEXTID_DARK_RANGER_LYANA2    = 11588,
   SPELL_DARK_RANGER_LYANA_KILL_CREDIT = 42799
};

bool GossipHello_npc_dark_ranger_lyana(Player* pPlayer, Creature* pCreature)
{
   if (pCreature->isQuestGiver())
       pPlayer->PrepareQuestMenu(pCreature->GetGUID());

   if (pPlayer->GetQuestStatus(QUEST_REPORTS_FROM_THE_FIELD) == QUEST_STATUS_INCOMPLETE)
   {
       pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_DARK_RANGER_LYANA, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
       pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_DARK_RANGER_LYANA1, pCreature->GetGUID());
   }
   else
       pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());
   return true;
}

bool GossipSelect_npc_dark_ranger_lyana(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
   switch(uiAction)
   {
       case GOSSIP_ACTION_INFO_DEF+1:
           pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_DARK_RANGER_LYANA2, pCreature->GetGUID());
           pCreature->CastSpell(pPlayer, SPELL_DARK_RANGER_LYANA_KILL_CREDIT, true);
            break;
   }
   return true;
}

#define GOSSIP_ITEM_HARRY "Je viens reprendre l'argent que vous devez � Taruk"
enum
{
	GAMBLING_DEBT = 11464,
	SAY_HARRY_PAID = -2000000, //TODO : ajouter une entrée dans la DB
	SAY_HARRY_DONT_PAY = -2000001,
	SAY_HARRY_CANT_PAY = -2000002,
};
//fss
bool GossipHello_npc_Harry_Silvermoon(Player* pPlayer, Creature* pCreature)
{
	if (pCreature->isQuestGiver())
		pPlayer->PrepareQuestMenu(pCreature->GetGUID());

	if (pPlayer->GetQuestStatus(GAMBLING_DEBT) == QUEST_STATUS_INCOMPLETE)
		pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_HARRY, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

	pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());
	return true;
}

struct MANGOS_DLL_DECL npc_Harry_SilvermoonAI : public ScriptedAI
{
	uint32 m_uiNpcFlags;

	bool bEvent;
	bool bActiveAttack;
	uint32 m_uiSayTimer;
	uint64 m_uiPlayerGUID;

	npc_Harry_SilvermoonAI(Creature* pCreature) : ScriptedAI(pCreature)
	{
		m_uiNpcFlags = pCreature->GetUInt32Value(UNIT_NPC_FLAGS);
		Reset();
	}

	void Reset()
	{
		me->SetUInt32Value(UNIT_NPC_FLAGS, m_uiNpcFlags);
		me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
		me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNK_9);
		me->setFaction(35);

		bEvent = false;
		bActiveAttack = false;
		m_uiSayTimer = 1000;
	}

	void StartEvent(Player* pPlayer)
	{
		me->SetUInt32Value(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_NONE);
		DoScriptText(SAY_HARRY_DONT_PAY, me, pPlayer);
		me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
		me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNK_9);
		me->AddThreat(pPlayer,3.0f);
		bEvent = true;
		me->setFaction(40);
	}
	
	void AddQuestItem(Player* pPlayer)
	{
	    //Adding items
		uint32 noSpaceForCount = 0;
		uint32 count = 1;
		uint32 itemId = 34115;
		// check space and find places
		ItemPosCountVec dest;
		uint8 msg = pPlayer->CanStoreNewItem( NULL_BAG, NULL_SLOT, dest, itemId, count, &noSpaceForCount );
		if( msg != EQUIP_ERR_OK )                               // convert to possible store amount
			count -= noSpaceForCount;

		Item* item = pPlayer->StoreNewItem( dest, itemId, true, Item::GenerateItemRandomPropertyId(itemId));

		if(count > 0 && item)
		{
			pPlayer->SendNewItem(item,count,true,false);
		}
		
		if(noSpaceForCount > 0)
			DoScriptText(SAY_HARRY_CANT_PAY, me, pPlayer);
		else
			DoScriptText(SAY_HARRY_PAID, me, pPlayer);
	}

	void DamageTaken(Unit* pDoneBy, uint32 &damage)
	{
		if (damage < me->GetHealth())
			return;

		//damage will kill, this is pretty much the same as 1%HP left
		if (bEvent)
		{
			damage = 0;
			if (Player* pPlayer = (Player*)Unit::GetUnit(*me, m_uiPlayerGUID))
			{
				ThreatList const& threatlist = me->getThreatManager().getThreatList();

				for(ThreatList::const_iterator itr = threatlist.begin(); itr != threatlist.end(); ++itr)
				{
					Unit* pUnit = Unit::GetUnit((*me), (*itr)->getUnitGuid());
					if(pUnit->GetTypeId() == TYPEID_PLAYER && ((Player*)pUnit)->GetQuestStatus(GAMBLING_DEBT) == QUEST_STATUS_INCOMPLETE)
					{
						AddQuestItem((Player*)pUnit);
					}
				}
			}

		}
		EnterEvadeMode();
	}

	void UpdateAI(const uint32 diff)
	{

		if (CanDoSomething())
		return;

		DoMeleeAttackIfReady();
	}
}; 

bool GossipSelect_npc_Harry_Silvermoon(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
	if (uiAction == GOSSIP_ACTION_INFO_DEF+1)
	{
		((npc_Harry_SilvermoonAI*)pCreature->AI())->m_uiPlayerGUID = pPlayer->GetGUID();
		((npc_Harry_SilvermoonAI*)pCreature->AI())->StartEvent(pPlayer);
		pPlayer->CLOSE_GOSSIP_MENU();
	}

	return true;
}

CreatureAI* GetAI_npc_npc_Harry_SilvermoonAI(Creature* pCreature)
{
    return new npc_Harry_SilvermoonAI(pCreature);
}

/*############
## McGoyver ##
############*/

#define GOSSIP_ITEM_MCGOYVER1 "Walt sent me to pick up some dark iron ingots."
#define GOSSIP_ITEM_MCGOYVER2 "Yarp."

enum
{
    QUEST_WE_CAN_REBUILD_IT                = 11483,
    GOSSIP_TEXTID_MCGOYVER                 = 12193,
    ITEM_DARK_IRON_INGOTS                  = 34135,
    SPELL_MCGOYVER_TAXI_EXPLORERSLEAGUE    = 44280,
    SPELL_MCGOYVER_CREATE_DARK_IRON_INGOTS = 44512
};

bool GossipHello_npc_mcgoyver(Player* pPlayer, Creature* pCreature)
{
    switch(pPlayer->GetQuestStatus(QUEST_WE_CAN_REBUILD_IT))
    {
        case QUEST_STATUS_INCOMPLETE:
            if (!pPlayer->HasItemCount(ITEM_DARK_IRON_INGOTS, 1, true))
            {
                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_MCGOYVER1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
                pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());
            }
            else
            {
                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_MCGOYVER2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
                pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_MCGOYVER, pCreature->GetGUID());
            }
            break;
        case QUEST_STATUS_COMPLETE:
            if (!pPlayer->GetQuestRewardStatus(QUEST_WE_CAN_REBUILD_IT))
            {
                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_MCGOYVER2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
                pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_MCGOYVER, pCreature->GetGUID());
                break;
            }
        default:
            pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());
    }

    return true;
}

bool GossipSelect_npc_mcgoyver(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    switch(uiAction)
    {
        case GOSSIP_ACTION_INFO_DEF+1:
            pCreature->CastSpell(pPlayer, SPELL_MCGOYVER_CREATE_DARK_IRON_INGOTS, true);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_MCGOYVER2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_MCGOYVER, pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+2:
            pPlayer->CLOSE_GOSSIP_MENU();
            pCreature->CastSpell(pPlayer, SPELL_MCGOYVER_TAXI_EXPLORERSLEAGUE, true);
            break;
    }

    return true;
}


void AddSC_howling_fjord()
{
   Script *newscript;

    newscript = new Script;
    newscript->Name = "npc_deathstalker_razael";
    newscript->pGossipHello = &GossipHello_npc_deathstalker_razael;
    newscript->pGossipSelect = &GossipSelect_npc_deathstalker_razael;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_dark_ranger_lyana";
    newscript->pGossipHello = &GossipHello_npc_dark_ranger_lyana;
    newscript->pGossipSelect = &GossipSelect_npc_dark_ranger_lyana;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "proto_egg";
    newscript->GetAI = &GetAI_proto_egg;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "web_spid_AI";
    newscript->GetAI = &GetAI_web;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "npc_harry_silvermoon";
	newscript->GetAI = &GetAI_npc_npc_Harry_SilvermoonAI;
    newscript->pGossipHello = &GossipHello_npc_Harry_Silvermoon;
    newscript->pGossipSelect = &GossipSelect_npc_Harry_Silvermoon;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "npc_mcgoyver";
    newscript->pGossipHello = &GossipHello_npc_mcgoyver;
    newscript->pGossipSelect = &GossipSelect_npc_mcgoyver;
    newscript->RegisterSelf();


}
