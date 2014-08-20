#ifndef __PLAYER_INFO_SCENE_H__
#define __PLAYER_INFO_SCENE_H__

#include "cocos2d.h"
#include "cocos-ext.h"
#include "PlayerInfo.h"

USING_NS_CC;
USING_NS_CC_EXT;

using namespace ui;
using namespace std;

class PlayerInfoScene : public cocos2d::CCLayer {
public:
	virtual bool init();
	static CCScene* scene();
	CREATE_FUNC(PlayerInfoScene);

	void closeBtnTouchEvent(CCObject *pSender, TouchEventType type);

	void setPlayerInfo(PlayerInfo* playerInfo);

private:
	Label * mNameLabel;
	Label * mCoinLabel;
};

#endif