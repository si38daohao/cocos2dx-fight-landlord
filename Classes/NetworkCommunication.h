#pragma once
#include <pthread.h>
#include "cocoa\CCArray.h"
#include "ExtensionMacros.h"
#include "ODSocket.h"

NS_CC_EXT_BEGIN

typedef void (CCObject::*SEL_HttpResponse_Jonathan)(char * response);
#define httpresponse2_selector(_SELECTOR) (cocos2d::extension::SEL_HttpResponse_Jonathan)(&_SELECTOR)

class NetworkCommunication : public CCObject
{
public:
	static NetworkCommunication * getInstance();
	void connect(char * ip, int port);
	void disconnect();
	void addRequest(char * request);
	void dispatchResponseCallbacks(float delta);
	void setResponseCallback(CCObject* pTarget, SEL_HttpResponse_Jonathan pSelector);
private:
	NetworkCommunication(void);
	~NetworkCommunication(void);
};

NS_CC_EXT_END

