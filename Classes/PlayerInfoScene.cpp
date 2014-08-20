#include "PlayerInfoScene.h"


USING_NS_CC;
USING_NS_CC_EXT;

CCScene * PlayerInfoScene::scene() {
	CCScene *scene = CCScene::create();

	PlayerInfoScene* layer = PlayerInfoScene::create();

	scene->addChild(layer);

	return scene;
}

bool PlayerInfoScene::init() {
	if (!CCLayer::init()) {
		return false;
	}

	UILayer* layer = UILayer::create();
	UILayout* layout = static_cast<Layout *>(GUIReader::shareReader()->widgetFromJsonFile("player_info_dialog_layout.json"));
	layer->addWidget(layout);
	this->addChild(layer);

	((Button*)(layer->getWidgetByName("close_btn")))->addTouchEventListener(this, toucheventselector(PlayerInfoScene::closeBtnTouchEvent));

	mNameLabel = (Label *)(layer->getWidgetByName("name_label"));
	mCoinLabel = (Label *)(layer->getWidgetByName("coin_label"));

	return true;
}

void PlayerInfoScene::closeBtnTouchEvent(CCObject *pSender, TouchEventType type) {
	if (type == TOUCH_EVENT_ENDED) {
		this->removeFromParentAndCleanup(true);
	}
}

void PlayerInfoScene::setPlayerInfo(PlayerInfo* playerInfo) {
	mNameLabel->setText(playerInfo->getName());

	char * coinString = new char[16];
	sprintf(coinString, "%d", playerInfo->getCoinAmount());
	mCoinLabel->setText(coinString);
}

