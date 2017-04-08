/*
 * Transaction.cpp
 *
 *  Created on: Nov 18, 2015
 *      Author: yuanfan
 */

#include "Transaction.h"

Transaction::Transaction() {
	// TODO Auto-generated constructor stub
	//Transaction(ACTIVE);
}

Transaction::~Transaction() {
	// TODO Auto-generated destructor stub
}

void Transaction::init(){
	init(0);
}

void Transaction::init(__time_t mintime){
#ifdef USECDS
	atomic_init(&status, ACTIVE);
#else
	status.store(ACTIVE);
#endif
	mintime_ = mintime;
	abortByPredict = false;
	sleeptime = 0;
}

bool Transaction::commit(){
	Status s=Transaction::ACTIVE;
	return status.compare_exchange_strong(s, (int)Transaction::COMMIT);
}

bool Transaction::abort(){
	Status s=Transaction::ACTIVE;
	return status.compare_exchange_strong(s, (int)Transaction::ABORT);
}

Transaction::Status Transaction::getStatus(){
	return status.load();
}
