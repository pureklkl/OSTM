/*
 * ContentionManager.h
 *
 *  Created on: Nov 23, 2015
 *      Author: yuanfan
 */

#ifndef SRC_CONTENTIONMANAGER_H_
#define SRC_CONTENTIONMANAGER_H_

#include"Transaction.h"
#ifdef USBGC
#include<gc_cpp.h>
class ContentionManager : public gc {
#else
class ContentionManager{
#endif
	public:
		virtual void resolve(Transaction *me, Transaction *other)=0;
		virtual ~ContentionManager(){};
};

#endif /* SRC_CONTENTIONMANAGER_H_ */
