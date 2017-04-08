/*
 * SDLinkNode.h
 *
 *  Created on: Dec 2, 2015
 *      Author: yuanfan
 */

#ifndef SRC_SDLINKNODE_H_
#define SRC_SDLINKNODE_H_

#include "DLinkNode.h"
#include "Copyable.h"
template<class T>
class SDLinkNode: public DLinkNode<T> , Copyable<SDLinkNode<T> >{
public:
	SDLinkNode() {
		// TODO Auto-generated constructor stub
		next_ = NULL;
		prev_ = NULL;
	}
	SDLinkNode(T var, DLinkNode<T> * prev, DLinkNode<T> * next) {
		// TODO Auto-generated constructor stub
		var_ = var;
		next_ = next;
		prev_ = prev;
	}
	virtual ~SDLinkNode() {
		// TODO Auto-generated destructor stub
	}
	void init(T var);
	void init(T var, DLinkNode<T> * prev, DLinkNode<T> * next);
	virtual T get(){return var_;};
	virtual void set(T var){var_ = var;};
	virtual void set_prev_next(DLinkNode<T> * prev, DLinkNode<T> * next){prev_ = prev; next_ = next;};
	virtual void set_prev(DLinkNode<T> * prev){prev_ = prev;};
	virtual void set_next(DLinkNode<T> * next){next_ = next;};
	virtual DLinkNode<T> * get_next(){return next_;};
	virtual DLinkNode<T> * get_prev(){return prev_;};
	virtual void copyTo(SDLinkNode<T> * target){target->set(var_);
												target->set_prev_next(prev_, next_);}
private:
	DLinkNode<T> * prev_;
	DLinkNode<T> * next_;
	T var_;
};

template<class T>
void SDLinkNode<T>::init(T var){
	init(var, NULL, NULL);
}
template<class T>
void SDLinkNode<T>::init(T var, DLinkNode<T> * prev, DLinkNode<T> * next){
	var_ = var;
	prev_ = prev;
	next_ = next;
}

#endif /* SRC_SDLINKNODE_H_ */
