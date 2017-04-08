/*
 * SStackNode.h
 *
 *  Created on: Nov 25, 2015
 *      Author: yuanfan
 */

#ifndef SRC_SSTACKNODE_H_
#define SRC_SSTACKNODE_H_

#include "StackNode.h"
#include "Copyable.h"

template<class T>
class SStackNode: public StackNode<T>, public Copyable<SStackNode<T> > {
public:
	SStackNode();
	SStackNode(T var);
	virtual ~SStackNode();
	void set(T var);
	T get();
	void copyTo(SStackNode<T>* target);
private:
	T var_;
};

template<class T> SStackNode<T>::SStackNode() {
	// TODO Auto-generated constructor stub
}

template<class T> SStackNode<T>::SStackNode(T var){
	var_ = var;
}

template<class T> SStackNode<T>::~SStackNode() {
	// TODO Auto-generated destructor stub
}

template<class T> void SStackNode<T>::copyTo(SStackNode<T>* target){
	target->var_ = this->var_;
}

template<class T> void SStackNode<T>::set(T var){
	var_ = var;
}

template<class T> T SStackNode<T>::get(){
	return var_;
}

#endif /* SRC_SSTACKNODE_H_ */
