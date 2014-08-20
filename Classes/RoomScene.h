#ifndef __ROOM_SCENE_H__
#define __ROOM_SCENE_H__

#include "cocos2d.h"
#include "cocos-ext.h"
#include "PlayerInfo.h"
#include <vector>

USING_NS_CC;
USING_NS_CC_EXT;

using namespace ui;
using namespace std;


class RoomScene : public cocos2d::CCLayer
{
public:
	virtual bool init();

	static cocos2d::CCScene* scene();

	void onResponse(char * response);

	void dropBtnTouchEvent(CCObject *pSender, TouchEventType type);
	void exitBtnTouchEvent(CCObject *pSender, TouchEventType type);
	void readyBtnTouchEvent(CCObject *pSender, TouchEventType type);
	void bidBtnTouchEvent(CCObject* pSender, TouchEventType type);
	void notBidBtnTouchEvent(CCObject* pSender, TouchEventType type);
	void outbidBtnTouchEvent(CCObject* pSender, TouchEventType type);
	void notOutBidBtnTouchEvent(CCObject* pSender, TouchEventType type);
	void layCardBtnTouchEvent(CCObject* pSender, TouchEventType type);
	void notLayCardBtnTouchEvent(CCObject* pSender, TouchEventType type);

	void prePlayerHeadTouchEvent(CCObject* pSender, TouchEventType type);
	void myPlayerHeadTouchEvent(CCObject* pSender, TouchEventType type);
	void nextPlayerHeadTouchEvent(CCObject* pSender, TouchEventType type);

	CREATE_FUNC(RoomScene);

private:

	UIPanel* myCardsPanel;
	Button* dropBtn;
	UIPanel* toolbarPanel;
	UIPanel* roomInfoPanel;

	Button* exitBtn;

	Button* readyBtn;

	vector<PlayerInfo*>* mPlayerInfoArray;
	vector<Label*>* playerNameLabelArray;
	vector<UIPanel*>* mPlayerCoinPanelArray;
	vector<Label*>* playerCoinLabelArray;

	vector<ImageView*>* playerHeadImageViewArray;
	vector<ImageView*>* playerRoleImageViewArray;
	vector<ImageView*>* mPlayerActionImageViewArray;
	vector<ImageView*>* mPlayerClockImageViewArray;

	vector<UIPanel*>* playerCardCounterPanelArray;
	vector<Label*>* mPlayerCardCounterLabelArray;
	vector<UIPanel*>* mPlayerLaidCardPanelArray;

	UIPanel* readyBtnPanel;
	UIPanel* bidButtonPanel;
	UIPanel* outbidButtonPanel;
	UIPanel* mLayCardButtonPanel;

	UIPanel* myCoinAmountImgPanel;

	UIPanel* kittyCardPanel;

	Label* mBasechipLabel;
	Label* mScoreLabel;

	Label* mMyCoinLabel;

	vector<int>* mKittyCardArray;
	vector<int>* mHandCardArray;
};

#endif
