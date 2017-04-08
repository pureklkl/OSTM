/*
 * BackofManager.cpp
 *
 *  Created on: Nov 23, 2015
 *      Author: yuanfan
 */

#include "BackofManager.h"

BackofManager::BackofManager() {
	// TODO Auto-generated constructor stub
	delay = MIN_DELAY;
	previous = NULL;
}

BackofManager::~BackofManager() {
	// TODO Auto-generated destructor stub
}

void BackofManager::init(){
	previous = NULL;
	delay = MIN_DELAY;
}

void BackofManager::resolve(Transaction* me, Transaction* other){
	if((other) != previous){
		previous = other;
		delay = MIN_DELAY;
	}
	if(delay < MAX_DELAY){
		_sleep(delay);
		delay<<=1;
	}else{
		other->abort();
		delay = MIN_DELAY;
	}
}
