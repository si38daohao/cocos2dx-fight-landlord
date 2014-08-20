#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include "cocos-ext.h"

USING_NS_CC;
USING_NS_CC_EXT;
using namespace ui;


class HelloWorld : public cocos2d::CCLayer
{
public:
    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();  

    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::CCScene* scene();
    
    // a selector callback
    void menuCloseCallback(CCObject* pSender);
	void menuConnectCallback(CCObject* pSender);
	void menuRenameCallback(CCObject * pSender);
	void menuGetInfoCallback(CCObject * pSender);
	void onHttpRequestCompleted(CCNode* node, void *data);
	void onResponse(char * response);
	void onConnectResult(int resultCode, char * desc);
	void settingTouchEvent(CCObject *pSender, TouchEventType type);
	void firstChargeTouchEvent(CCObject *pSender, TouchEventType type);
	void chargeTouchEvent(CCObject *pSender, TouchEventType type);
    
    // implement the "static node()" method manually
    CREATE_FUNC(HelloWorld);

private:
	Label * userNameLabel;
	Label * userCoinAmountLabel;
};

#endif // __HELLOWORLD_SCENE_H__
