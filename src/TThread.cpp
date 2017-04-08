/*
 * TThread.cpp
 *
 *  Created on: Nov 18, 2015
 *      Author: yuanfan
 */

#include "TThread.h"

#define STUDYRATE (0.8)
#define FORGETRATE (0.2)

static __time_t caltime(timespec starttime, timespec endtime){
	return (1000000000*(endtime.tv_sec-starttime.tv_sec)+endtime.tv_nsec-starttime.tv_nsec);
}

void doit(void* arg){
	while(true){
		Transaction * me = new Transaction();
		BackofManager * cm = new BackofManager();
		cm->init();
		me->init();
		try{
			TF action= (TF)(((int *)arg)[0]);
			(*action)(me, cm, arg);
		}catch(AbortedException &e){
		}catch(...){
			throw;
		}
		if(me->commit()){
			//printf("finish!");
			return;
		}
		me->abort();
	}
}

void doit_smart(void* arg){
	__time_t mintime = 0;
	timespec starttime, aborttime;
	while(true){
		Transaction * me = new Transaction();
		SmartBackofCM * cm = new SmartBackofCM();
		cm->init();
		me->init(mintime);
		try{
			TF action= (TF)(((int *)arg)[0]);
			clock_gettime(CLOCK_REALTIME, &starttime);
			me->setstarttime(starttime);
			(*action)(me, cm, arg);
		}catch(AbortedException &e){
			//if(!me->getabortstatus()){
				aborttime = e.getaborttime();
				__time_t runtime = caltime(starttime, aborttime)-me->getsleeptime();
				mintime = (mintime + runtime)>>1;
			//}
			printf("mintime : %ld\n", mintime);
//			if(mintime<runtime){
//				mintime = (1.-STUDYRATE)*(double)mintime + STUDYRATE*(double)runtime;
//			}
//			else{
//				mintime = (1.-FORGETRATE)*(double)mintime + FORGETRATE*(double)runtime;
//			}
		}catch(...){
			throw;
		}
		if(me->commit()){
			//printf("finish!");
			return;
		}
		me->abort();
	}
}

void * doit_pthread(void* arg){
	doit(arg);
	return NULL;
}
void * doit_smart_pthread(void* arg){
	doit_smart(arg);
	return NULL;
}

