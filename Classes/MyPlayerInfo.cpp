#include "MyPlayerInfo.h"

static MyPlayerInfo * myPlayerInfoInstance;


MyPlayerInfo * MyPlayerInfo::getInstance() {
	if (myPlayerInfoInstance == NULL) {
		myPlayerInfoInstance = new MyPlayerInfo();
	}

	return myPlayerInfoInstance;
}