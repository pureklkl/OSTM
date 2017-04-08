/*
 * TThread.h
 *
 *  Created on: Nov 18, 2015
 *      Author: yuanfan
 */

#ifndef SRC_TTHREAD_H_
#define SRC_TTHREAD_H_

#include <stdio.h>
#include"AbortedException.h"
#include"Transaction.h"
#include"BackofManager.h"
#include"SmartBackofCM.h"
#include <sys/time.h>
#include <time.h>
typedef void (*TF)(Transaction *, ContentionManager *, void *);

void doit(void * args);
void * doit_pthread(void* arg);
void doit_smart(void* arg);
void * doit_smart_pthread(void* arg);
//args[0] should be your transaction function TF
//TF should be like void TF(Transaction *,
//					        ContentionManager *,
//					        void * args)
#endif /* SRC_TTHREAD_H_ */
