#include "RoomScene.h"
#include "cocos-ext.h"
#include "Card.h"
#include <vector>
#include "NetworkCommunication.h"
#include "MyPlayerInfo.h"
#include "CommandCodeDef.h"
#include "CocoStudio\Json\rapidjson\stringbuffer.h"
#include "CocoStudio\Json\rapidjson\writer.h"
#include "PlayerInfoScene.h"
#include "algorithm"
#include "SmallCard.h"
#include "MiddleCard.h"

USING_NS_CC;

USING_NS_CC_EXT;

using namespace ui;
using namespace extension;
using namespace rapidjson;
using namespace std;

CCScene * RoomScene::scene() {
	// 'scene' is an autorelease object
    CCScene *scene = CCScene::create();
    // 'layer' is an autorelease object
	RoomScene *layer = RoomScene::create();
     //add layer as a child to scene
    scene->addChild(layer);

	return scene;
}

bool RoomScene::init() {
	if ( !CCLayer::init() )
    {
        return false;
    }

	extension::NetworkCommunication::getInstance()->setResponseCallback(this, httpresponse2_selector(RoomScene::onResponse));

	UILayer* layer = UILayer::create();
	UILayout* layout = static_cast<Layout *>(GUIReader::shareReader()->widgetFromJsonFile("room_layout.json"));
	layer->addWidget(layout);
	this->addChild(layer);

	// insert cards
	UIPanel * myCardsPanel = UIPanel::create();
	myCardsPanel->setLayoutType(LAYOUT_ABSOLUTE);
	//myCardsPanel->setBackGroundImage("HelloWorld.png");
	//myCardsPanel->setBackGroundImageScale9Enabled(true);
	RelativeLayoutParameter * layoutParam = RelativeLayoutParameter::create();
	layoutParam->setAlign(RELATIVE_ALIGN_PARENT_TOP_CENTER_HORIZONTAL);
	layoutParam->setMargin(Margin(0, 525, 0, 0));
	myCardsPanel->setLayoutParameter(layoutParam);
	myCardsPanel->setSize(CCSize(1280, 234));

	RelativeLayoutParameter * layoutParam2 = RelativeLayoutParameter::create();
	layoutParam2->setAlign(RELATIVE_ALIGN_PARENT_TOP_LEFT);

	/*for (int i = 0; i < 5; i++) {
		Card * card = Card::create(SUIT_HEART, VALUE_POKER_BIG, SIDE_BACK);
		card->setPosition(ccp(60 * i, 0));
		myCardsPanel->addChild(card);
	}*/

	layout->addChild(myCardsPanel, 1);
	this->myCardsPanel = myCardsPanel;

	// get all widgets needed when updating ui
	dropBtn = static_cast<Button *>(layer->getWidgetByName("drop_btn"));
	dropBtn->addTouchEventListener(this, toucheventselector(RoomScene::dropBtnTouchEvent));
	this->toolbarPanel = static_cast<UIPanel *>(layer->getWidgetByName("toolbar_container"));
	this->roomInfoPanel = static_cast<UIPanel *>(layer->getWidgetByName("room_info_container"));
	this->exitBtn = static_cast<Button *>(layer->getWidgetByName("exit_btn"));
	this->exitBtn->addTouchEventListener(this, toucheventselector(RoomScene::exitBtnTouchEvent));
	this->readyBtn = static_cast<Button *>(layer->getWidgetByName("ready_btn"));
	this->readyBtn->addTouchEventListener(this, toucheventselector(RoomScene::readyBtnTouchEvent));
	this->readyBtnPanel = static_cast<UIPanel *>(layer->getWidgetByName("ready_button_panel"));

	this->mPlayerInfoArray = new vector<PlayerInfo*>(3);

	this->playerNameLabelArray = new vector<Label*>(3);
	(*this->playerNameLabelArray)[0] = static_cast<Label *>(layer->getWidgetByName("pre_player_name_label"));
	(*this->playerNameLabelArray)[0]->setVisible(false);
	(*this->playerNameLabelArray)[1] = static_cast<Label *>(layer->getWidgetByName("my_name_label"));
	(*this->playerNameLabelArray)[1]->setVisible(false);
	(*this->playerNameLabelArray)[2] = static_cast<Label *>(layer->getWidgetByName("next_player_name_label"));
	(*this->playerNameLabelArray)[2]->setVisible(false);

	mPlayerCoinPanelArray = new vector<UIPanel*>(3);
	(*this->mPlayerCoinPanelArray)[0] = static_cast<UIPanel *>(layer->getWidgetByName("pre_player_coin_panel"));
	(*this->mPlayerCoinPanelArray)[1] = static_cast<UIPanel *>(layer->getWidgetByName("my_coin_panel"));
	(*this->mPlayerCoinPanelArray)[2] = static_cast<UIPanel *>(layer->getWidgetByName("next_player_coin_panel"));
	for (int i = 0; i < 3; i++) {
		if ((*this->mPlayerCoinPanelArray)[i] != NULL) {
			(*this->mPlayerCoinPanelArray)[i]->setVisible(false);
		}
	}

	this->playerCoinLabelArray = new vector<Label*>(3);
	(*this->playerCoinLabelArray)[0] = static_cast<Label *>(layer->getWidgetByName("pre_player_coin_label"));
	(*this->playerCoinLabelArray)[1] = static_cast<Label *>(layer->getWidgetByName("my_coin_label"));
	(*this->playerCoinLabelArray)[2] = static_cast<Label *>(layer->getWidgetByName("next_player_coin_label"));
	for (int i = 0; i < 3; i++) {
		if ((*this->playerCoinLabelArray)[i] != NULL) {
			(*this->playerCoinLabelArray)[i]->setVisible(false);
		}
	}

	this->playerHeadImageViewArray = new vector<ImageView*>(3);
	(*this->playerHeadImageViewArray)[0] = static_cast<ImageView *>(layer->getWidgetByName("pre_player_head_img"));
	(*this->playerHeadImageViewArray)[0]->addTouchEventListener(this, toucheventselector(RoomScene::prePlayerHeadTouchEvent));
	(*this->playerHeadImageViewArray)[0]->setTouchEnabled(true);
	(*this->playerHeadImageViewArray)[0]->setVisible(false);
	(*this->playerHeadImageViewArray)[1] = static_cast<ImageView *>(layer->getWidgetByName("my_head_img"));
	(*this->playerHeadImageViewArray)[1]->addTouchEventListener(this, toucheventselector(RoomScene::myPlayerHeadTouchEvent));
	(*this->playerHeadImageViewArray)[1]->setTouchEnabled(true);
	(*this->playerHeadImageViewArray)[1]->setVisible(false);
	(*this->playerHeadImageViewArray)[2] = static_cast<ImageView *>(layer->getWidgetByName("next_player_head_img"));
	(*this->playerHeadImageViewArray)[2]->addTouchEventListener(this, toucheventselector(RoomScene::nextPlayerHeadTouchEvent));
	(*this->playerHeadImageViewArray)[2]->setTouchEnabled(true);
	(*this->playerHeadImageViewArray)[2]->setVisible(false);


	this->playerRoleImageViewArray = new vector<ImageView*>(3);
	(*this->playerRoleImageViewArray)[0] = static_cast<ImageView *>(layer->getWidgetByName("pre_player_role_img"));
	(*this->playerRoleImageViewArray)[0]->setVisible(false);
	(*this->playerRoleImageViewArray)[1] = static_cast<ImageView *>(layer->getWidgetByName("my_role_img"));
	(*this->playerRoleImageViewArray)[1]->setVisible(false);
	(*this->playerRoleImageViewArray)[2] = static_cast<ImageView *>(layer->getWidgetByName("next_player_role_img"));
	(*this->playerRoleImageViewArray)[2]->setVisible(false);

	this->mPlayerActionImageViewArray = new vector<ImageView*>(3);
	(*this->mPlayerActionImageViewArray)[0] = static_cast<ImageView *>(layer->getWidgetByName("pre_player_action_img"));
	(*this->mPlayerActionImageViewArray)[0]->setVisible(false);
	(*this->mPlayerActionImageViewArray)[1] = static_cast<ImageView *>(layer->getWidgetByName("my_action_img"));
	(*this->mPlayerActionImageViewArray)[1]->setVisible(false);
	(*this->mPlayerActionImageViewArray)[2] = static_cast<ImageView *>(layer->getWidgetByName("next_player_action_img"));
	(*this->mPlayerActionImageViewArray)[2]->setVisible(false);

	this->mPlayerClockImageViewArray = new vector<ImageView*>(3);
	(*this->mPlayerClockImageViewArray)[0] = static_cast<ImageView *>(layer->getWidgetByName("pre_player_clock_img"));
	(*this->mPlayerClockImageViewArray)[0]->setVisible(false);
	(*this->mPlayerClockImageViewArray)[1] = static_cast<ImageView *>(layer->getWidgetByName("my_player_clock_img"));
	(*this->mPlayerClockImageViewArray)[1]->setVisible(false);
	(*this->mPlayerClockImageViewArray)[2] = static_cast<ImageView *>(layer->getWidgetByName("next_player_clock_img"));
	(*this->mPlayerClockImageViewArray)[2]->setVisible(false);

	this->playerCardCounterPanelArray = new vector<UIPanel*>(3);
	(*this->playerCardCounterPanelArray)[0] = static_cast<UIPanel *>(layer->getWidgetByName("pre_player_card_counter_panel"));
	(*this->playerCardCounterPanelArray)[0]->setVisible(false);
	(*this->playerCardCounterPanelArray)[1] = static_cast<UIPanel *>(layer->getWidgetByName("my_card_counter_panel"));
	//(*this->playerCardCounterPanelArray)[1]->setVisible(false);
	(*this->playerCardCounterPanelArray)[2] = static_cast<UIPanel *>(layer->getWidgetByName("next_player_card_counter_panel"));
	(*this->playerCardCounterPanelArray)[2]->setVisible(false);

	this->mPlayerCardCounterLabelArray = new vector<Label*>(3);
	(*this->mPlayerCardCounterLabelArray)[0] = static_cast<Label *>(layer->getWidgetByName("pre_player_card_counter_label"));
	(*this->mPlayerCardCounterLabelArray)[1] = static_cast<Label *>(layer->getWidgetByName("my_card_counter_label"));
	(*this->mPlayerCardCounterLabelArray)[2] = static_cast<Label *>(layer->getWidgetByName("next_player_card_counter_label"));
	for (int i = 0; i < 3; i++) {
		if ((*this->mPlayerCardCounterLabelArray)[i] != NULL) {
			(*this->mPlayerCardCounterLabelArray)[i]->setVisible(false);
		}
	}

	this->mPlayerLaidCardPanelArray = new vector<UIPanel*>(3);
	(*this->mPlayerLaidCardPanelArray)[0] = static_cast<UIPanel *>(layer->getWidgetByName("pre_player_laid_cards_panel"));
	//(*this->mPlayerLaidCardPanelArray)[0]->setVisible(false);
	(*this->mPlayerLaidCardPanelArray)[1] = static_cast<UIPanel *>(layer->getWidgetByName("my_laid_cards_panel"));
	//(*this->mPlayerLaidCardPanelArray)[1]->setVisible(false);
	(*this->mPlayerLaidCardPanelArray)[2] = static_cast<UIPanel *>(layer->getWidgetByName("next_player_laid_cards_panel"));
	//(*this->mPlayerLaidCardPanelArray)[2]->setVisible(false);

	readyBtnPanel = static_cast<UIPanel *>(layer->getWidgetByName("ready_button_panel"));
	bidButtonPanel = static_cast<UIPanel *>(layer->getWidgetByName("bid_button_panel"));
	bidButtonPanel->setVisible(false);
	bidButtonPanel->setTouchEnabled(false);
	((Button*)bidButtonPanel->getChildByName("bid_button"))->addTouchEventListener(this, toucheventselector(RoomScene::bidBtnTouchEvent));
	((Button*)bidButtonPanel->getChildByName("not_bid_button"))->addTouchEventListener(this, toucheventselector(RoomScene::notBidBtnTouchEvent));

	outbidButtonPanel = static_cast<UIPanel *>(layer->getWidgetByName("outbid_button_panel"));
	outbidButtonPanel->setVisible(false);
	outbidButtonPanel->setTouchEnabled(false);
	((Button*)outbidButtonPanel->getChildByName("outbid_button"))->addTouchEventListener(this, toucheventselector(RoomScene::outbidBtnTouchEvent));
	((Button*)outbidButtonPanel->getChildByName("not_outbid_button"))->addTouchEventListener(this, toucheventselector(RoomScene::notOutBidBtnTouchEvent));

	mLayCardButtonPanel = static_cast<UIPanel *>(layer->getWidgetByName("lay_card_button_panel"));
	mLayCardButtonPanel->setVisible(false);
	mLayCardButtonPanel->setTouchEnabled(false);
	((Button*)mLayCardButtonPanel->getChildByName("lay_button"))->addTouchEventListener(this, toucheventselector(RoomScene::layCardBtnTouchEvent));
	((Button*)mLayCardButtonPanel->getChildByName("not_lay_button"))->addTouchEventListener(this, toucheventselector(RoomScene::notLayCardBtnTouchEvent));

	myCoinAmountImgPanel = static_cast<UIPanel *>(layer->getWidgetByName("my_coin_amount_img_panel"));
	
	kittyCardPanel = static_cast<UIPanel *>(layer->getWidgetByName("kitty_cards_panel"));

	mKittyCardArray = new vector<int>(3);
	mHandCardArray = new vector<int>();

	mBasechipLabel = static_cast<Label *>(layer->getWidgetByName("basechip_label"));
	mBasechipLabel->setText("2");
	mScoreLabel = static_cast<Label *>(layer->getWidgetByName("score_label"));
	mMyCoinLabel = static_cast<Label *>(layer->getWidgetByName("my_coin_label"));

	char * jsonData = "{\"command\":6}";
	extension::NetworkCommunication::getInstance()->send(jsonData, strlen(jsonData));

	return true;
}

void RoomScene::dropBtnTouchEvent(CCObject *pSender, TouchEventType type) {
	if (type == TOUCH_EVENT_ENDED) {

		CCPoint position = this->toolbarPanel->getPosition();
		CCSize screenSize = CCDirector::sharedDirector()->getVisibleSize();


		// it is hidden now
		if (position.y == screenSize.height) {
			CCActionInterval* actionBy1 = CCMoveBy::create(0.5, ccp(0, -106));
			this->toolbarPanel->runAction(actionBy1);
			CCActionInterval* actionBy2 = CCMoveBy::create(0.5, ccp(0, 98));
			this->roomInfoPanel->runAction(actionBy2);
		} else {
			CCActionInterval*  actionBy1 = CCMoveBy::create(0.5, ccp(0, 106));
			this->toolbarPanel->runAction(actionBy1);
			CCActionInterval* actionBy2 = CCMoveBy::create(0.5, ccp(0, -98));
			this->roomInfoPanel->runAction(actionBy2);
		}

		CCLog("Jonathan: position is %f, %f", position.x, position.y);
	}
}

void RoomScene::exitBtnTouchEvent(CCObject *pSender, TouchEventType type) {
	if (type == TOUCH_EVENT_ENDED) {
		CCLog("Jonathan. in RoomScene::exitBtnTouchEvent");
		CCDirector::sharedDirector()->popScene();
	}
}

void RoomScene::readyBtnTouchEvent(CCObject *pSender, TouchEventType type) {
	if (type == TOUCH_EVENT_ENDED) {
		CCLog("Jonathan. in RoomScene::readyBtnTouchEvent");
		char * jsonData = "{\"command\":10}";
		extension::NetworkCommunication::getInstance()->send(jsonData, strlen(jsonData));

		this->readyBtnPanel->setVisible(false);
		this->readyBtnPanel->setTouchEnabled(false);
	}
}

const bool comparator(const int & a, const int & b) {
	if (a >= 500 && b < 500) {
		return true;
	} else if (a < 500 && b >= 500) {
		return false;
	} else {
		return a % 100 > b % 100;
	}
}

void RoomScene::onResponse(char * response) {
	CCLog("Jonathan: in RoomScene::onResponse. response = %s", response);

	Document doc;
		
	if (doc.ParseInsitu<0>(response).HasParseError()) {
		CCLog("Error occured when parsing json");
		return;
	}

	int command = doc["command"].GetInt();

	switch (command)
	{
	case COMMAND_ON_PLAYER_JOIN_TABLE:
		{
			const char * id = doc["player_id"].GetString();
			const char * name = doc["player_name"].GetString();
			int coins = doc["player_coins"].GetInt();
			int index = doc["seat_index"].GetInt();
			const char * tableId = doc["table_id"].GetString();

			if (strcmp(id, MyPlayerInfo::getInstance()->getId()) == 0) {
				MyPlayerInfo::getInstance()->setCurrentSeatIndex(index);
				MyPlayerInfo::getInstance()->setCoinAmount(coins);
				MyPlayerInfo::getInstance()->setCurrentRoomId(tableId);

				(*this->mPlayerInfoArray)[1] = MyPlayerInfo::getInstance();
			}

			Value& totalPlayerInfoArray = doc["all_player_info"];
			for (int i = 0; i < totalPlayerInfoArray.Size(); i++) {
				const char * playerId = totalPlayerInfoArray[i]["player_id"].GetString();

				const char * playerName = totalPlayerInfoArray[i]["player_name"].GetString();
				int playerCoins = totalPlayerInfoArray[i]["player_coins"].GetInt();
				int playerIndex = totalPlayerInfoArray[i]["seat_index"].GetInt();

				PlayerInfo * playerInfo = new PlayerInfo();
				playerInfo->setId(playerId);
				playerInfo->setName(playerName);
				playerInfo->setCoinAmount(playerCoins);
				playerInfo->setCurrentSeatIndex(playerIndex);
				playerInfo->setCurrentRoomId(tableId);

				if (MyPlayerInfo::getInstance()->getCurrentSeatIndex() == 0) {
					if (playerIndex == 1) {
						(*this->mPlayerInfoArray)[2] = playerInfo;
					} else if (playerIndex == 2) {
						(*this->mPlayerInfoArray)[0] = playerInfo;
					}
				} else if (MyPlayerInfo::getInstance()->getCurrentSeatIndex() == 1) {
					if (playerIndex == 0) {
						(*this->mPlayerInfoArray)[0] = playerInfo;
					} else if (playerIndex == 2) {
						(*this->mPlayerInfoArray)[2] = playerInfo;
					}
				} else if (MyPlayerInfo::getInstance()->getCurrentSeatIndex() == 2) {
					if (playerIndex == 0) {
						(*this->mPlayerInfoArray)[2] = playerInfo;
					} else if (playerIndex == 1) {
						(*this->mPlayerInfoArray)[0] = playerInfo;
					}
				}
			}

			for (int i = 0; i < 3; i++) {
				PlayerInfo * info = (*(this->mPlayerInfoArray))[i];
			
				if (info != NULL) {
					(*playerHeadImageViewArray)[i]->setVisible(true);
					(*playerNameLabelArray)[i]->setVisible(true);

					if ((*mPlayerCoinPanelArray)[i] != NULL) {
						(*mPlayerCoinPanelArray)[i]->setVisible(true);
					}

					if ((*playerCoinLabelArray)[i] != NULL) {
						(*playerCoinLabelArray)[i]->setVisible(true);
					}
				

					Label * nameLabel = (*this->playerNameLabelArray)[i];
					nameLabel->setText(info->getName());

					if (i == 1) {
						char* coinString = new char[128];
						sprintf(coinString, "%d", info->getCoinAmount());
						mMyCoinLabel->setText(coinString);
					} else {
						Label * coinLabel = (*this->playerCoinLabelArray)[i];
						char* coinString = new char[128];
						sprintf(coinString, "%d", info->getCoinAmount());
						coinLabel->setText(coinString);
					}
				}
			}

			break;
		}
	case COMMAND_ON_PLAYER_LEAVE_TABLE:
		{
			break;
		}
	case COMMAND_ON_PLAYER_READY:
		{
			/*Lenovo S650(ID: 4260904): Sending Command. command = {
			   "command" : 11,
			   "player_id" : "4260904"
			}*/
			const char* playerId = doc["player_id"].GetString();
			for (int i = 0; i < mPlayerInfoArray->size(); i++) {
				if ((*mPlayerInfoArray)[i] != NULL && strcmp(playerId, (*mPlayerInfoArray)[i]->getId()) == 0) {
					(*mPlayerActionImageViewArray)[i]->setVisible(true);
					(*mPlayerActionImageViewArray)[i]->loadTexture("1280_800_wdj/room/player_ready.png");
				}
			}

			break;
		}
	case COMMAND_ON_SERVE_CARDS:
		{
			Value& cardArray = doc["card_array"];

			CCSize screenSize = CCDirector::sharedDirector()->getVisibleSize();

			vector<int> sortedCardsArray;

			for (int i = 0; i < cardArray.Size(); i++) {
				sortedCardsArray.push_back(cardArray[i].GetInt());
				mHandCardArray->push_back(cardArray[i].GetInt());
			}

			sort(sortedCardsArray.begin(), sortedCardsArray.end(), comparator);

			for (int i = 0; i < sortedCardsArray.size(); i++) {
				int v = sortedCardsArray.at(i);

				int suit = (v / 100) - 1;
				int value = v % 100;

				if (suit > 3) {
					value = value + VALUE_POKER_SMALL;
				}

				Card* card = Card::create(suit, value, SIDE_FRONT);
				card->setPosition(ccp(screenSize.width,0));

				int cardXStep = (screenSize.width - card->getSize().width) / (cardArray.Size() - 1);

				CCActionInterval* actionTo = CCMoveTo::create(0.5, ccp(cardXStep * i, 0));
				card->runAction(actionTo);
				this->myCardsPanel->addChild(card);
			}

			this->readyBtnPanel->setEnabled(false);

			Value& kittyCardArray = doc["kitty_card_array"];
			for (int i = 0; i < 3; i++) {
				(*mKittyCardArray)[i] = kittyCardArray[i].GetInt();
				int v = kittyCardArray[i].GetInt();

				int suit = (v / 100) - 1;
				int value = v % 100;

				if (suit > 3) {
					value = value + VALUE_POKER_SMALL;
				}

				SmallCard* card = SmallCard::create(suit, value, SIDE_BACK);
				CCPoint pos = card->getPosition();
				card->setPosition(ccp(40*i, pos.y));
				kittyCardPanel->addChild(card);
			}

			// 隐藏玩家分数，显示剩余牌数量
			for (int i = 0; i < mPlayerInfoArray->size(); i++) {
				if ((*mPlayerCoinPanelArray)[i] != NULL) {
					(*mPlayerCoinPanelArray)[i]->setVisible(false);
				}

				if ((*playerCardCounterPanelArray)[i] != NULL) {
					(*playerCardCounterPanelArray)[i]->setVisible(true);

					(*mPlayerCardCounterLabelArray)[i]->setVisible(true);
					(*mPlayerCardCounterLabelArray)[i]->setText("17");
				}
			}
			break;
		}
	case COMMAND_ON_BID_CHANCE:
		{
			for (int i = 0; i < mPlayerActionImageViewArray->size(); i++) {
				(*mPlayerActionImageViewArray)[i]->setVisible(false);
			}
			

			this->readyBtnPanel->setEnabled(false);
			this->bidButtonPanel->setEnabled(true);
			this->bidButtonPanel->setVisible(true);
			break;
		}
	case COMMAND_REQUEST_TO_BID:
		{
			/*Jonathan: in RoomScene::onResponse. response = {
			   "command" : 14,
			   "current_multiple" : 3,
			   "player_id" : "1573928",
			   "response" : true
			}*/

			const char* playerId = doc["player_id"].GetString();
			bool response = doc["response"].GetBool();

			for (int i = 0; i < mPlayerInfoArray->size(); i++) {
				if (strcmp(playerId, (*mPlayerInfoArray)[i]->getId()) == 0) {
					(*mPlayerActionImageViewArray)[i]->setVisible(true);
					if (response) {
						(*mPlayerActionImageViewArray)[i]->loadTexture("1280_800_wdj/room/player_call_landlord.png");
					} else {
						(*mPlayerActionImageViewArray)[i]->loadTexture("1280_800_wdj/room/player_pass_score.png");
					}
				}
			}

			break;
		}
	case COMMAND_ON_SNATCH_CHANCE:
		{
			/*for (int i = 0; i < mPlayerActionImageViewArray->size(); i++) {
				(*mPlayerActionImageViewArray)[i]->setVisible(false);
			}*/

			this->outbidButtonPanel->setVisible(true);
			this->outbidButtonPanel->setEnabled(true);
			this->mLayCardButtonPanel->setEnabled(false);
			this->bidButtonPanel->setEnabled(false);
			this->readyBtnPanel->setEnabled(false);
			break;
		}
	case COMMAND_REQUEST_TO_SNATCH:
		{
			const char* playerId = doc["player_id"].GetString();
			bool response = doc["response"].GetBool();
			int currentScore = doc["current_multiple"].GetInt();

			// score加倍
			if (response) {
				char * scoreString = new char[8];
				sprintf(scoreString, "%d", currentScore);
				mScoreLabel->setText(scoreString);
				delete(scoreString);
			}

			for (int i = 0; i < mPlayerInfoArray->size(); i++) {
				if (strcmp(playerId, (*mPlayerInfoArray)[i]->getId()) == 0) {
					(*mPlayerActionImageViewArray)[i]->setVisible(true);
					if (response) {
						(*mPlayerActionImageViewArray)[i]->loadTexture("1280_800_wdj/room/player_grab_landlord.png");
					} else {
						(*mPlayerActionImageViewArray)[i]->loadTexture("1280_800_wdj/room/player_pass_call.png");
					}
				}
			}
			break;
		}
	case COMMAND_ON_LANDLORD_SETTLED:
		{
			const char* landlordPlayerId = doc["landlord_player_id"].GetString();

			for (int i = 0; i < 3; i++) {
				if (strcmp((*this->mPlayerInfoArray)[i]->getId(), landlordPlayerId) == 0) {
					(*this->playerRoleImageViewArray)[i]->setVisible(true);
					(*this->playerRoleImageViewArray)[i]->loadTexture("1280_800_wdj/room/landlord.png");
				} else {
					(*this->playerRoleImageViewArray)[i]->setVisible(true);
					(*this->playerRoleImageViewArray)[i]->loadTexture("1280_800_wdj/room/farmer.png");
				}
			}

			//翻开底牌
			CCObject* kittyCard;
			CCARRAY_FOREACH(kittyCardPanel->getChildren(), kittyCard) {
				((SmallCard*)kittyCard)->flip();
			}

			CCSize screenSize = CCDirector::sharedDirector()->getVisibleSize();
			CCArray* children = this->myCardsPanel->getChildren();

			if (strcmp(landlordPlayerId, MyPlayerInfo::getInstance()->getId()) == 0) {
				for (int i = 0; i < mKittyCardArray->size(); i++) {
					mHandCardArray->push_back(((*mKittyCardArray)[i]));
					int v = (*mKittyCardArray)[i];

					int suit = (v / 100) - 1;
					int value = v % 100;

					if (suit > 3) {
						value = value + VALUE_POKER_SMALL;
					}

					Card* card = Card::create(suit, value, SIDE_FRONT);
					children->addObject(card);
				}

				int count = children->count();

				// sort the cards with bubble sort method
				for (int i = 0; i < count; i++) {
					for (int j = 0; j < count - i - 1; j++) {
						Card* card1 = (Card*)children->objectAtIndex(j);
						Card* card2 = (Card*)children->objectAtIndex(j + 1);
						if (card1->getValue() < card2->getValue()) {
							children->replaceObjectAtIndex(j, card2);
							children->replaceObjectAtIndex(j + 1, card1);
						}
					}
				}

				// move cards to a new array

				CCArray* newChildren = CCArray::create();
				for (int i = 0; i < count; i++) {
					newChildren->addObject(children->objectAtIndex(i));
				}

				myCardsPanel->removeAllChildren();

				for (int i = 0; i < count; i++) {
					Card* card = (Card*) newChildren->objectAtIndex(i);
					int cardXStep = (screenSize.width - card->getSize().width) / (count - 1);

					CCActionInterval* actionTo = CCMoveTo::create(0.5, ccp(cardXStep * i, 0));
					card->runAction(actionTo);

					myCardsPanel->addChild(card);
				}

			}
		
			break;
		}
	case COMMAND_ON_LEAD_CARD:
		{
			for (int i = 0; i < mPlayerActionImageViewArray->size(); i++) {
				(*mPlayerActionImageViewArray)[i]->setVisible(false);
			}

			const char * id = doc["player_id"].GetString();
			if (strcmp(id, MyPlayerInfo::getInstance()->getId()) == 0) {
				this->mLayCardButtonPanel->setVisible(true);
				this->mLayCardButtonPanel->setEnabled(true);
				(*mPlayerLaidCardPanelArray)[1]->removeAllChildren();

				// 闹钟显示
				(*mPlayerClockImageViewArray)[1]->setVisible(true);
			}
			break;
		}
	case COMMAND_ON_HAND_OUT_CARDS:
		{
			/*ME302C(ID: 6101336): Sending Command. command = {
			   "card_array" : [ 203, 403 ],
			   "command" : 20,
			   "hand_out_player_id" : "6095912",
			   "hand_out_player_name" : "GT-I9003",
			   "next_to_hand_out_card_player_id" : "6098856",
			   "next_to_hand_out_card_player_name" : "Galaxy Nexus",
			   "response" : true
			}*/
			bool response = doc["response"].GetBool();
			const char * layCardPlayerId = doc["hand_out_player_id"].GetString();
			Value& laidCardArray = doc["card_array"];

			if (response) {
				vector<MiddleCard *> smallCardArray;

				if (laidCardArray.IsArray()) {
					for (int i = 0; i < laidCardArray.Size(); i++) {
						int v = laidCardArray[i].GetInt();
						int suit = (v / 100) - 1;
						int value = v % 100;

						if (suit > 3) {
							value = value + VALUE_POKER_SMALL;
						}

						MiddleCard* middleCard = MiddleCard::create(suit, value, SIDE_FRONT);

						smallCardArray.push_back(middleCard);
					}
				

					for(int i = 0;i < mPlayerInfoArray->size(); i++) {
						if (strcmp(layCardPlayerId, (*mPlayerInfoArray)[i]->getId()) == 0) {
							// 隐藏闹钟
							(*mPlayerClockImageViewArray)[i]->setVisible(false);

							(*mPlayerLaidCardPanelArray)[i]->removeAllChildren();
							for (int j = 0; j < smallCardArray.size(); j++) {
								CCPoint pos = smallCardArray[j]->getPosition();
								(smallCardArray[j])->setPosition(CCPoint(50*j, pos.y));
								(*mPlayerLaidCardPanelArray)[i]->addChild(smallCardArray[j]);
							}

							// 更新剩余牌数信息
							char * leftCardCountString = new char[8];
							int leftCardCount = doc["left_card_count"].GetInt();
							sprintf(leftCardCountString, "%d", leftCardCount);

							if ((*mPlayerCardCounterLabelArray)[i] != NULL) {
								(*mPlayerCardCounterLabelArray)[i]->setVisible(true);
								(*mPlayerCardCounterLabelArray)[i]->setText(leftCardCountString);
							}
							delete(leftCardCountString);
						}
					}
				} else {
					//不出牌的情形
					for(int i = 0;i < mPlayerInfoArray->size(); i++) {
						if (strcmp(layCardPlayerId, (*mPlayerInfoArray)[i]->getId()) == 0) {
							// 隐藏闹钟
							(*mPlayerClockImageViewArray)[i]->setVisible(false);
							(*mPlayerActionImageViewArray)[i]->setVisible(true);
							(*mPlayerActionImageViewArray)[i]->loadTexture("1280_800_wdj/room/player_pass.png");
						}
					}
				}

				const char* nextLayCardPlayerId = doc["next_to_hand_out_card_player_id"].GetString();

				// 下一个出牌玩家的闹钟显示出来
				for (int i = 0; i < mPlayerInfoArray->size(); i++) {
					if (strcmp(nextLayCardPlayerId, (*mPlayerInfoArray)[i]->getId()) == 0) {
						(*mPlayerClockImageViewArray)[i]->setVisible(true);
					}
				}

				if (strcmp(nextLayCardPlayerId, MyPlayerInfo::getInstance()->getId()) == 0) {
					mLayCardButtonPanel->setVisible(true);
					mLayCardButtonPanel->setEnabled(true);
					(*mPlayerLaidCardPanelArray)[1]->removeAllChildren();
				}

				// 更新倍数信息
				int score = doc["current_multiple"].GetInt();
				char * scoreString= new char[8];
				sprintf(scoreString, "%d", score);
				mScoreLabel->setText(scoreString);
				delete(scoreString);
			} else {
				// 打出去的牌收回来
				CCArray* children = this->myCardsPanel->getChildren();
				for (int i = 0; i < laidCardArray.Size(); i++) {
					mHandCardArray->push_back(laidCardArray[i].GetInt());
					int v = laidCardArray[i].GetInt();

					int suit = (v / 100) - 1;
					int value = v % 100;

					if (suit > 3) {
						value = value + VALUE_POKER_SMALL;
					}

					Card* card = Card::create(suit, value, SIDE_FRONT);
					children->addObject(card);
				}

				int count = children->count();

				// sort the cards with bubble sort method
				for (int i = 0; i < count; i++) {
					for (int j = 0; j < count - i - 1; j++) {
						Card* card1 = (Card*)children->objectAtIndex(j);
						Card* card2 = (Card*)children->objectAtIndex(j + 1);
						if (card1->getValue() < card2->getValue()) {
							children->replaceObjectAtIndex(j, card2);
							children->replaceObjectAtIndex(j + 1, card1);
						}
					}
				}

				// move cards to a new array
				CCArray* newChildren = CCArray::create();
				for (int i = 0; i < count; i++) {
					newChildren->addObject(children->objectAtIndex(i));
				}

				myCardsPanel->removeAllChildren();
				CCSize screenSize = CCDirector::sharedDirector()->getVisibleSize();
				for (int i = 0; i < count; i++) {
					Card* card = (Card*) newChildren->objectAtIndex(i);
					int cardXStep = (screenSize.width - card->getSize().width) / (count - 1);
					CCActionInterval* actionTo = CCMoveTo::create(0.5, ccp(cardXStep * i, 0));
					card->runAction(actionTo);
					myCardsPanel->addChild(card);
				}

				mLayCardButtonPanel->setVisible(true);
				mLayCardButtonPanel->setEnabled(true);
				(*mPlayerLaidCardPanelArray)[1]->removeAllChildren();
			}

			break;
		}
	case COMMAND_ON_ROUND_END:
		{
			/*Galaxy Nexus(ID: 4004184): Sending Command. command = {
			   "command" : 21,
			   "multiple" : 3,
			   "player_info_array" : [
				  {
					 "card_array" : [
						209,
						412,
						108,
						307,
						414,
						112,
						406,
						314,
						405,
						105,
						207,
						210,
						410,
						312,
						115,
						313,
						411,
						107,
						212
					 ],
					 "coins" : 994,
					 "player_id" : "3998760"
				  },
				  {
					 "card_array" : null,
					 "coins" : 1003,
					 "player_id" : "4001704"
				  },
				  {
					 "card_array" : [
						315,
						311,
						310,
						308,
						409,
						403,
						213,
						214,
						215,
						309,
						306,
						111,
						110,
						206,
						407,
						501,
						104
					 ],
					 "coins" : 1003,
					 "player_id" : "4004184"
				  }
			   ],
			   "winner_player_id_array" : [ "4001704", "4004184" ]
			}*/
			Value& playerInfoArray = doc["player_info_array"];

			if (playerInfoArray.IsArray()) {
				for (int i = 0; i < playerInfoArray.Size(); i++) {
					const char * id = playerInfoArray[i]["player_id"].GetString();
					int coins = playerInfoArray[i]["coins"].GetInt();
					char * coinString = new char[32];
					sprintf(coinString, "%d", coins);

					for (int j = 0; j < mPlayerInfoArray->size(); i++) {
						if (strcmp(id, (*mPlayerInfoArray)[j]->getId()) == 0) {
							(*mPlayerInfoArray)[j]->setCoinAmount(coins);
							
							if ((*playerCoinLabelArray)[j] != NULL) {
								(*playerCoinLabelArray)[j]->setText(coinString);
							} else {
								mMyCoinLabel->setText(coinString);
							}
						}
					}
				}
			}



			Value& winnerPlayerIdArray = doc["winner_player_id_array"];

			if (winnerPlayerIdArray.IsArray()) {

				bool winnerFound = false;
				for (int i = 0; i < winnerPlayerIdArray.Size(); i++) {
					const char * winnerId = winnerPlayerIdArray[i].GetString();

					if (strcmp(winnerId, MyPlayerInfo::getInstance()->getId()) == 0) {
						winnerFound = true;

						CCArmatureDataManager::sharedArmatureDataManager()->addArmatureFileInfo("RoundEndWinAnimation/RoundEndWinAnimation0.png", "RoundEndWinAnimation/RoundEndWinAnimation0.plist", "RoundEndWinAnimation/RoundEndWinAnimation.ExportJson");
						CCArmatureDataManager::sharedArmatureDataManager()->addArmatureFileInfo("RoundEndWinAnimation/RoundEndWinAnimation1.png", "RoundEndWinAnimation/RoundEndWinAnimation1.plist", "RoundEndWinAnimation/RoundEndWinAnimation.ExportJson");
						CCArmatureDataManager::sharedArmatureDataManager()->addArmatureFileInfo("RoundEndWinAnimation/RoundEndWinAnimation2.png", "RoundEndWinAnimation/RoundEndWinAnimation2.plist", "RoundEndWinAnimation/RoundEndWinAnimation.ExportJson");
						CCArmatureDataManager::sharedArmatureDataManager()->addArmatureFileInfo("RoundEndWinAnimation/RoundEndWinAnimation3.png", "RoundEndWinAnimation/RoundEndWinAnimation3.plist", "RoundEndWinAnimation/RoundEndWinAnimation.ExportJson");
						CCArmatureDataManager::sharedArmatureDataManager()->addArmatureFileInfo("RoundEndWinAnimation/RoundEndWinAnimation4.png", "RoundEndWinAnimation/RoundEndWinAnimation4.plist", "RoundEndWinAnimation/RoundEndWinAnimation.ExportJson");
						CCArmatureDataManager::sharedArmatureDataManager()->addArmatureFileInfo("RoundEndWinAnimation/RoundEndWinAnimation5.png", "RoundEndWinAnimation/RoundEndWinAnimation5.plist", "RoundEndWinAnimation/RoundEndWinAnimation.ExportJson");

						CCArmature* armature = CCArmature::create("RoundEndWinAnimation");

						armature->getAnimation()->play("Win");
						
						CCSize screenSize = CCDirector::sharedDirector()->getVisibleSize();
						armature->setPosition(screenSize.width / 2, screenSize.height / 2);

						this->addChild(armature);
					}
				}

				if (winnerFound == false) {
					CCArmatureDataManager::sharedArmatureDataManager()->addArmatureFileInfo("RoundEndLoseAnimation/RoundEndLoseAnimation0.png", "RoundEndLoseAnimation/RoundEndLoseAnimation0.plist", "RoundEndLoseAnimation/RoundEndLoseAnimation.ExportJson");
					CCArmatureDataManager::sharedArmatureDataManager()->addArmatureFileInfo("RoundEndLoseAnimation/RoundEndLoseAnimation1.png", "RoundEndLoseAnimation/RoundEndLoseAnimation1.plist", "RoundEndLoseAnimation/RoundEndLoseAnimation.ExportJson");
					CCArmatureDataManager::sharedArmatureDataManager()->addArmatureFileInfo("RoundEndLoseAnimation/RoundEndLoseAnimation2.png", "RoundEndLoseAnimation/RoundEndLoseAnimation2.plist", "RoundEndLoseAnimation/RoundEndLoseAnimation.ExportJson");
					CCArmatureDataManager::sharedArmatureDataManager()->addArmatureFileInfo("RoundEndLoseAnimation/RoundEndLoseAnimation3.png", "RoundEndLoseAnimation/RoundEndLoseAnimation3.plist", "RoundEndLoseAnimation/RoundEndLoseAnimation.ExportJson");
					CCArmatureDataManager::sharedArmatureDataManager()->addArmatureFileInfo("RoundEndLoseAnimation/RoundEndLoseAnimation4.png", "RoundEndLoseAnimation/RoundEndLoseAnimation4.plist", "RoundEndLoseAnimation/RoundEndLoseAnimation.ExportJson");
					CCArmatureDataManager::sharedArmatureDataManager()->addArmatureFileInfo("RoundEndLoseAnimation/RoundEndLoseAnimation5.png", "RoundEndLoseAnimation/RoundEndLoseAnimation5.plist", "RoundEndLoseAnimation/RoundEndLoseAnimation.ExportJson");
					CCArmatureDataManager::sharedArmatureDataManager()->addArmatureFileInfo("RoundEndLoseAnimation/RoundEndLoseAnimation6.png", "RoundEndLoseAnimation/RoundEndLoseAnimation6.plist", "RoundEndLoseAnimation/RoundEndLoseAnimation.ExportJson");
					CCArmatureDataManager::sharedArmatureDataManager()->addArmatureFileInfo("RoundEndLoseAnimation/RoundEndLoseAnimation7.png", "RoundEndLoseAnimation/RoundEndLoseAnimation7.plist", "RoundEndLoseAnimation/RoundEndLoseAnimation.ExportJson");

					CCArmature* armature = CCArmature::create("RoundEndLoseAnimation");

					armature->getAnimation()->play("Lose");
						
					CCSize screenSize = CCDirector::sharedDirector()->getVisibleSize();
					armature->setPosition(screenSize.width / 2, screenSize.height / 2);

					this->addChild(armature);
				}
			}

			mLayCardButtonPanel->setVisible(true);
			mLayCardButtonPanel->setEnabled(true);

			break;
		}
	}
}

void RoomScene::bidBtnTouchEvent(CCObject* pSender, TouchEventType type) {
	if (type == TOUCH_EVENT_ENDED) {
		CCLog("Jonathan: in bidBtnTouchEvent");

		Document doc;
		doc.SetObject();
		doc.AddMember("command", COMMAND_REQUEST_TO_BID, doc.GetAllocator());
		doc.AddMember("player_id", MyPlayerInfo::getInstance()->getId(), doc.GetAllocator());
		doc.AddMember("response", true, doc.GetAllocator());

		

		StringBuffer stringBuffer;
		Writer<StringBuffer> writer(stringBuffer);
		doc.Accept(writer);
		char * jsonData = new char[128];
		sprintf(jsonData, "%s", stringBuffer.GetString());
		int size = strlen(jsonData);
		extension::NetworkCommunication::getInstance()->send(jsonData, size);

		this->bidButtonPanel->setEnabled(false);
	}
}

void RoomScene::notBidBtnTouchEvent(CCObject* pSender, TouchEventType type) {
	if (type == TOUCH_EVENT_ENDED) {
	}
}

void RoomScene::outbidBtnTouchEvent(CCObject* pSender, TouchEventType type) {
	if (type == TOUCH_EVENT_ENDED) {
		Document doc;
		doc.SetObject();
		doc.AddMember("command", COMMAND_REQUEST_TO_SNATCH, doc.GetAllocator());
		doc.AddMember("player_id", MyPlayerInfo::getInstance()->getId(), doc.GetAllocator());
		doc.AddMember("response", true, doc.GetAllocator());

		StringBuffer stringBuffer;
		Writer<StringBuffer> writer(stringBuffer);
		doc.Accept(writer);
		char * jsonData = new char[128];
		sprintf(jsonData, "%s", stringBuffer.GetString());
		int size = strlen(jsonData);
		extension::NetworkCommunication::getInstance()->send(jsonData, size);

		this->outbidButtonPanel->setEnabled(false);
	}
}

void RoomScene::notOutBidBtnTouchEvent(CCObject* pSender, TouchEventType type) {
	if (type == TOUCH_EVENT_ENDED) {
		
	}
}

void RoomScene::layCardBtnTouchEvent(CCObject* pSender, TouchEventType type) {
	if (type == TOUCH_EVENT_ENDED) {
		CCArray* cardsArray = this->myCardsPanel->getChildren();
		CCObject* card;
		vector<int> poppedUpCardValueArray;

		CCArray* poppedUpCardArray = CCArray::create();

		CCARRAY_FOREACH(cardsArray, card) {
			if (((Card*)card)->isPoppedUp()) {

				poppedUpCardArray->addObject(card);

				int suit = ((Card*)card)->getSuit();
				int value = ((Card*)card)->getValue();

				int convertedValue;
				if (value == VALUE_POKER_SMALL) {
					convertedValue = 500;
				} else if (value == VALUE_POKER_BIG) {
					convertedValue = 501;
				} else {
					convertedValue = (suit + 1) * 100 + value;
				}

				poppedUpCardValueArray.push_back(convertedValue);
			}
		}

		// remove cards
		CCARRAY_FOREACH(poppedUpCardArray, card) {
			this->myCardsPanel->removeChild((Card *)card, true);
		}

		// reset card's position
		cardsArray = this->myCardsPanel->getChildren();
		CCSize screenSize = CCDirector::sharedDirector()->getVisibleSize();
		
		int i = 0;
		CCARRAY_FOREACH(cardsArray, card) {
			int cardXStep;

			if (cardsArray->count() == 1) {
				cardXStep = 50;
			}else {
				cardXStep = (screenSize.width - ((Card*)card)->getSize().width) / (cardsArray->count() - 1);
			}

			CCActionInterval* actionTo = CCMoveTo::create(0.5, ccp(cardXStep * i++, 0));
			((Card*)card)->runAction(actionTo);
		}
		

		Document doc;
		doc.SetObject();

		doc.AddMember("command", COMMAND_REQUEST_TO_HAND_OUT_CARDS, doc.GetAllocator());

		Value cardValueJsonArray;
		cardValueJsonArray.SetArray();
		for (int i = 0; i < poppedUpCardValueArray.size(); i++) {
			cardValueJsonArray.PushBack(poppedUpCardValueArray[i], doc.GetAllocator());
		}

		doc.AddMember("card_array", cardValueJsonArray, doc.GetAllocator());

		StringBuffer stringBuffer;
		Writer<StringBuffer> writer(stringBuffer);
		doc.Accept(writer);
		char * jsonData = new char[128];
		sprintf(jsonData, "%s", stringBuffer.GetString());
		int size = strlen(jsonData);
		extension::NetworkCommunication::getInstance()->send(jsonData, size);

		this->mLayCardButtonPanel->setEnabled(false);
	}
}

void RoomScene::notLayCardBtnTouchEvent(CCObject* pSender, TouchEventType type) {
	if (type == TOUCH_EVENT_ENDED) {
		Document doc;
		doc.SetObject();

		doc.AddMember("command", COMMAND_REQUEST_TO_HAND_OUT_CARDS, doc.GetAllocator());
		Value cardValueJsonArray;
		cardValueJsonArray.SetArray();
		doc.AddMember("card_array", cardValueJsonArray, doc.GetAllocator());

		StringBuffer stringBuffer;
		Writer<StringBuffer> writer(stringBuffer);
		doc.Accept(writer);
		char * jsonData = new char[128];
		sprintf(jsonData, "%s", stringBuffer.GetString());
		int size = strlen(jsonData);
		extension::NetworkCommunication::getInstance()->send(jsonData, size);

		this->mLayCardButtonPanel->setEnabled(false);

		(*mPlayerActionImageViewArray)[1]->setVisible(true);
		(*mPlayerActionImageViewArray)[1]->loadTexture("1280_800_wdj/room/player_pass.png");
	}
}

void RoomScene::prePlayerHeadTouchEvent(CCObject* pSender, TouchEventType type) {
	if (type == TOUCH_EVENT_ENDED) {
		CCLog("Jonathan: in RoomScene::prePlayerHeadTouchEvent");

		if ((*this->mPlayerInfoArray)[0] == NULL) {
			return;
		}

		PlayerInfoScene* s = PlayerInfoScene::create();
		if (s)
		{
			s->setPlayerInfo((*this->mPlayerInfoArray)[0]);
			this->addChild(s);
		}
	}
}

void RoomScene::myPlayerHeadTouchEvent(CCObject* pSender, TouchEventType type) {
	if (type == TOUCH_EVENT_ENDED) {
		CCLog("Jonathan: in RoomScene::myPlayerHeadTouchEvent");
		PlayerInfoScene* s = PlayerInfoScene::create();
		if (s)
		{
			s->setPlayerInfo((*this->mPlayerInfoArray)[1]);
			this->addChild(s);
		}
	}
}

void RoomScene::nextPlayerHeadTouchEvent(CCObject* pSender, TouchEventType type) {
	if (type == TOUCH_EVENT_ENDED) {
		CCLog("Jonathan: in RoomScene::nextPlayerHeadTouchEvent");

		if ((*this->mPlayerInfoArray)[2] == NULL) {
			return;
		}

		PlayerInfoScene* s = PlayerInfoScene::create();
		if (s)
		{
			s->setPlayerInfo((*this->mPlayerInfoArray)[2]);
			this->addChild(s);
		}
	}
}