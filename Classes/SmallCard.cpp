#include "SmallCard.h"


SmallCard * SmallCard::create(int suit, int value, int initSide) {
	Card* card = Card::create(suit, value, initSide);

	card->setScale(0.22);

	return static_cast<SmallCard*>(card);
}