#pragma once


#include "cocos2d.h"
#include "ExtensionMacros.h"

NS_CC_EXT_BEGIN

class SocketRequest : public CCObject
{
public:
	const char * pBuffer;
	int size;
public:
	SocketRequest(const char * buffer, int size);
	SocketRequest(void);
	~SocketRequest(void);
};

NS_CC_EXT_END