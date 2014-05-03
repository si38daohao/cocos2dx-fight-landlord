#include "SocketResponse.h"

NS_CC_EXT_BEGIN

SocketResponse::SocketResponse(char* buffer, int size) {
	this->buffer = buffer;
	this->size = size;
}

SocketResponse::SocketResponse(void)
{
}


SocketResponse::~SocketResponse(void)
{
}

NS_CC_EXT_END