/*
 * FreeObject.h
 *
 *  Created on: Nov 23, 2015
 *      Author: yuanfan
 */

#ifndef SRC_FREEOBJECT_H_
#define SRC_FREEOBJECT_H_

#include<atomic>

#include "AtomicObject.h"
#include "Copyable.h"
#include "Transaction.h"
#include "ContentionManager.h"
#include "AbortedException.h"

extern std::atomic<int> db_abort;

template<class T>//check T in constructor
class FreeObject: public AtomicObject<T>{
public:
	FreeObject(){};
	FreeObject(T * st);
	FreeObject(T * st, Transaction *me);
	virtual ~FreeObject();
	T* openWrite(Transaction *me, ContentionManager *cm);
	T* openRead(Transaction *me, ContentionManager *cm);
	bool validate(Transaction *me);
protected:
#ifdef USEBGC
	class locator : public gc{
#else
	class locator{
#endif
	public:
		Transaction* owner;
		T* oldver;
		T* newver;
	};
	std::atomic_intptr_t start;
};

template<class T> FreeObject<T>::FreeObject(T* st) {
	// TODO Auto-generated constructor stub
	locator * initl = new locator();
	initl->oldver = st;
	initl->newver = new T();
	initl->oldver->template copyTo(initl->newver);

	initl->owner = new Transaction();
	(initl->owner)->init();
	(initl->owner)->commit();
#ifdef USECDS
	std::atomic_init(&start, (long)initl);
#else
	start.store((long)initl);
#endif
}

template<class T> FreeObject<T>::FreeObject(T* st, Transaction* me) {
	// TODO Auto-generated constructor stub
	locator * initl = new locator();
	initl->oldver = st;
	initl->newver = new T();
	initl->oldver->template copyTo(initl->newver);

	initl->owner = me;
#ifdef USECDS
	std::atomic_init(&start, (long)initl);
#else
	start.store((long)initl);
#endif
}

template<class T> FreeObject<T>::~FreeObject() {
	// TODO Auto-generated destructor stub

}

template<class T> T* FreeObject<T>::openWrite(Transaction *me, ContentionManager *cm) {
	switch(me->getStatus()){
	case Transaction::COMMIT: return ((locator *)start.load())->newver;break;
	case Transaction::ABORT: throw AbortedException();break;
	case Transaction::ACTIVE:{
		locator * l = ((locator *)start.load());
		if(l->owner == me){
			return l->newver;
		}
		locator * newl= new locator();
		newl->owner = me;
		bool isInterrupt = false;
		while(!isInterrupt){
			locator *oldl = ((locator *)start.load());
			Transaction * curowner = oldl->owner;
			switch(curowner->getStatus()){
			case Transaction::COMMIT:
				newl->oldver = oldl->newver;break;
			case Transaction::ABORT:
				newl->oldver = oldl->oldver;break;
			case Transaction::ACTIVE:
				cm->resolve(me, oldl->owner);
				continue;
			}
			try{
				newl->newver = new T();
			}catch(...){
				throw;
			}
			newl->oldver->template copyTo(newl->newver);
#ifdef USECDS
			long oldaddr = (long)oldl;
			if(start.compare_exchange_strong(oldaddr, (long)newl))
				return newl->newver;
#else
			int oldaddr = (int)oldl;
			if(start.compare_exchange_strong(oldaddr, (long)newl))
				return newl->newver;
#endif

		}break;
		}
	default: printf("UNKNOW Transaction!"); break;
	}
#ifdef USECDS
		MODEL_ASSERT(0);
#else
		assert(0);
#endif
	return NULL;
}

template<class T> T* FreeObject<T>::openRead(Transaction *me, ContentionManager *cm) {
	switch(me->getStatus()){
	case Transaction::COMMIT: return ((locator *)start.load())->newver;break;
	case Transaction::ABORT: throw AbortedException();break;
	case Transaction::ACTIVE:{
		locator * l = ((locator *)start.load());
		if(l->owner == me)
			return l->newver;
		locator * newl= new locator();
		newl->owner = me;
		bool isInterrupt = false;
		while(!isInterrupt){
			locator *oldl = ((locator *)start.load());
			Transaction * curowner = oldl->owner;
			switch(curowner->getStatus()){
			case Transaction::COMMIT:
				//newl->oldver = oldl->oldver;
				//newl->newver = oldl->newver;
				newl->oldver = oldl->newver;
				//printf("commit!");
				break;
			case Transaction::ABORT:
				newl->oldver = oldl->oldver;
				//newl->newver = oldl->oldver;
				//if(db_abort.fetch_add(1)==2){
				//	printf("%x", oldl);
				//}
				//printf("abort!");
				break;
			case Transaction::ACTIVE:
				cm->resolve(me, oldl->owner);
				continue;
			}
			try{
				newl->newver = new T();
			}catch(...){
				throw;
			}
			newl->oldver->copyTo(newl->newver);
#ifdef USECDS
			long oldaddr = (long)oldl;
			if(start.compare_exchange_strong(oldaddr, (long)newl))
				return newl->newver;
#else
			int oldaddr = (int)oldl;
			if(start.compare_exchange_strong(oldaddr, (long)newl))
				return newl->newver;
#endif

		}break;
		}
	default: printf("UNKNOW Transaction!"); break;
	}
#ifdef USECDS
			MODEL_ASSERT(0);
#else
			assert(0);
#endif
	return NULL;
}

template<class T> bool FreeObject<T>::validate(Transaction* me){
	return me->getStatus()==Transaction::ACTIVE;
}

#endif /* SRC_FREEOBJECT_H_ */
