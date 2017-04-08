/*
 * FreeObjectRead.h
 *
 *  Created on: Nov 28, 2015
 *      Author: yuanfan
 */

#ifndef SRC_FREEOBJECTREAD_H_
#define SRC_FREEOBJECTREAD_H_

#include "AtomicObject.h"
#include "Copyable.h"
#include "Transaction.h"
#include "ContentionManager.h"
#include "AbortedException.h"

template<class T>
class FreeObjectRead: public AtomicObject<T> {
public:
	FreeObjectRead(){};
	FreeObjectRead(T* st);
	FreeObjectRead(T* st, Transaction *me);
	virtual ~FreeObjectRead(){};
	T* openWrite(Transaction *me, ContentionManager *cm);
	T* openRead(Transaction *me, ContentionManager *cm);
	bool validate(Transaction *me);
protected:
#ifdef USEBGC
	class locatorOL : public gc{
#else
	class locatorOL {
#endif
	public:
		enum RWStat{READT, WRITET, UNKNOWN};
		typedef int rwstatus;
		typedef struct OwnerList{
			Transaction* tran_;
			RWStat rws_;
			OwnerList* nextT_;
			OwnerList(Transaction* tran, RWStat rws): tran_(tran), rws_(rws), nextT_(NULL){}
			void CopyTo(OwnerList* target){
				target->tran_ = tran_;
				target->rws_ = rws_;
			}
		}OwnerList;
		//For reader
		bool haveMe(Transaction* tran){
			OwnerList* cur = owner;
			while(cur!=NULL){
				if((cur->tran_) == tran)
					return true;
				cur = cur->nextT_;
			}
			return false;}
		OwnerList * getCopy(){
			if(owner==NULL) return NULL;
			OwnerList *copyHead = NULL, *copycur = NULL, *cur = owner;
			while(cur!=NULL){
				if(cur->tran_->getStatus()==Transaction::ACTIVE){
					if(copycur == NULL){
						copycur = new OwnerList(NULL, READT);
						copyHead = copycur;
					}else{
						copycur->nextT_ = new OwnerList(NULL, READT);
						copycur = copycur->nextT_;
					}
					cur->CopyTo(copycur);
					copycur->nextT_= NULL;
				}
				cur = cur->nextT_;
			}
			return copyHead;
		}
		//For Writer
		bool isMe(Transaction* tran){
			if(owner==NULL){
#ifdef USECDS
			MODEL_ASSERT(0);
#else
			assert(0);
#endif
				return false;
			}
			else
				return (((owner->tran_) == tran)&&
						((owner->nextT_) == NULL));
		}
		bool isMeWrite(Transaction* tran){
			return isMe(tran)&&((owner->rws_)==WRITET);
		}
		bool isMeRead(Transaction* tran){
			return isMe(tran)&&((owner->rws_)==READT);
		}
		Transaction* getActive(Transaction* me){
			OwnerList* cur = owner;
			while(cur!=NULL){
				if(cur->tran_!=me && cur->tran_->getStatus()==Transaction::ACTIVE)
					return cur->tran_;
				cur = cur->nextT_;
			}
			return NULL;
		}
		//Comment
		void clearOwner(){owner = NULL;}
		void setOwner(OwnerList* ol){owner = ol;}
		void putMetoEnd(Transaction* me, RWStat rws){
			if(owner == NULL)
				owner = new OwnerList(me, rws);
			else{
				OwnerList* cur = owner;
				while(cur->nextT_!=NULL){cur = cur->nextT_;}
				cur->nextT_ = new OwnerList(me, rws);
			}
		}
		RWStat getRWStat(){
			if(owner == NULL){return UNKNOWN;}
			else{return owner->rws_;}
		}
		Transaction::Status getTranStatus(Transaction* me){
			OwnerList* cur = owner;
			while(cur!=NULL){
				if(cur->tran_!=me && cur->tran_->getStatus()==Transaction::ACTIVE)
					return Transaction::ACTIVE;
				cur = cur->nextT_;
			}
			cur = owner;
			while(cur!=NULL){
				if(cur->tran_!=me)
					return cur->tran_->getStatus();
				cur = cur->nextT_;
			}
			if(!isMeRead(me)){
#ifdef USECDS
			MODEL_ASSERT(0);
#else
			assert(0);
#endif
			}
			return Transaction::COMMIT;//reach here only when a writer want to change the read access to write access on current object
		}

		T* oldver;
		T* newver;
		OwnerList* owner;
	};
	std::atomic_intptr_t start;
};

template<class T> FreeObjectRead<T>::FreeObjectRead(T* st) {
	// TODO Auto-generated constructor stub
	locatorOL * initl = new locatorOL();
	initl->oldver = st;
	initl->newver = new T();
	initl->oldver->template copyTo(initl->newver);

	Transaction* init_me = new Transaction();
	init_me->init();
	init_me->commit();
	initl->clearOwner();
	initl->putMetoEnd(init_me, locatorOL::WRITET);
#ifdef USECDS
	atomic_init(&start, (long)initl);
#else
	start.store((long)initl);
#endif
}

template<class T> FreeObjectRead<T>::FreeObjectRead(T* st, Transaction *me) {
	// TODO Auto-generated constructor stub
	locatorOL * initl = new locatorOL();
	initl->oldver = st;
	initl->newver = new T();
	initl->oldver->template copyTo(initl->newver);

	Transaction* init_me = me;
	initl->clearOwner();
	initl->putMetoEnd(init_me, locatorOL::WRITET);
#ifdef USECDS
	atomic_init(&start, (long)initl);
#else
	start.store((long)initl);
#endif
}

template<class T> T* FreeObjectRead<T>::openRead(Transaction *me, ContentionManager *cm){
	switch(me->getStatus()){
		case Transaction::COMMIT: return ((locatorOL *)start.load())->newver;break;
		case Transaction::ABORT: throw AbortedException();break;
		case Transaction::ACTIVE:
			locatorOL * l = ((locatorOL *)start.load());

			if(l->haveMe(me))
				return l->newver;

			locatorOL * newl= new locatorOL();
			bool isInterrupt = false;
			while(!isInterrupt){
				locatorOL *oldl = ((locatorOL *)start.load());
#ifdef USECDS
				long oldaddrRead;
#else
				int oldaddrRead;
#endif
				switch(oldl->getTranStatus(me)){
				case Transaction::COMMIT	: newl->oldver = oldl->newver;break;
				case Transaction::ABORT		: newl->oldver = oldl->oldver;break;
				case Transaction::ACTIVE	:
					switch(oldl->getRWStat()){
					case locatorOL::UNKNOWN: break;
					case locatorOL::READT:
						newl->setOwner(oldl->getCopy());//filter commit, abort reader
						newl->putMetoEnd(me, locatorOL::READT);//only contain active reader
						newl->oldver = oldl->oldver;
						newl->newver = oldl->newver;
#ifndef USECDS
			oldaddrRead = (long)oldl;
			if(start.compare_exchange_strong(oldaddrRead, (long)newl))
				return newl->newver;
#else
			oldaddrRead = (int)oldl;
			if(start.compare_exchange_strong(oldaddrRead, (long)newl))
				return newl->newver;
#endif

						continue;break;
					case locatorOL::WRITET:
						Transaction* curWrite = oldl->getActive(me);
						if(curWrite!=NULL)
							cm->resolve(me, curWrite);
						continue;break;
					}
					break;
				}
				newl->clearOwner();
				newl->putMetoEnd(me, locatorOL::READT);
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

			}
	}
#ifdef USECDS
			MODEL_ASSERT(0);
#else
			assert(0);
#endif
			return NULL;
}

template<class T> T* FreeObjectRead<T>::openWrite(Transaction *me, ContentionManager *cm){
	switch(me->getStatus()){
		case Transaction::COMMIT: return ((locatorOL *)start.load())->newver;break;
		case Transaction::ABORT: throw AbortedException();break;
		case Transaction::ACTIVE:
			locatorOL * l = ((locatorOL *)start.load());
			if(l->isMeWrite(me))// writer may be in the reader list
				return l->newver;

			locatorOL * newl= new locatorOL();
			newl->clearOwner();
			newl->putMetoEnd(me, locatorOL::WRITET);
			bool isInterrupt = false;
			while(!isInterrupt){
				locatorOL *oldl = ((locatorOL *)start.load());
				switch(oldl->getTranStatus(me)){
				case Transaction::COMMIT	: newl->oldver = oldl->newver;break;
				case Transaction::ABORT		: newl->oldver = oldl->oldver;break;
				case Transaction::ACTIVE	:
					Transaction* curOwner = oldl->getActive(me);//If there are multiple owners, we abort them one by one.
					if(curOwner!=NULL)
						cm->resolve(me, curOwner);
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


			}
	}
#ifdef USECDS
			MODEL_ASSERT(0);
#else
			assert(0);
#endif
			return NULL;
}

template<class T> bool FreeObjectRead<T>::validate(Transaction* me){
	return me->getStatus()==Transaction::ACTIVE;
}

#endif /* SRC_FREEOBJECTREAD_H_ */
