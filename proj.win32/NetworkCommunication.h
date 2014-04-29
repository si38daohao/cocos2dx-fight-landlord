#pragma once
#include <pthread\pthread.h>
#include "cocoa\CCArray.h"
#include "ExtensionMacros.h"

NS_CC_EXT_BEGIN

class NetworkCommunication
{
public:
	static NetworkCommunication * getInstance();
	void connect(char * ip, int port);
	void disconnect();
	void isConnected();
private:
	NetworkCommunication(void);
	~NetworkCommunication(void);
};

NS_CC_EXT_END

