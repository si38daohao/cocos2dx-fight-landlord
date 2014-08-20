#include "PlayerInfo.h"


void PlayerInfo::setId(const char * id) {
	this->id = id;
}

const char * PlayerInfo::getId() {
	return this->id;
}

void PlayerInfo::setName(const char * name) {
	this->name = name;
}

const char * PlayerInfo::getName() {
	return this->name;
}

void PlayerInfo::setCoinAmount(int amount) {
	this->coinAmount = amount;
}

int PlayerInfo::getCoinAmount() {
	return this->coinAmount;
}

void PlayerInfo::setCurrentRoomId(const char* roomId) {
	this->currentRoomId = roomId;
}

const char * PlayerInfo::getCurrentRoomId() {
	return this->currentRoomId;
}

void PlayerInfo::setCurrentSeatIndex(int index) {
	this->currentSeatIndex = index;
}

int PlayerInfo::getCurrentSeatIndex() {
	return this->currentSeatIndex;
}