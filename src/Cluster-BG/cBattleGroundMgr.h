#ifndef __C_BATTLEGROUND_MGR_H_
#define __C_BATTLEGROUND_MGR_H_

#include <Common.h>
#include "cBattleGround.h"

typedef std::map<uint64,cBattleGround*> cBattleGroundMap;
class cBattleGroundMgr
{
	public:
		cBattleGroundMgr() {}
		~cBattleGroundMgr() {}
		uint64 CreateBattleGround();
		void DropBattleGround(uint64 id);
		cBattleGround* getBattleGround(uint64 id);

	private:
		cBattleGroundMap m_BGMap;
};

#define sClusterBGMgr MaNGOS::Singleton<cBattleGroundMgr>::Instance()

#endif