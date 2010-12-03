#include "BattleGround.h"
#include "BattleGroundMgr.h"
#include "PlayerBot.h"

PlayerBot::PlayerBot(WorldSession* session)//: Player(session)
{
	specIdx = 0;
	m_decideToFight = false;
}

PlayerBot::~PlayerBot()
{
	delete bot;
}

void PlayerBot::GoToCacIfIsnt(Unit* target)
{
	if(!target)
		return;

	if(bot->GetDistance2d(target) >= 4.5f)
		bot->GetMotionMaster()->MoveChase(target,3.0f);
}

void PlayerBot::Stay()
{
	bot->GetMotionMaster()->Clear();
}

void PlayerBot::Update(uint32 diff)
{
	ASSERT(bot);
	
	if(bot->IsInWorld() && !bot->InBattleGroundQueue() && !bot->GetBattleGround())
	{
		BattleGroundQueue& bgQueue = sBattleGroundMgr.m_BattleGroundQueues[BATTLEGROUND_QUEUE_RANDOM];
		BattleGround* bg = sBattleGroundMgr.GetBattleGroundTemplate(BATTLEGROUND_RB);
		if(!bg)	return;

		PvPDifficultyEntry const* bracketEntry = GetBattlegroundBracketByLevel(bg->GetMapId(),bot->getLevel());
		if(!bracketEntry) return;

		bgQueue.AddGroup(bot, NULL, BATTLEGROUND_RB, bracketEntry, 0, false, false, 0);
		bot->AddBattleGroundQueueId(BATTLEGROUND_QUEUE_RANDOM);
	}

	// CombatHandler for all classes
	if(HasDecidedToFight())
	{
		switch(bot->getClass())
		{
			case CLASS_WARRIOR:
				HandleWarriorCombat();
				break;
			case CLASS_PALADIN:
				HandlePaladinCombat();
				break;
			case CLASS_HUNTER:
				HandleHunterCombat();
				break;
			case CLASS_ROGUE:
				HandleRogueCombat();
				break;
			case CLASS_PRIEST:
				HandlePriestCombat();
				break;
			case CLASS_DEATH_KNIGHT:
				HandleDKCombat();
				break;
			case CLASS_SHAMAN:
				HandleShamanCombat();
				break;
			case CLASS_MAGE:
				HandleMageCombat();
				break;
			case CLASS_WARLOCK:
				HandleWarlockCombat();
				break;
			case CLASS_DRUID:
				HandleDruidCombat();
				break;
		}
	}
}


void PlayerBot::HandleRogueCombat()
{
	switch(specIdx)
	{
		case 0: // Assass
			break;
		case 1: // Combat
			break;
		case 2: // Finesse
			break;
	}
}

void PlayerBot::HandleShamanCombat()
{
	switch(specIdx)
	{
		case 0: // Heal
			break;
		case 1: // Elem
			break;
		case 2: // Cac
			break;
	}
}

void PlayerBot::HandleDKCombat()
{
	switch(specIdx)
	{
		case 0: // Sang
			break;
		case 1: // Givre
			break;
		case 2: // impie&
			break;
	}
}

void PlayerBot::HandleDruidCombat()
{
	switch(specIdx)
	{
		case 0: // equi
			break;
		case 1: // heal
			break;
		case 2: // cac
			break;
	}
}

void PlayerBot::HandleHunterCombat()
{
	switch(specIdx)
	{
		case 0: // survie
			break;
		case 1: // pr�ci
			break;
		case 2: // ??
			break;
	}
}

void PlayerBot::HandleMageCombat()
{
	switch(specIdx)
	{
		case 0: // feu
			break;
		case 1: // givre
			break;
		case 2: // arcane
			break;
	}
}

void PlayerBot::HandlePaladinCombat()
{
	switch(specIdx)
	{
		case 0: // vindicte
			break;
		case 1: // heal
			break;
		case 2: // proto
			break;
	}
}

void PlayerBot::HandlePriestCombat()
{
	switch(specIdx)
	{
		case 0: // heal
			break;
		case 1: // disci
			break;
		case 2: // shadow
			break;
	}
}

#define POSTURE_DEF			5301
#define POSTURE_ARM			2457
#define POSTURE_FURY		2458
#define SPELL_FRAPPE_HERO	47450
#define SPELL_SANGUINAIRE	2687
#define SPELL_EXEC			47471
#define SPELL_BERSERK		18499
#define SPELL_ENRAGE		2687
#define SPELL_TOURBILLON	1680

void PlayerBot::HandleWarriorCombat()
{
	if(bot->GetPower(POWER_RAGE) < 60)
	{
		bot->CastSpell(bot,SPELL_BERSERK);
		bot->CastSpell(bot,SPELL_ENRAGE);
	}

	if(Unit* target = Unit::GetUnit(*bot,bot->GetTargetGUID()))
	{
		if(bot->IsNonMeleeSpellCasted(false))
			return;
	
		GoToCacIfIsnt(target);

		switch(specIdx)
		{
			case 0: // arme
				if(!bot->HasAura(POSTURE_ARM))
					bot->CastSpell(bot,POSTURE_ARM);
				break;
			case 1: // furie
				if(!bot->HasAura(POSTURE_FURY))
					bot->CastSpell(bot,POSTURE_FURY);

				if(target->GetHealth() * 100.0f / target->GetMaxHealth() < 15.0f && bot->GetPower(POWER_RAGE) >= 35)
					bot->CastSpell(target,SPELL_EXEC);
				
				bot->CastSpell(target,SPELL_TOURBILLON);
				bot->CastSpell(target,SPELL_SANGUINAIRE);

				if(bot->GetPower(POWER_RAGE) >= 40)
					bot->CastSpell(target,SPELL_FRAPPE_HERO);
				break;
			case 2: // proto
				if(!bot->HasAura(POSTURE_DEF))
					bot->CastSpell(bot,POSTURE_DEF);
				break;
		}
	}
}

void PlayerBot::HandleWarlockCombat()
{
	switch(specIdx)
	{
		case 0: // affli
			break;
		case 1: // demono
			break;
		case 2: // destru
			break;
	}
}