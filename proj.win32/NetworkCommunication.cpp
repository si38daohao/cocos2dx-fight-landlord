#include "NetworkCommunication.h"

#include <pthread\pthread.h>

NS_CC_EXT_BEGIN

static NetworkCommunication * s_instance;

static CCArray * s_requestQueue;
static CCArray * s_responseQueue;

static pthread_t s_networkReadThread;
static pthread_t s_networkWriteThread;

static pthread_mutex_t s_requestQueueMutex;
static pthread_mutex_t s_responseQueueMutex;

static pthread_mutex_t s_readThreadSleepMutex;
static pthread_cond_t s_readThreadSleepCondition;

static pthread_mutex_t s_writeThreadSleepMutex;
static pthread_cond_t s_writeThreadSleepCondition;


static void* networkReadThreadWork(void *data) {

	CCLog("Jonathan: in networkReadThreadWork");

	

	return NULL;
}

static void* networkWriteThreadWork(void *data) {
	CCLog("Jonathan: in networkWriteThreadWork");

	std::string * request = NULL;
	while (true) {

		pthread_mutex_lock(&s_requestQueueMutex);
		if (0 != s_requestQueue->count()) {
			request = dynamic_cast<std::string *> (s_requestQueue->objectAtIndex(0));
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


NS_CC_EXT_END