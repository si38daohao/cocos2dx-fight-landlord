#ifndef __MIDDLE_CARD_H__
#define __MIDDLE_CARD_H__

#include "Card.h"

class MiddleCard : public Card {
public:
	static MiddleCard * create(int suit, int value, int initSide);
};

#endif