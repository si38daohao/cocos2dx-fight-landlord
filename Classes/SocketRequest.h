#pragma once

#include "cocos2d.h"
#include "ExtensionMacros.h"

NS_CC_EXT_BEGIN

class SocketRequest : public CCObject
{
public:
	char * pBuffer;
	int size;
public:
	SocketRequest(char * buffer, int size);
	SocketRequest(void);
	~SocketRequest(void);
};

NS_CC_EXT_END