#include "MiddleCard.h"

MiddleCard* MiddleCard::create(int suit, int value, int initSide) {
	Card* card = Card::create(suit, value, initSide);

	card->setScale(0.5);

	return static_cast<MiddleCard*>(card);
}