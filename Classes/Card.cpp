#include "Card.h"
#include "cocos-ext.h"

USING_NS_CC;

USING_NS_CC_EXT;

using namespace ui;

Card * Card::create(int suit, int value, int initSide) {
	Card * card = new Card(suit, value, initSide);

	if (card && card->init()) {
		card->autorelease();
		UILayout* layout = static_cast<Layout *>(GUIReader::shareReader()->widgetFromJsonFile("card_layout.json"));

		layout->addTouchEventListener(card, toucheventselector(Card::touchEvent));

		ImageView * leftTopImg = static_cast<ImageView *>(layout->getChildByName("left_top_img"));
		ImageView * smallSuitImg = static_cast<ImageView *>(layout->getChildByName("small_suit_image"));
		ImageView * bigSuitImg = static_cast<ImageView *>(layout->getChildByName("big_suit_image"));
		ImageView * backImg = static_cast<ImageView *>(layout->getChildByName("card_back_image"));

		if (value == VALUE_POKER_SMALL) {
			leftTopImg->loadTexture("bigcard/jokerwordblack.png");
			smallSuitImg->setVisible(false);
			bigSuitImg->loadTexture("bigcard/jokerblack.png");
		} else if (value == VALUE_POKER_BIG) {
			leftTopImg->loadTexture("bigcard/jokerwordred.png");
			smallSuitImg->setVisible(false);
			bigSuitImg->loadTexture("bigcard/jokerred.png");
		} else {
			char * texturePath = new char[128];
			switch (suit)
			{
			case SUIT_SPADE:
				smallSuitImg->loadTexture("bigcard/spadeSmall.png");

				if (value == VALUE_J) {
					bigSuitImg->loadTexture("bigcard/jblack.png");
				} else if (value == VALUE_Q) {
					bigSuitImg->loadTexture("bigcard/qblack.png");
				} else if (value == VALUE_K) {
					bigSuitImg->loadTexture("bigcard/kblack.png");
				} else {
					bigSuitImg->loadTexture("bigcard/plumBig.png");
				}

				if (value == VALUE_A || value == VALUE_2) {
					sprintf(texturePath, "bigcard/%dblack.png", value - VALUE_K);
				} else {
					sprintf(texturePath, "bigcard/%dblack.png", value);
				}
				break;
			case SUIT_CLUB:
				smallSuitImg->loadTexture("bigcard/plumSmall.png");

				if (value == VALUE_J) {
					bigSuitImg->loadTexture("bigcard/jblack.png");
				} else if (value == VALUE_Q) {
					bigSuitImg->loadTexture("bigcard/qblack.png");
				} else if (value == VALUE_K) {
					bigSuitImg->loadTexture("bigcard/kblack.png");
				} else {
					bigSuitImg->loadTexture("bigcard/plumBig.png");
				}


				if (value == VALUE_A || value == VALUE_2) {
					sprintf(texturePath, "bigcard/%dblack.png", value - VALUE_K);
				} else {
					sprintf(texturePath, "bigcard/%dblack.png", value);
				}
				break;
			case SUIT_HEART:
				smallSuitImg->loadTexture("bigcard/heartSmall.png");

				if (value == VALUE_J) {
					bigSuitImg->loadTexture("bigcard/jred.png");
				} else if (value == VALUE_Q) {
					bigSuitImg->loadTexture("bigcard/qred.png");
				} else if (value == VALUE_K) {
					bigSuitImg->loadTexture("bigcard/kred.png");
				} else {
					bigSuitImg->loadTexture("bigcard/heartBig.png");
				}

				if (value == VALUE_A || value == VALUE_2) {
					sprintf(texturePath, "bigcard/%dred.png", value - VALUE_K);
				} else {
					sprintf(texturePath, "bigcard/%dred.png", value);
				}
				break;
			case SUIT_DIAMOND:
				smallSuitImg->loadTexture("bigcard/squareSmall.png");

				if (value == VALUE_J) {
					bigSuitImg->loadTexture("bigcard/jred.png");
				} else if (value == VALUE_Q) {
					bigSuitImg->loadTexture("bigcard/qred.png");
				} else if (value == VALUE_K) {
					bigSuitImg->loadTexture("bigcard/kred.png");
				} else {
					bigSuitImg->loadTexture("bigcard/squareBig.png");
				}

				if (value == VALUE_A || value == VALUE_2) {
					sprintf(texturePath, "bigcard/%dred.png", value - VALUE_K);
				} else {
					sprintf(texturePath, "bigcard/%dred.png", value);
				}
				break;
			default:
				break;
			}

			leftTopImg->loadTexture(texturePath);
		}

		if (initSide == SIDE_BACK) {
			backImg->setVisible(true);
		} else {
			backImg->setVisible(false);
		}

		card->addChild(layout);
		card->setSize(CCSize(168, 234));

		return card;
	}
	CC_SAFE_DELETE(card);
	return NULL;
}

Card::Card() {
	
}

Card::Card(int suit, int value, int initSide) {
	this->suit = suit;
	this->value = value;

	this->sideShowing = initSide;

	this->mPoppedUp = false;
}

Card::~Card() {

}

bool Card::init() {
	if (UIPanel::init())
    {
        setTouchEnabled(true);
        return true;
    }
    return false;
}

void Card::setSuit(int suit) {
	this->suit = suit;
}

int Card::getSuit() {
	return this->suit;
}

void Card::setValue(int value) {
	this->value = value;
}

int Card::getValue() {
	return this->value;
}

bool Card::isFrontShowing() {
	return this->sideShowing == SIDE_FRONT ? true : false;
}

bool Card::isBackShowing() {
	return this->sideShowing == SIDE_BACK ? true : false;
}

void Card::flip() {

	ImageView * backImg = static_cast<ImageView *>(UIHelper::seekWidgetByName(this, "card_back_image"));//static_cast<ImageView *>(this->getChildByName("card_back_image"));
	if (this->sideShowing == SIDE_BACK) {
		backImg->setVisible(false);
		this->sideShowing = SIDE_FRONT;
	} else {
		backImg->setVisible(true);
		this->sideShowing = SIDE_BACK;
	}

}

void Card::touchEvent(CCObject *pSender, TouchEventType type) {
	if (type == TOUCH_EVENT_ENDED) {
		CCLog("Jonathan: in Card::touchEvent");

		if (mPoppedUp == false) {
			CCPoint point = this->getPosition();
			this->setPosition(CCPoint(point.x, point.y + 50));
			mPoppedUp = true;
		} else {
			CCPoint point = this->getPosition();
			this->setPosition(CCPoint(point.x, point.y - 50));
			mPoppedUp = false;
		}
	}
}

bool Card::isPoppedUp() {
	return mPoppedUp;
}