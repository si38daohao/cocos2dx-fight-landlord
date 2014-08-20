#ifndef __SMALL_CARD_H__
#define __SMALL_CARD_H__

#include "Card.h"

class SmallCard : public Card {
public:
	static SmallCard * create(int suit, int value, int initSide);
};

#endif