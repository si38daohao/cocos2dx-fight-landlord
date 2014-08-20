#include "HelloWorldScene.h"
#include "NetworkCommunication.h"
#include "CocoStudio\Json\rapidjson\writer.h"
#include "cocos-ext.h"
#include "RoomScene.h"
#include "MyPlayerInfo.h"

USING_NS_CC;

USING_NS_CC_EXT;

using namespace ui;
using namespace extension;
using namespace rapidjson;

CCScene* HelloWorld::scene()
{
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();

    HelloWorld *layer = HelloWorld::create();
    scene->addChild(layer);
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !CCLayer::init() )
    {
        return false;
    }

	extension::NetworkCommunication::getInstance()->setConnectResultCallback(this, connectresult_selector(HelloWorld::onConnectResult));
	extension::NetworkCommunication::getInstance()->setResponseCallback(this, httpresponse2_selector(HelloWorld::onResponse));


	UILayer * layer = UILayer::create();
	UILayout * layout = static_cast<Layout *>(GUIReader::shareReader()->widgetFromJsonFile("login_layout.json"));
	layer->addWidget(layout);
	this->addChild(layer);
	
	UIButton * settingButton = static_cast<UIButton *>(layer->getWidgetByName("setting_btn"));
	settingButton->addTouchEventListener(this, toucheventselector(HelloWorld::settingTouchEvent));

	UIButton * firstChargebutton = static_cast<UIButton *>(layer->getWidgetByName("first_charge_btn"));
	firstChargebutton->addTouchEventListener(this, toucheventselector(HelloWorld::firstChargeTouchEvent));

	ui::UIListView * listview = ui::UIListView::create();
	listview->setDirection(ui::SCROLLVIEW_DIR_HORIZONTAL);
	listview->setAnchorPoint(ccp(0, 0));
	listview->setPosition(ccp(0, 170));
	listview->setSize(cocos2d::CCSize(1100, 450));

	UILayout * itemLayout = static_cast<Layout *>(GUIReader::shareReader()->widgetFromJsonFile("hall_item_layout_1.json"));
	listview->setItemModel(itemLayout);

	// laizi item
	listview->pushBackDefaultItem();

	// sanren item
	UILayout * sanrenItemLayout = static_cast<Layout *>(GUIReader::shareReader()->widgetFromJsonFile("hall_item_layout_1.json"));
	((UIImageView *)(sanrenItemLayout->getChildByName("figure_image_view")))->loadTexture("1280_800_wdj/login/item_three.png");
	((UIImageView *)(sanrenItemLayout->getChildByName("hall_name_image_view")))->loadTexture("1280_800_wdj/login/item_words_sanren.png");
	listview->pushBackCustomItem(sanrenItemLayout);

	// match item
	UILayout * matchItemLayout = (UILayout *)sanrenItemLayout->clone();
	((UIImageView *)(matchItemLayout->getChildByName("figure_image_view")))->loadTexture("1280_800_wdj/login/item_match.png");
	((UIImageView *)(matchItemLayout->getChildByName("hall_name_image_view")))->loadTexture("1280_800_wdj/login/item_words_match.png");
	listview->pushBackCustomItem(matchItemLayout);

	// more-play item
	UILayout * morePlayItemLayout = (UILayout *)sanrenItemLayout->clone();
	((UIImageView *)(morePlayItemLayout->getChildByName("figure_image_view")))->loadTexture("1280_800_wdj/login/item_more.png");
	((UIImageView *)(morePlayItemLayout->getChildByName("hall_name_image_view")))->loadTexture("1280_800_wdj/login/item_words_more_play.png");
	listview->pushBackCustomItem(morePlayItemLayout);


	((UIPanel *)layout->getChildByName("hall_listview_container"))->addChild(listview);

	// get all widgets needed when updating ui
	userNameLabel = static_cast<Label *>(layer->getWidgetByName("nick_name_label"));
	userCoinAmountLabel = static_cast<Label *>(layer->getWidgetByName("coin_amount_label"));
	static_cast<Label *>(layer->getWidgetByName("add_coin_btn"))->addTouchEventListener(this, toucheventselector(HelloWorld::chargeTouchEvent));

    return true;
}

void HelloWorld::onResponse(char * response) {
	CCLog("Jonathan: in HelloWorld::onResponse. response = %s", response);
	
	if (response != NULL) {
		Document doc;
		
		if (doc.ParseInsitu<0>(response).HasParseError()) {
			CCLog("Error occured when parsing json");
			return;
		}

		int command = doc["command"].GetInt();
		int coinAmount;
		const char * id;
		const char * name;
		char * coinAmountString;

		switch (command)
		{
		case 5:
			coinAmount = doc["player_coins"].GetInt();
			id = doc["player_id"].GetString();
			name = doc["player_name"].GetString();

			MyPlayerInfo::getInstance()->setCoinAmount(coinAmount);
			MyPlayerInfo::getInstance()->setId(id);
			MyPlayerInfo::getInstance()->setName(name);

			userNameLabel->setText(name);
			coinAmountString = new char[128];
			sprintf(coinAmountString, "%d", coinAmount); 
			userCoinAmountLabel->setText(coinAmountString);
			delete(coinAmountString);
			break;
		case 7:
			CCScene* s = RoomScene::scene();
			CCScene* pScene = CCTransitionMoveInR::create(1, s);
			if (pScene)
			{
				CCDirector::sharedDirector()->pushScene(pScene);
			}
			break;
		}
	}
}

void HelloWorld::onConnectResult(int resultCode, char * desc) {
	CCLog("Jonathan: in onConnectResult. resultCode = %d", resultCode);
	char * jsonData = "{\"command\":4}";
	extension::NetworkCommunication::getInstance()->send(jsonData, strlen(jsonData));
}

void HelloWorld::settingTouchEvent(CCObject *pSender, TouchEventType type) {

	if (type == TOUCH_EVENT_ENDED) {
		CCLog("Jonathan. in HelloWorld::settingTouchEvent");

		/*char * jsonData = "{\"command\":6}";
		extension::NetworkCommunication::getInstance()->send(jsonData, strlen(jsonData));*/
		CCScene* s = RoomScene::scene();
		/*CCScene* pScene = CCTransitionMoveInR::create(1, s);*/
		if (s)
		{
			CCDirector::sharedDirector()->pushScene(s);
		}
	}
}

void HelloWorld::firstChargeTouchEvent(CCObject *pSender, TouchEventType type) {

	if (type == TOUCH_EVENT_ENDED) {
		CCLog("Jonathan. in HelloWorld::firstChargeTouchEvent");
		extension::NetworkCommunication::getInstance()->connect("172.20.133.41", 4000);
	}
}

void HelloWorld::chargeTouchEvent(CCObject *pSender, TouchEventType type) {
	if (type == TOUCH_EVENT_ENDED) {
		CCLog("Jonathan. in HelloWorld::chargeTouchEvent");
		MyPlayerInfo::getInstance()->setCoinAmount(MyPlayerInfo::getInstance()->getCoinAmount() + 10);
		char * coinAmountString = new char[128];
		sprintf(coinAmountString, "%d", MyPlayerInfo::getInstance()->getCoinAmount());
		userCoinAmountLabel->setText(coinAmountString);
	}
}

