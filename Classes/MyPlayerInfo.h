#ifndef __MY_PLAYER_INFO_H__
#define __MY_PLAYER_INFO_H__

#include "PlayerInfo.h"

class MyPlayerInfo : public PlayerInfo {

public:
	static MyPlayerInfo* getInstance();
};

#endif