/*
 * AtomicObject.h
 *
 *  Created on: Nov 23, 2015
 *      Author: yuanfan
 */

#ifndef SRC_ATOMICOBJECT_H_
#define SRC_ATOMICOBJECT_H_
#ifdef USEBGC
#include<gc_cpp.h>
#endif
#include"Copyable.h"
#include"Transaction.h"
#include"ContentionManager.h"
template<class T>
#ifdef USEBGC
class AtomicObject : public gc {
#else
class AtomicObject {
#endif
protected:
	T* internalinit;
public:
	AtomicObject():internalinit(0){}
	AtomicObject(T* init){
		            (void)static_cast<Copyable<T> *>((T *)0);
#ifdef USEBGC
		            (void)static_cast<gc *>((T *)0);
#endif
		            internalinit = init;}
	virtual	T* openRead(Transaction* me, ContentionManager * cm)=0;
	virtual T* openWrite(Transaction * me, ContentionManager * cm)=0;
	virtual bool validate(Transaction * me)=0;
	virtual ~AtomicObject(){};
};

#endif /* SRC_ATOMICOBJECT_H_ */
