#pragma once

#include "cocos2d.h"
#include "ExtensionMacros.h"

NS_CC_EXT_BEGIN

class SocketResponse : public CCObject
{
public:
	char * buffer;
	int size;
	SocketResponse(char* buffer, int size);
	SocketResponse(void);
	~SocketResponse(void);
};

NS_CC_EXT_END