/*
 * BackofManager.h
 *
 *  Created on: Nov 23, 2015
 *      Author: yuanfan
 */

#ifndef SRC_BACKOFMANAGER_H_
#define SRC_BACKOFMANAGER_H_
#ifdef USECDS
#include <threads.h>
#else
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#endif
#include "ContentionManager.h"
class BackofManager: public ContentionManager {
public:
	BackofManager();
	virtual ~BackofManager();
	void init();
	void resolve(Transaction* me, Transaction* other);

protected:
#ifdef USECDS
	void _sleep(int t){while((t--)>0);
	thrd_yield();
	};
#else
	void _sleep(int t){
		//usleep(t);
//		timespec tim;
//		tim.tv_sec=0;
//		tim.tv_nsec = t;
//		nanosleep(&tim, NULL);
		while((t--)>0){pthread_yield();};
	};
#endif
	static const int MIN_DELAY = 1;//TBD(To Be Determined)
	static const int MAX_DELAY = 128;//TBD
	Transaction* previous;
	int delay;
};

#endif /* SRC_BACKOFMANAGER_H_ */
