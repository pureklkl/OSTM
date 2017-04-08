#include <stdio.h>
#include <stack>
#include <atomic>
#include <cstdlib>

#ifdef USECDS
#include <threads.h>
#include <mutex>
#include <model-assert.h>
#include "librace.h"
#endif

#include <assert.h>
#include <pthread.h>
#include <sys/time.h>
#include <stdarg.h>

#include "Transaction.h"
#include "ContentionManager.h"
#include "TThread.h"

#include "TStackNode.h"
#include "DoubleLinkList.h"
#include "SDoubleLinkList.h"

#define MAXADD 1

#define TESTPRENUM1 -10
#define TESTPRENUM2 10
#define TESTPRENUM3 30
#define TESTADDNUM1 35
#define TESTADDNUM2 -15
#define TESTREMOVENUM1 -10
#define TESTREMOVENUM2	10
#define TESTCONTAINNUM1 15
#define TESTCONTAINNUM2 -5

#define MINVAR -5000
#define MAXVAR 5000
#define PREADD 10
#define MAXOPERATION 20000
#define WRITERATIO (0.2)

#define SETSIZE 1000
#define SETWRITERATIO (0.2)
#define EACHOPT (32)

#define maxThread (2)
#define maxTestTime (100)

std::atomic<int> db_abort;

pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;

std::atomic<int> val;

TStackNode<int> c1, c2;
DoubleLinkList<int> dlist;
SDoubleLinkList<int> sdlist;
int vallist[maxThread*MAXOPERATION];
int operatelist[maxThread*MAXOPERATION];

TStackNode<int> Tnodeset[SETSIZE];
SStackNode<int> Snodeset[SETSIZE];

int setvarlist1[MAXOPERATION];
int setvarlist2[MAXOPERATION];
int setoperatelist[MAXOPERATION];

void genintargs(TF faddr, int * &args, int n, ...){
	 va_list   arg_ptr;
	 va_start(arg_ptr,n);
	 args = new int[n+1];
	 args[0] = (int) faddr;
	 for(int i=1;i<n+1;i++){
		 args[i]=va_arg(arg_ptr,int);
	 }
	 va_end(arg_ptr);
}
static __time_t caltime(timeval starttime, timeval endtime){
	return (1000000*(endtime.tv_sec-starttime.tv_sec)+endtime.tv_usec-starttime.tv_usec);
}
void addtwo1(Transaction *me, ContentionManager *cm, void* arg){
	dlist.add(me, cm, TESTADDNUM1);
}
void addtwo2(Transaction *me, ContentionManager *cm, void* arg){
	dlist.add(me, cm, TESTADDNUM2);
}

void removetwo1(Transaction *me, ContentionManager *cm, void* arg){
	dlist.remove(me, cm, TESTREMOVENUM1);
}

void removetwo2(Transaction *me, ContentionManager *cm, void* arg){
	dlist.remove(me, cm, TESTREMOVENUM2);
}

void linkListInit(Transaction *me, ContentionManager *cm, void* arg){
		dlist.add(me, cm, TESTPRENUM1);
		dlist.add(me, cm, TESTPRENUM2);
		dlist.add(me, cm, TESTPRENUM3);
}

void printList(Transaction *me, ContentionManager *cm, void* arg){
	dlist.print(me, cm);
}

void check(Transaction *me, ContentionManager *cm, void* arg){
	bool a1, a2, m1, m2, res=true;
	a1 = dlist.contain(me, cm, TESTADDNUM1);
	a2 = dlist.contain(me, cm, TESTADDNUM2);
	m1 = dlist.contain(me, cm, TESTREMOVENUM1);
	m2 = dlist.contain(me, cm, TESTREMOVENUM2);
	if(!a1){printf("a1 wrong");res = false;}
	if(!a2){printf("a2 wrong");res = false;}
	if(m1){printf("m1 wrong");res = false;}
	if(m2){printf("m2 wrong");res = false;}
#ifdef USECDS
	MODEL_ASSERT(res);
#else
	assert(res);
#endif
}

void* saddtwo1(void* arg){
	sdlist.add(TESTADDNUM1);
	return NULL;
}
void* saddtwo2(void* arg){
	sdlist.add(TESTADDNUM2);
	return NULL;
}

void* sremovetwo1(void* arg){
	sdlist.remove(TESTREMOVENUM1);
	return NULL;
}

void* sremovetwo2(void* arg){
	sdlist.remove(TESTREMOVENUM2);
	return NULL;
}

void* slinkListInit(void* arg){
		sdlist.add(TESTPRENUM1);
		sdlist.add(TESTPRENUM2);
		sdlist.add(TESTPRENUM3);
		return NULL;
}

void* sprintList(void* arg){
	sdlist.print();
	return NULL;
}

void scheck(void* arg){
	bool a1, a2, m1, m2, res=true;
	a1 = sdlist.contain(TESTADDNUM1);
	a2 = sdlist.contain(TESTADDNUM2);
	m1 = sdlist.contain(TESTREMOVENUM1);
	m2 = sdlist.contain(TESTREMOVENUM2);
	if(!a1){printf("a1 wrong");res = false;}
	if(!a2){printf("a2 wrong");res = false;}
	if(m1){printf("m1 wrong");res = false;}
	if(m2){printf("m2 wrong");res = false;}
#ifdef USECDS
	MODEL_ASSERT(res);
#else
	assert(res);
#endif
}


#ifdef USECDS

void tranCheckTwo(Transaction *me, ContentionManager *cm, void* arg){
	int t1=0, t2=0;
	do{
		t1 = c1.get(me, cm);
		t2 = c2.get(me, cm);
		printf("Check %d %d\n", t1, t2);
		if(t1!=t2||(t1!=0&&t1!=MAXADD+1)){
			printf("transtate %d", me->getStatus());
			MODEL_ASSERT(0);
		}
	}while(t1<MAXADD&&t2<MAXADD);
}

void tranAddTwo(Transaction *me, ContentionManager *cm, void* arg){
	int t1, t2;

	do{
		t1 = c1.get(me, cm);
		c1.set(me, cm, t1+1);
		t2 = c2.get(me, cm);
		c2.set(me, cm, t2+1);
		printf("Add %d %d\n", t1, t2);
		if(t1!=t2){
			printf("transtate %d", me->getStatus());
			MODEL_ASSERT(0);

		}
	}while(t1<MAXADD&&t2<MAXADD);

}

void test_add_two(){
	thrd_t adder, checker;
	c1.init(0, TStackNode<int>::FREEOBJREAD);
	c2.init(0, TStackNode<int>::FREEOBJREAD);

//	c1.init(0);
//	c2.init(0);

	TF faddr;
	int *argsadder = new int[1];
	faddr = &tranAddTwo;
	argsadder[0] = (int)faddr;
	thrd_create(&adder, &doit, argsadder);
//	thrd_create(&adder2, &doit, argsadder);

	int *argschecker = new int[1];
	faddr = &tranCheckTwo;
	argschecker[0] = (int)faddr;
	thrd_create(&checker, &doit, argschecker);
//	thrd_create(&checker2, &doit, argschecker);

	thrd_join(adder);
//	thrd_join(adder2);
	thrd_join(checker);
//	thrd_join(checker2);
}



void test_linkList(TDLinkNode<int>::AtomicObjectType atype){
	thrd_t adder1, adder2,
		   remover1, remover2;
	dlist.init(MINVAR, MAXVAR, atype);
	int * args;
	genintargs(linkListInit, args, 0);
	doit((void *) args);

	genintargs(printList, args, 0);
	doit((void *) args);

	int * argsaa1;
	genintargs(addtwo1, argsaa1, 0);
	thrd_create(&adder1, &doit, argsaa1);

	int * argsaa2;
	genintargs(addtwo2, argsaa2, 0);
	thrd_create(&adder2, &doit, argsaa2);

	int * argsma1;
	genintargs(removetwo1, argsma1, 0);
	thrd_create(&remover1, &doit, argsma1);

	int * argsma2;
	genintargs(removetwo2, argsma2, 0);
	thrd_create(&remover2, &doit, argsma2);

	thrd_join(adder1);
	thrd_join(adder2);
	thrd_join(remover1);
	thrd_join(remover2);

	genintargs(printList, args, 0);
	doit((void *) args);

	genintargs(check, args, 0);
	doit((void *) args);
}

int user_main(int argc, char **argv){
	printf("!!!finall!!\n");
	//test_lock_stack();
	//test_tran_stack();
	//test_add_two();
	test_linkList(TDLinkNode<int>::FREEOBJREAD);
	return 1;
}

#else

void tranAddTwo_P(Transaction *me, ContentionManager *cm, void* arg){
	int t1, t2;
		t1 = c1.get(me, cm);
		c1.set(me, cm, t1+1);
		t2 = c2.get(me, cm);
		c2.set(me, cm, t2+1);
}

timeval starttime, endtime;
void test_add_two_performance(TStackNode<int>::AtomicObjectType atype){
	pthread_t adder[maxThread];
	c1.init(0, atype);
	c2.init(0, atype);

	TF faddr;
	int *argsadder = new int[1];
	faddr = &tranAddTwo_P;
	argsadder[0] = (int)faddr;
	gettimeofday(&starttime, 0);
	for(int i=0; i<maxThread ;i++){
		int iret1 = pthread_create(&(adder[i]), NULL, doit_pthread, (void *) argsadder);
		if(iret1){
			printf("thread create fail");
			assert(0);
		}
	}
	for(int i = 0; i<maxThread; i++)
		pthread_join( adder[i], NULL);
	gettimeofday(&endtime, 0);
}

SStackNode<int> lc1, lc2;

void* lockaddtwo(void * args){
	pthread_mutex_lock(&mutex1);
	int t1, t2;
	t1 = lc1.get();
	lc1.set(t1+1);
	t2 = lc2.get();
	lc2.set(t2+1);
	printf("Add %d %d\n", t1, t2);
	if(t1!=t2){
		printf("erro!");
		while(true);
		assert(0);
	}
	pthread_mutex_unlock(&mutex1);
	return NULL;
}

void test_add_two_lock_performance(){
	pthread_t adder[maxThread];
	lc1.set(0);
	lc2.set(0);
	gettimeofday(&starttime, 0);
	for(int i=0; i<maxThread ;i++){
		int iret1 = pthread_create(&(adder[i]), NULL, lockaddtwo, NULL);
		if(iret1){
			printf("thread create fail");
			assert(0);
		}
	}
	for(int i = 0; i<maxThread; i++)
		pthread_join( adder[i], NULL);
	gettimeofday(&endtime, 0);
}
__time_t tlink = 0, slink = 0;
void test_linkList(TDLinkNode<int>::AtomicObjectType atype){
	pthread_t adder1, adder2,
		   	  remover1, remover2;
	dlist.init(MINVAR, MAXVAR, atype);
	int * args;
	genintargs(linkListInit, args, 0);
	doit((void *) args);

	genintargs(printList, args, 0);
	doit((void *) args);
	gettimeofday(&starttime, 0);
	int * argsaa1;
	genintargs(addtwo1, argsaa1, 0);
	int iret1 = pthread_create(&adder1, NULL, doit_pthread, (void *) argsaa1);
	if(iret1){
		printf("thread create fail");
		assert(0);
	}

	int * argsaa2;
	genintargs(addtwo2, argsaa2, 0);
	int iret2 = pthread_create(&adder2, NULL, doit_pthread, (void *) argsaa2);
	if(iret2){
		printf("thread create fail");
		assert(0);
	}

	int * argsma1;
	genintargs(removetwo1, argsma1, 0);
	int iret3 = pthread_create(&remover1, NULL, doit_pthread, (void *) argsma1);
	if(iret3){
		printf("thread create fail");
		assert(0);
	}

	int * argsma2;
	genintargs(removetwo2, argsma2, 0);
	int iret4 = pthread_create(&remover2, NULL, doit_pthread, (void *) argsma2);
	if(iret4){
		printf("thread create fail");
		assert(0);
	}

	pthread_join(adder1, NULL);
	pthread_join(adder2, NULL);
	pthread_join(remover1, NULL);
	pthread_join(remover2, NULL);
	gettimeofday(&endtime, 0);
    __time_t uset = caltime(starttime, endtime);
    tlink +=uset;
    printf("used time %ld\n", uset);

	genintargs(printList, args, 0);
	doit((void *) args);

	genintargs(check, args, 0);
	doit((void *) args);
}

void test_slinkList(){
	pthread_t adder1, adder2,
		   	  remover1, remover2;
	sdlist.init(MINVAR, MAXVAR);
	slinkListInit(NULL);
	sprintList(NULL);

	gettimeofday(&starttime, 0);
	int iret1 = pthread_create(&adder1, NULL, saddtwo1, NULL);
	if(iret1){
		printf("thread create fail");
		assert(0);
	}

	int iret2 = pthread_create(&adder2, NULL, saddtwo2, NULL);
	if(iret2){
		printf("thread create fail");
		assert(0);
	}

	int iret3 = pthread_create(&remover1, NULL, sremovetwo1, NULL);
	if(iret3){
		printf("thread create fail");
		assert(0);
	}

	int iret4 = pthread_create(&remover2, NULL, sremovetwo2, NULL);
	if(iret4){
		printf("thread create fail");
		assert(0);
	}

	pthread_join(adder1, NULL);
	pthread_join(adder2, NULL);
	pthread_join(remover1, NULL);
	pthread_join(remover2, NULL);
	gettimeofday(&endtime, 0);
    __time_t uset = caltime(starttime, endtime);
    printf("used time %ld\n", uset);
    sprintList(NULL);
    slink+=uset;
}
std::atomic_int opiter;

void normadd(Transaction *me, ContentionManager *cm, void* arg){
	int var = ((int*)arg)[1];
	dlist.add(me, cm, var);
}
void normremove(Transaction *me, ContentionManager *cm, void* arg){
	int var = ((int*)arg)[1];
	dlist.remove(me, cm, var);
}
void normcontain(Transaction *me, ContentionManager *cm, void* arg){
	int var = ((int*)arg)[1];
	dlist.contain(me, cm, var);
}
void* runDlist(void* arg){
//	int start = (((int*)arg)[1])*MAXOPERATION, end = start+MAXOPERATION;
//	for(int i = start; i<end; i++){
//		switch(operatelist[i]){
//		case 0:dlist.add(me, cm, vallist[i]);break;
//		case 1:dlist.remove(me, cm, vallist[i]);break;
//		case 2:dlist.contain(me, cm, vallist[i]);break;
//		}
//	}
	int *addargs, *removeargs, *containargs;
	genintargs(&normadd, addargs, 1, 0);
	genintargs(&normremove, removeargs, 1, 0);
	genintargs(&normcontain, containargs, 1, 0);
	while(true){
		int iter = opiter.fetch_add(1);
		if(iter>=maxThread*MAXOPERATION)
			return NULL;
		switch(operatelist[iter]){
		case 0:addargs[1]=vallist[iter];doit(addargs);break;
		case 1:removeargs[1]=vallist[iter];doit(removeargs);break;
		case 2:containargs[1]=vallist[iter];doit(containargs);break;
		}
	}
	return NULL;
}
void* runDlist_smart(void* arg){
//	int start = (((int*)arg)[1])*MAXOPERATION, end = start+MAXOPERATION;
//	for(int i = start; i<end; i++){
//		switch(operatelist[i]){
//		case 0:dlist.add(me, cm, vallist[i]);break;
//		case 1:dlist.remove(me, cm, vallist[i]);break;
//		case 2:dlist.contain(me, cm, vallist[i]);break;
//		}
//	}
	int *addargs, *removeargs, *containargs;
	genintargs(&normadd, addargs, 1, 0);
	genintargs(&normremove, removeargs, 1, 0);
	genintargs(&normcontain, containargs, 1, 0);
	while(true){
		int iter = opiter.fetch_add(1);
		if(iter>=maxThread*MAXOPERATION)
			return NULL;
		switch(operatelist[iter]){
		case 0:addargs[1]=vallist[iter];doit_smart(addargs);break;
		case 1:removeargs[1]=vallist[iter];doit_smart(removeargs);break;
		case 2:containargs[1]=vallist[iter];doit_smart(containargs);break;
		}
	}
	return NULL;
}
void * runDSlist(void* arg){
//	int start = (((int*)arg)[0])*MAXOPERATION, end = start+MAXOPERATION;
//	for(int i = start; i<end; i++){
//		switch(operatelist[i]){
//		case 0:sdlist.add(vallist[i]);break;
//		case 1:sdlist.remove(vallist[i]);break;
//		case 2:sdlist.contain(vallist[i]);break;
//		}
//	}
	while(true){
		int iter = opiter.fetch_add(1);
		if(iter>=maxThread*MAXOPERATION)
			return NULL;
		switch(operatelist[iter]){
		case 0:sdlist.add(vallist[iter]);break;
		case 1:sdlist.remove(vallist[iter]);break;
		case 2:sdlist.contain(vallist[iter]);break;
		}
	}
	return NULL;
}
void genOperateList(){
	int total = maxThread*MAXOPERATION;
	int range = MAXVAR - MINVAR;
	int addratio = WRITERATIO/2.*(double)RAND_MAX, removeratio = WRITERATIO*(double)RAND_MAX;
	for(int i = 0; i<total ; i++){
		vallist[i] = rand()%range + MINVAR;
		int opti = rand();
		if(opti<addratio){
			operatelist[i] = 0;
		}else if(opti<removeratio){
			operatelist[i] = 1;
		}else{
			operatelist[i] = 2;
		}
	}
}
void linkListInit_r(Transaction *me, ContentionManager *cm, void* arg){
	int range = MAXVAR - MINVAR;
	for(int i = 0; i<PREADD; i++){
		int var = rand()%range + MINVAR;
		dlist.add(me, cm, var);
	}
}
void linkSListInit_r(){
	int range = MAXVAR - MINVAR;
	for(int i = 0; i<PREADD; i++){
		int var = rand()%range + MINVAR;
		sdlist.add(var);
	}
}

enum CMTYPE{BACKOFCM, SMARTBACKOFCM};
__time_t test_linkList_r(TDLinkNode<int>::AtomicObjectType atype, CMTYPE cmt){
	pthread_t adder[maxThread];
	dlist.init(MINVAR-1, MAXVAR, atype);
	int * initargs;
	genintargs(linkListInit_r, initargs, 0);
	doit(initargs);
	gettimeofday(&starttime, 0);
	for(int i=0; i<maxThread ;i++){
		int iret1;
		switch(cmt){
		case CMTYPE::BACKOFCM :
			iret1 = pthread_create(&(adder[i]), NULL, runDlist, NULL);
			break;
		case CMTYPE::SMARTBACKOFCM :
			iret1 = pthread_create(&(adder[i]), NULL, runDlist_smart, NULL);
			break;
		}
		if(iret1){
			printf("thread create fail");
			assert(0);
		}
	}
	for(int i = 0; i<maxThread; i++)
		pthread_join( adder[i], NULL);
	gettimeofday(&endtime, 0);
    __time_t uset = caltime(starttime, endtime);
    printf("used time %ld\n", uset);
    return uset;
}
__time_t test_slinkList_r(){
	pthread_t adder[maxThread];
	sdlist.init(MINVAR-1, MAXVAR);
	linkSListInit_r();
	gettimeofday(&starttime, 0);
	for(int i=0; i<maxThread ;i++){
		int *argsadder = new int[1];
		argsadder[0] = i;
		int iret1= pthread_create(&(adder[i]), NULL, runDSlist, (void *)argsadder);
		if(iret1){
			printf("thread create fail");
			assert(0);
		}
	}
	for(int i = 0; i<maxThread; i++)
		pthread_join( adder[i], NULL);
	gettimeofday(&endtime, 0);
    __time_t uset = caltime(starttime, endtime);
    printf("used time %ld\n", uset);
    return uset;
}

void tranAddTwo_PSET(Transaction *me, ContentionManager *cm, void* arg){
	int t1, t2, c=EACHOPT;
	int iter1=((int *)arg)[1], iter2 = ((int *)arg)[2];
	while(c-->0){
		t1 = Tnodeset[iter1].get(me, cm);
		Tnodeset[iter1].set(me, cm, t1+1);
		t2 = Tnodeset[iter2].get(me, cm);
		Tnodeset[iter2].set(me, cm, t2+1);
	}
}
void tranCheckTwo_PSET(Transaction *me, ContentionManager *cm, void* arg){
	int t1, t2, c=EACHOPT;
	int iter1=((int *)arg)[1], iter2 = ((int *)arg)[2];
	while(c-->0){
		t1 = Tnodeset[iter1].get(me, cm);
		t2 = Tnodeset[iter2].get(me, cm);
	}
}
void * runADDTwo_smart(void *arg){
	int *addargs, *checkargs;
	genintargs(&tranAddTwo_PSET, addargs, 2, 0, 0);
	genintargs(&tranCheckTwo_PSET, checkargs, 2, 0, 0);
	while(true){
		int iter = opiter.fetch_add(1);
		if(iter>=MAXOPERATION)
			return NULL;
		switch(operatelist[iter]){
		case 0:addargs[1]=setvarlist1[iter];
			   addargs[2]=setvarlist2[iter];
			   doit_smart(addargs);break;
		case 1:checkargs[1]=setvarlist1[iter];
			   checkargs[2]=setvarlist2[iter];
			   doit_smart(checkargs);break;
		}
	}
	return NULL;
}
void * runADDTwo(void *arg){
	int *addargs, *checkargs;
	genintargs(&tranAddTwo_PSET, addargs, 2, 0, 0);
	genintargs(&tranCheckTwo_PSET, checkargs, 2, 0, 0);
	while(true){
		int iter = opiter.fetch_add(1);
		if(iter>=MAXOPERATION)
			return NULL;
		switch(operatelist[iter]){
		case 0:addargs[1]=setvarlist1[iter];
			   addargs[2]=setvarlist2[iter];
			   doit(addargs);break;
		case 1:checkargs[1]=setvarlist1[iter];
			   checkargs[2]=setvarlist2[iter];
			   doit(checkargs);break;
		}
	}
	return NULL;
}
__time_t test_ADDTwoSET_r(TStackNode<int>::AtomicObjectType atype, CMTYPE cmt){
	pthread_t adder[maxThread];
	for(int i = 0; i<MAXOPERATION; i++){
		Tnodeset[i].init(0, atype);
	}
	gettimeofday(&starttime, 0);
	for(int i=0; i<maxThread ;i++){
		int iret1;
		switch(cmt){
		case CMTYPE::BACKOFCM :
			iret1 = pthread_create(&(adder[i]), NULL, runADDTwo, NULL);
			break;
		case CMTYPE::SMARTBACKOFCM :
			iret1 = pthread_create(&(adder[i]), NULL, runADDTwo_smart, NULL);
			break;
		}
		if(iret1){
			printf("thread create fail");
			assert(0);
		}
	}
	for(int i = 0; i<maxThread; i++)
		pthread_join( adder[i], NULL);
	gettimeofday(&endtime, 0);
    __time_t uset = caltime(starttime, endtime);
    printf("used time %ld\n", uset);
    return uset;
}
void tranSAddTwo_PSET(void* arg){
	int t1, t2, c=EACHOPT;
	int iter1=((int *)arg)[1], iter2 = ((int *)arg)[2];
	pthread_mutex_lock(&mutex1);
	while(c-->0){
		t1 = Snodeset[iter1].get();
		Snodeset[iter1].set(t1+1);
		t2 = Snodeset[iter2].get();
		Snodeset[iter2].set(t2+1);
	}
	pthread_mutex_unlock(&mutex1);
}
void tranSCheckTwo_PSET(void* arg){
	int t1, t2, c=EACHOPT;
	int iter1=((int *)arg)[1], iter2 = ((int *)arg)[2];
	pthread_mutex_lock(&mutex1);
	while(c-->0){
		t1 = Snodeset[iter1].get();
		t2 = Snodeset[iter2].get();
	}
	pthread_mutex_unlock(&mutex1);
}
void * runSADDTwo(void *arg){
	int *addargs, *checkargs;
	genintargs(NULL, addargs, 2, 0, 0);
	genintargs(NULL, checkargs, 2, 0, 0);
	while(true){
		int iter = opiter.fetch_add(1);
		if(iter>=MAXOPERATION)
			return NULL;
		switch(operatelist[iter]){
		case 0:addargs[1]=setvarlist1[iter];
			   addargs[2]=setvarlist2[iter];
			   tranSAddTwo_PSET(addargs);break;
		case 1:checkargs[1]=setvarlist1[iter];
			   checkargs[2]=setvarlist2[iter];
			   tranSCheckTwo_PSET(checkargs);break;
		}
	}
	return NULL;
}
__time_t test_sADDTwoSET_r(){
	pthread_t adder[maxThread];
	gettimeofday(&starttime, 0);
	for(int i=0; i<maxThread ;i++){
		int iret1;
			iret1 = pthread_create(&(adder[i]), NULL, runSADDTwo, NULL);
		if(iret1){
			printf("thread create fail");
			assert(0);
		}
	}
	for(int i = 0; i<maxThread; i++)
		pthread_join( adder[i], NULL);
	gettimeofday(&endtime, 0);
    __time_t uset = caltime(starttime, endtime);
    printf("used time %ld\n", uset);
    return uset;
}
void genSETOperateList(){
	int total = MAXOPERATION;
	int range = SETSIZE;
	int addratio = SETWRITERATIO*(double)RAND_MAX;
	for(int i = 0; i<total ; i++){
		setvarlist1[i] = rand()%range;
		while((setvarlist2[i] = rand()%range)==setvarlist1[i]);
		int opti = rand();
		if(opti<addratio){
			setoperatelist[i] = 0;
		}else{
			setoperatelist[i] = 1;
		}
	}
}

int main(int argc, char **argv){
	printf("!!!FINALL!!\n");
	//test_lock_stack();
	//test_tran_stack();
	//test_add_two();
#ifdef USEBGC
	GC_INIT();
#endif
	srand(42);
	//genOperateList();
	genSETOperateList();
//	int i=20;
//	while(i--){
//		test_slinkList();
//		test_linkList(TDLinkNode<int>::FREEOBJREAD);
//	}
//	printf("lock : %ld\n", slink);
//	printf("transaction : %ld\n", tlink);
	__time_t total = 0;
	for(int i = 0;i<20;i++){
		opiter.store(0);
		//total+=test_linkList_r(TDLinkNode<int>::FREEOBJREAD, CMTYPE::SMARTBACKOFCM);
		//total+=test_linkList_r(TDLinkNode<int>::FREEOBJREAD, CMTYPE::BACKOFCM);
		//total+=test_linkList_r(TDLinkNode<int>::FREEOBJ, CMTYPE::SMARTBACKOFCM);
		//total+=test_linkList_r(TDLinkNode<int>::FREEOBJ, CMTYPE::BACKOFCM);
		//total+=test_slinkList_r();
		total+=test_ADDTwoSET_r(TStackNode<int>::FREEOBJ, CMTYPE::SMARTBACKOFCM);
		//total+=test_ADDTwoSET_r(TStackNode<int>::FREEOBJREAD, CMTYPE::BACKOFCM);
		//total+=test_ADDTwoSET_r(TStackNode<int>::FREEOBJ, CMTYPE::SMARTBACKOFCM);
		//total+=test_ADDTwoSET_r(TStackNode<int>::FREEOBJ, CMTYPE::BACKOFCM);
		//total+=test_sADDTwoSET_r();
	}
	printf("aver: %lf\n", (double)total/20.);
	return 1;
}
#endif


