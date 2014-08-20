#pragma once
#include <pthread.h>
#include "cocoa\CCArray.h"
#include "ExtensionMacros.h"
#include "ODSocket.h"

NS_CC_EXT_BEGIN

typedef void (CCObject::*SEL_SocketResponse)(char * response);
#define httpresponse2_selector(_SELECTOR) (cocos2d::extension::SEL_SocketResponse)(&_SELECTOR)

typedef void (CCObject::*SEL_ConnectResult)(int resultCode, char * desc);
#define connectresult_selector(_SELECTOR) (cocos2d::extension::SEL_ConnectResult)(&_SELECTOR)

class NetworkCommunication : public CCObject
{
public:
	static NetworkCommunication * getInstance();
	void connect(char * ip, int port);
	void disconnect();
	void addRequest(char * request);
	void dispatchResponseCallbacks(float delta);
	void setResponseCallback(CCObject* pTarget, SEL_SocketResponse pSelector);
	void setConnectResultCallback(CCObject * pTarget, SEL_ConnectResult pSelector);

	void send(const char* buffer, int size);
private:
	NetworkCommunication(void);
	~NetworkCommunication(void);
};

NS_CC_EXT_END

