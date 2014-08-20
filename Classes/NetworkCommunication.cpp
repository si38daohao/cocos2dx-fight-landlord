#include "NetworkCommunication.h"
#include "cocos2d.h"
#include "SocketRequest.h"
#include "SocketResponse.h"

#include <pthread.h>

NS_CC_EXT_BEGIN

static NetworkCommunication * s_instance;

static CCArray * s_requestQueue;
static CCArray * s_responseQueue;

static char * response = NULL;

static pthread_t s_networkReadThreadId;
static pthread_t s_networkWriteThreadId;
static pthread_t s_connectThreadId;

static pthread_mutex_t s_requestQueueMutex;
static pthread_mutex_t s_responseQueueMutex;

static pthread_mutex_t s_readThreadSleepMutex;
static pthread_cond_t s_readThreadSleepCondition;

static pthread_mutex_t s_writeThreadSleepMutex;
static pthread_cond_t s_writeThreadSleepCondition;

static ODSocket * odsocket;
	
static CCObject * target = NULL;
static SEL_SocketResponse selector = NULL;

static SEL_ConnectResult s_connectResultSelector = NULL;
static CCObject * connectResultTarget = NULL;

static void* networkReadThreadWork(void *data) {

	CCLog("Jonathan: in networkReadThreadWork");

	char recvBuffer[1024];
	while (true) {
		CCLog("Jonathan: in while loop");
		int ret = odsocket->Recv(recvBuffer, 1024);

		if (ret > 0) {
			recvBuffer[ret] = '\0';

			pthread_mutex_lock(&s_responseQueueMutex);
			char * tmp = new char[1024];
			strcpy(tmp, recvBuffer);
			
			SocketResponse* socketResponse = new SocketResponse(tmp, ret);
			s_responseQueue->addObject(socketResponse);

			pthread_mutex_unlock(&s_responseQueueMutex);
		} else {
			pthread_cond_wait(&s_readThreadSleepCondition, &s_readThreadSleepMutex);
		}
	}

	return NULL;
}

static void* networkWriteThreadWork(void *data) {
	CCLog("Jonathan: in networkWriteThreadWork");

	SocketRequest* request = NULL;
	while (true) {

		pthread_mutex_lock(&s_requestQueueMutex);
		if (0 != s_requestQueue->count()) {
			request = dynamic_cast<SocketRequest*> (s_requestQueue->objectAtIndex(0));
			s_requestQueue->removeObjectAtIndex(0);
		}
		pthread_mutex_unlock(&s_requestQueueMutex);

		if (NULL == request) {
			pthread_cond_wait(&s_writeThreadSleepCondition, &s_writeThreadSleepMutex);
			continue;
		}

		odsocket->Send(request->pBuffer, request->size);
		request = NULL;
		pthread_cond_signal(&s_readThreadSleepCondition);
	}

	return NULL;
}

struct IP_PORT_INFO {
	char * ip;
	int port;
};

static void * connectThreadWork(void * data) {

	IP_PORT_INFO * pIPPortInfo = (IP_PORT_INFO *)data;
	bool ret = false;

	CCLog("Jonathan: before connect");
	char * content = "{\"command\":6, \"player_id\":\"12345678\"}";
	ret = odsocket->Connect(pIPPortInfo->ip, pIPPortInfo->port);
	CCLog("Jonathan: after connect");

	if (connectResultTarget != NULL && s_connectResultSelector != NULL) {
		(connectResultTarget->*s_connectResultSelector)(ret ? 1 : 0, NULL);
	}

	return NULL;
}

NetworkCommunication::NetworkCommunication(void)
{
	s_requestQueue = new CCArray();
	s_requestQueue->init();

	s_responseQueue = new CCArray();
	s_responseQueue->init();

	pthread_mutex_init(&s_requestQueueMutex, NULL);
	pthread_mutex_init(&s_responseQueueMutex, NULL);

	pthread_mutex_init(&s_readThreadSleepMutex, NULL);
	pthread_cond_init(&s_readThreadSleepCondition, NULL);

	pthread_mutex_init(&s_writeThreadSleepMutex, NULL);
	pthread_cond_init(&s_writeThreadSleepCondition, NULL);

	pthread_create(&s_networkReadThreadId, NULL, networkReadThreadWork, NULL);
	pthread_detach(s_networkReadThreadId);

	pthread_create(&s_networkWriteThreadId, NULL, networkWriteThreadWork, NULL);
	pthread_detach(s_networkWriteThreadId);

	odsocket = new ODSocket();
	odsocket->Init();
	odsocket->Create(AF_INET,SOCK_STREAM,0);

	CCDirector::sharedDirector()->getScheduler()->scheduleSelector(
		schedule_selector(NetworkCommunication::dispatchResponseCallbacks), this, 0, false);
    //CCDirector::sharedDirector()->getScheduler()->pauseTarget(this);
}


NetworkCommunication::~NetworkCommunication(void)
{
}

NetworkCommunication * NetworkCommunication::getInstance() {
	if (s_instance == NULL) {
		s_instance = new NetworkCommunication();
	}

	return s_instance;
}

void NetworkCommunication::connect(char * ip, int port) {

	IP_PORT_INFO * pIPPortInfo  = new IP_PORT_INFO();
	pIPPortInfo->ip = ip;
	pIPPortInfo->port = port;

	pthread_create(&s_connectThreadId, NULL, connectThreadWork, pIPPortInfo);
	pthread_detach(s_connectThreadId);
}

void NetworkCommunication::dispatchResponseCallbacks(float delta) {
	SocketResponse * socketResponse = NULL;
	pthread_mutex_lock(&s_responseQueueMutex);
	if (s_responseQueue->count() > 0) {
		socketResponse = dynamic_cast<SocketResponse*> (s_responseQueue->objectAtIndex(0));
		s_responseQueue->removeObjectAtIndex(0);
	}
	pthread_mutex_unlock(&s_responseQueueMutex);

	if (socketResponse != NULL && target != NULL && selector != NULL) {
		(target->*selector)(socketResponse->buffer);
	}
}

void NetworkCommunication::setResponseCallback(CCObject* pTarget, SEL_SocketResponse pSelector) {
	target = pTarget;
	selector = pSelector;
}

void NetworkCommunication::setConnectResultCallback(CCObject* pTarget, SEL_ConnectResult pSelector) {
	connectResultTarget = pTarget;
	s_connectResultSelector = pSelector;
}

void NetworkCommunication::send(const char* buffer, int size) {
	SocketRequest* socketRequest = new SocketRequest(buffer, size);
	pthread_mutex_lock(&s_requestQueueMutex);
	s_requestQueue->addObject(socketRequest);
	pthread_mutex_unlock(&s_requestQueueMutex);

	pthread_cond_signal(&s_writeThreadSleepCondition);
}

NS_CC_EXT_END