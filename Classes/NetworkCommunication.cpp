#include "NetworkCommunication.h"
#include "cocos2d.h"

#include <pthread\pthread.h>

NS_CC_EXT_BEGIN

static NetworkCommunication * s_instance;

static CCArray * s_requestQueue;
static CCArray * s_responseQueue;

static char * response = NULL;

static pthread_t s_networkReadThread;
static pthread_t s_networkWriteThread;

static pthread_mutex_t s_requestQueueMutex;
static pthread_mutex_t s_responseQueueMutex;

static pthread_mutex_t s_readThreadSleepMutex;
static pthread_cond_t s_readThreadSleepCondition;

static pthread_mutex_t s_writeThreadSleepMutex;
static pthread_cond_t s_writeThreadSleepCondition;

static char * ip;
static int port;
static ODSocket * odsocket;
	
static CCObject * target = NULL;
static SEL_HttpResponse_Jonathan selector = NULL;

static void* networkReadThreadWork(void *data) {

	CCLog("Jonathan: in networkReadThreadWork");

	char recvBuffer[1024];
	while (true) {
		CCLog("Jonathan: in while loop");
		int ret = odsocket->Recv(recvBuffer, 1024);

		if (ret > 0) {
			recvBuffer[ret] = '\0';
			CCLog(recvBuffer);

			pthread_mutex_lock(&s_responseQueueMutex);
			char * tmp = new char[1024];
			strcpy_s(tmp, 1024, recvBuffer);
			response = tmp;
			pthread_mutex_unlock(&s_responseQueueMutex);
		} else {
			pthread_cond_wait(&s_readThreadSleepCondition, &s_readThreadSleepMutex);
		}
	}

	return NULL;
}

static void* networkWriteThreadWork(void *data) {
	CCLog("Jonathan: in networkWriteThreadWork");

	std::string * request = NULL;
	while (true) {

		pthread_mutex_lock(&s_requestQueueMutex);
		if (0 != s_requestQueue->count()) {
			request = dynamic_cast<std::string *> (s_requestQueue->objectAtIndex(0));
			CCLog(request->c_str());
		}
		pthread_mutex_unlock(&s_requestQueueMutex);

		if (NULL == request) {
			pthread_cond_wait(&s_writeThreadSleepCondition, &s_writeThreadSleepMutex);
			continue;
		}
		

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

	pthread_create(&s_networkReadThread, NULL, networkReadThreadWork, NULL);
	pthread_detach(s_networkReadThread);

	pthread_create(&s_networkWriteThread, NULL, networkWriteThreadWork, NULL);
	pthread_detach(s_networkWriteThread);

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

	CCLog("Jonathan: before connect");
	char * content = "{\"command\":6, \"player_id\":\"12345678\"}";
	odsocket->Connect(ip, port);
	odsocket->Send(content, strlen(content));
	CCLog("Jonathan: after connect");
	pthread_cond_signal(&s_readThreadSleepCondition);
	
}

void NetworkCommunication::dispatchResponseCallbacks(float delta) {
	char * res = NULL;
	pthread_mutex_lock(&s_responseQueueMutex);
	res = response;
	response = NULL;
	pthread_mutex_unlock(&s_responseQueueMutex);

	if (res != NULL && target != NULL && selector != NULL) {
		(target->*selector)(res);
	}
}

void NetworkCommunication::setResponseCallback(CCObject* pTarget, SEL_HttpResponse_Jonathan pSelector) {
	target = pTarget;
	selector = pSelector;
}

NS_CC_EXT_END