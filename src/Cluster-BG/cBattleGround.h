#ifndef __C_BATTLEGROUND_H_
#define __C_BATTLEGROUND_H_

#include <Common.h>
#include <BattleGround.h>

class cBattleGround
{
	public:
		cBattleGround();
		~cBattleGround() {}
		void SendWarningToAll(int32 entry, ...);
		void SendWarningToAll(std::string str);

		typedef std::map<uint64, BattleGroundPlayer> BattleGroundPlayerMap;
        BattleGroundPlayerMap const& GetPlayers() const { return m_Players; }
        uint32 GetPlayersSize() const { return m_Players.size(); }

		void setId(uint64 id) { m_Id = id; }
		uint64 getId() { return m_Id; }
	protected:
		uint64 m_Id;
		/* Player lists, those need to be accessible by inherited classes */
        BattleGroundPlayerMap  m_Players;
};

#endif