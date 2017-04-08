/*
 * StackNode.h
 *
 *  Created on: Nov 25, 2015
 *      Author: yuanfan
 */

#ifndef SRC_STACKNODE_H_
#define SRC_STACKNODE_H_

#ifdef USBGC
#include<gc_cpp.h>
#endif

template<class T>
#ifdef USBGC
class StackNode : public gc {
#else
class StackNode{
#endif
public:
	virtual ~StackNode(){};
};

#endif /* SRC_STACKNODE_H_ */
