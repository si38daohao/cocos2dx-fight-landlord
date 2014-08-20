#ifndef __CARD_H__
#define __CARD_H__

#include "cocos2d.h"
#include "cocos-ext.h"

#define SUIT_HEART 0
#define SUIT_SPADE 1
#define SUIT_CLUB 2
#define SUIT_DIAMOND 3

#define VALUE_3 3
#define VALUE_4 4
#define VALUE_5 5
#define VALUE_6 6
#define VALUE_7 7
#define VALUE_8 8
#define VALUE_9 9
#define VALUE_10 10
#define VALUE_J 11
#define VALUE_Q 12
#define VALUE_K 13
#define VALUE_A 14
#define VALUE_2 15
#define VALUE_POKER_SMALL 16
#define VALUE_POKER_BIG 17

#define SIDE_FRONT 0
#define SIDE_BACK 1

USING_NS_CC;
USING_NS_CC_EXT;

using namespace ui;

class Card : public UIPanel
{
public:
	static Card * create(int suit, int value, int initSide);
	Card();
	Card(int suit, int value, int initSide);
	~Card();

	void setSuit(int suit);
	int getSuit();
	void setValue(int value);
	int getValue();

	bool isFrontShowing();
	bool isBackShowing();

	void flip();

	void touchEvent(CCObject *pSender, TouchEventType type);

	bool isPoppedUp();

protected:
	virtual bool init();

private:
	int suit;
	int value;

	int sideShowing;

	bool mPoppedUp;
};

#endif