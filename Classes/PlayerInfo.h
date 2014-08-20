#ifndef __PLAYER_INFO_H__
#define __PLAYER_INFO_H__

#include "cocos2d.h"

USING_NS_CC;

class PlayerInfo : public CCObject {

private:
	const char * id;
	const char * name;
	int coinAmount;

	const char * currentRoomId;
	int currentSeatIndex;

public:
	void setId(const char * id);
	const char * getId();

	void setName(const char * name);
	const char * getName();

	void setCoinAmount(int amount);
	int getCoinAmount();

	void setCurrentRoomId(const char* roomId);
	const char * getCurrentRoomId();

	void setCurrentSeatIndex(int index);
	int getCurrentSeatIndex();
};

#endif