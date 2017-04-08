/*
 * SmartBackofCM.cpp
 *
 *  Created on: Dec 2, 2015
 *      Author: yuanfan
 */

#include "SmartBackofCM.h"

static __time_t caltime(timespec starttime, timespec endtime){
	return (1000000000*(endtime.tv_sec-starttime.tv_sec)+endtime.tv_nsec-starttime.tv_nsec);
}

void SmartBackofCM::resolve(Transaction* me, Transaction* other){
	if((other) != previous){
		previous = other;
		delay = MIN_DELAY;
		timespec nowtime;
		clock_gettime(CLOCK_REALTIME, &nowtime);
		__time_t expectedtime = other->getminruntime() - caltime(other->getstarttime(), nowtime);
		if(expectedtime>=(MAX_DELAY*2)){
			other->setpredictabort();
			other->abort();
			printf("smart!\n");
			return;
		}
	}

	if(delay < MAX_DELAY){
		_sleep(delay);
		me->addsleeptime(delay);
		delay<<=1;
	}else{
		other->abort();
		delay = MIN_DELAY;
	}
}
