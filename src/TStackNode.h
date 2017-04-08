/*
 * TStackNode.h
 *
 *  Created on: Nov 25, 2015
 *      Author: yuanfan
 */

#ifndef SRC_TSTACKNODE_H_
#define SRC_TSTACKNODE_H_

#include "Copyable.h"
#include "StackNode.h"
#include "SStackNode.h"
#include"Transaction.h"
#include"ContentionManager.h"
#include "AtomicObject.h"
#include "FreeObject.h"
#include "FreeObjectRead.h"
#include "AbortedException.h"
template<class T>
class TStackNode: public StackNode<T> {
public:
	enum AtomicObjectType{FREEOBJ, FREEOBJREAD};
	TStackNode();
	virtual ~TStackNode();
	void init(T var);
	void init(T var, AtomicObjectType atype);
	void set(Transaction *me, ContentionManager *cm, T var);
	T get(Transaction *me, ContentionManager *cm);
private:
	AtomicObject<SStackNode<T> >* at_;
};

template<class T> TStackNode<T>::TStackNode() {
	// TODO Auto-generated constructor stub
	at_ = NULL;
}

template<class T> TStackNode<T>::~TStackNode() {
	// TODO Auto-generated destructor stub
}

template<class T> void TStackNode<T>::init(T var){
	at_ = new  FreeObject<SStackNode<T> >(new SStackNode<T>(var));
}

template<class T> void TStackNode<T>::init(T var, AtomicObjectType atype){
	switch(atype){
	case	FREEOBJ		:
		at_ = new  FreeObject<SStackNode<T> >(new SStackNode<T>(var));break;
	case	FREEOBJREAD	:
		at_ = new  FreeObjectRead<SStackNode<T> >(new SStackNode<T>(var));break;
	}
}

template<class T> T TStackNode<T>::get(Transaction* me,
		  	  	  	  	  	  	  	   ContentionManager* cm){
	T foward= ((SStackNode<T> *)(at_->template openRead(me, cm)))->get();
	if(!at_ ->template validate(me))
		throw AbortedException();
	return foward;
}

template<class T> void TStackNode<T>::set(Transaction *me,
										  ContentionManager *cm,
										  T var){
	((SStackNode<T> *)(at_->template openWrite(me, cm)))->set(var);
}

#endif /* SRC_TSTACKNODE_H_ */
