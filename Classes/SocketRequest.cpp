#include "SocketRequest.h"

NS_CC_EXT_BEGIN

SocketRequest::SocketRequest(char * buffer, int size)
{
	this->pBuffer = buffer;
	this->size = size;
}


SocketRequest::~SocketRequest(void)
{
}

NS_CC_EXT_END