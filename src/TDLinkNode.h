/*
 * TDLinkNode.h
 *
 *  Created on: Dec 2, 2015
 *      Author: yuanfan
 */

#ifndef SRC_TDLINKNODE_H_
#define SRC_TDLINKNODE_H_

#include "DLinkNode.h"
#include "SDLinkNode.h"
#include "AtomicObject.h"
#include "FreeObject.h"
#include "FreeObjectRead.h"

template<class T>
class TDLinkNode: public DLinkNode<T> {
public:
	enum AtomicObjectType{FREEOBJ, FREEOBJREAD};
	TDLinkNode() {
		// TODO Auto-generated constructor stub
		at_ = NULL;
	}
	virtual ~TDLinkNode() {
		// TODO Auto-generated destructor stub
	}
	void init(T var);
	void init(T var, AtomicObjectType atype);
	void init(T var, Transaction *me, AtomicObjectType atype);
	void init(T var, AtomicObjectType atype, DLinkNode<T> * prev, DLinkNode<T> * next);
	virtual T get(Transaction *me, ContentionManager *cm);
	virtual void set(Transaction *me, ContentionManager *cm, T var);
	virtual void set_prev_next(Transaction *me, ContentionManager *cm,
							   DLinkNode<T> * prev, DLinkNode<T> * next);
	virtual void set_prev(Transaction *me, ContentionManager *cm,
						  DLinkNode<T> * prev);
	virtual void set_next(Transaction *me, ContentionManager *cm,
						  DLinkNode<T> * next);
	virtual DLinkNode<T> * get_next(Transaction *me, ContentionManager *cm);
	virtual DLinkNode<T> * get_prev(Transaction *me, ContentionManager *cm);
private:
	AtomicObject<SDLinkNode<T> >* at_;

};

template<class T> void TDLinkNode<T>::init(T var){
	init(var, FREEOBJ);
}
template<class T> void TDLinkNode<T>::init(T var, AtomicObjectType atype){
	init(var, FREEOBJ, NULL, NULL);
}
template<class T> void TDLinkNode<T>::init(T var,
										   AtomicObjectType atype,
										   DLinkNode<T> * prev,
										   DLinkNode<T> * next){
	switch(atype){
	case	FREEOBJ		:
		at_ = new  FreeObject<SDLinkNode<T> >(new SDLinkNode<T>(var, prev, next));break;
	case	FREEOBJREAD	:
		at_ = new  FreeObjectRead<SDLinkNode<T> >(new SDLinkNode<T>(var, prev, next));break;
	}
}

template<class T> void TDLinkNode<T>::init(T var, Transaction *me,
										   AtomicObjectType atype){
	switch(atype){
	case	FREEOBJ		:
		at_ = new  FreeObject<SDLinkNode<T> >(new SDLinkNode<T>(var, NULL, NULL), me);break;
	case	FREEOBJREAD	:
		at_ = new  FreeObjectRead<SDLinkNode<T> >(new SDLinkNode<T>(var, NULL, NULL), me);break;
	}
}

template<class T> T TDLinkNode<T>::get(Transaction *me,
									   ContentionManager *cm){
	T foward= ((SDLinkNode<T> *)(at_->template openRead(me, cm)))->get();
	if(!at_ ->template validate(me))
		throw AbortedException();
	return foward;
}

template<class T> DLinkNode<T> * TDLinkNode<T>::get_prev(Transaction *me,
									   	   	   	   	      ContentionManager *cm){
	DLinkNode<T> * foward= ((SDLinkNode<T> *)(at_->template openRead(me, cm)))->get_prev();
	if(!at_ ->template validate(me))
		throw AbortedException();
	return foward;
}

template<class T> DLinkNode<T> * TDLinkNode<T>::get_next(Transaction *me,
									   	   	   	   	      ContentionManager *cm){
	DLinkNode<T> * foward= ((SDLinkNode<T> *)(at_->template openRead(me, cm)))->get_next();
	if(!at_ ->template validate(me))
		throw AbortedException();
	return foward;
}

template<class T> void TDLinkNode<T>::set(Transaction *me,
										  ContentionManager *cm,
										  T var){
	((SDLinkNode<T> *)(at_->template openWrite(me, cm)))->set(var);
}

template<class T>  void TDLinkNode<T>::set_next(Transaction *me,
												ContentionManager *cm,
						  	  	  	  	  	  	DLinkNode<T> * next){
	((SDLinkNode<T> *)(at_->template openWrite(me, cm)))->set_next(next);
}

template<class T>  void TDLinkNode<T>::set_prev(Transaction *me,
												ContentionManager *cm,
						  	  	  	  	  	  	DLinkNode<T> * prev){
	((SDLinkNode<T> *)(at_->template openWrite(me, cm)))->set_prev(prev);
}

template<class T>  void TDLinkNode<T>::set_prev_next(Transaction *me,
												ContentionManager *cm,
						  	  	  	  	  	  	DLinkNode<T> * prev,
												DLinkNode<T> * next){
	((SDLinkNode<T> *)(at_->template openWrite(me, cm)))->set_prev_next(prev, next);
}



#endif /* SRC_TDLINKNODE_H_ */
