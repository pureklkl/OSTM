/*
 * SDoubleLinkList.h
 *
 *  Created on: Dec 3, 2015
 *      Author: yuanfan
 */

#ifndef SRC_SDOUBLELINKLIST_H_
#define SRC_SDOUBLELINKLIST_H_
#include "SDLinkNode.h"
#include <assert.h>
#include <stdio.h>
#include <pthread.h>
template<class T>
#ifdef USEBGC
class SDoubleLinkList : public gc {
#else
class SDoubleLinkList {
#endif
public:
	SDoubleLinkList(){
		// TODO Auto-generated constructor stub
		head = NULL;
		tail = NULL;
	}
	virtual ~SDoubleLinkList(){};
	void init(T minvar, T maxvar);
	bool add(T item);
	bool remove(T item);
	bool contain(T item);
	void print(){};
private:
	bool check(T item);
	bool findFromHead(T item, DLinkNode<T> * &prev, DLinkNode<T> * &next);
	bool findFromEnd(T item, DLinkNode<T> * &prev, DLinkNode<T> * &next);
	bool find(T item, DLinkNode<T> * &prev, DLinkNode<T> * &next);
	void lock(){pthread_mutex_lock(&mutexlock_);};
	void unlock(){pthread_mutex_unlock(&mutexlock_);};
	SDLinkNode<T> * head, * tail;
	T maxvar_;
	T minvar_;
	pthread_mutex_t mutexlock_;
};
template<>
void SDoubleLinkList<int>::print(){
	lock();
	SDLinkNode<int> * cur = head;
	int i=0;
	printf("%x, %x\n", (unsigned int)head, (unsigned int)tail);
	while(cur!=tail){
		printf("%d , next %x \n", cur->get(), (unsigned int)cur->get_next());
		cur = (SDLinkNode<int> * )cur->get_next();
		i++;
	}
	printf("%d , next %x \n", cur->get(), (unsigned int)cur->get_next());
	unlock();
}
template<class T>
//void DoubleLinkList<T>::init(T minvar, T maxvar, TDLinkNode<T>::AtomicObjectType atype){
void SDoubleLinkList<T>::init(T minvar, T maxvar){
	head = new SDLinkNode<T>();
	tail = new SDLinkNode<T>();
	head->init(minvar, NULL, tail);
	tail->init(maxvar, head, NULL);
	minvar_ = minvar;
	maxvar_ = maxvar;
	mutexlock_ = PTHREAD_MUTEX_INITIALIZER;
}

template<class T>
bool SDoubleLinkList<T>::check(T item){
	if(minvar_>=item||maxvar_<=item){
		return false;
	}
	return true;
}

template<class T>
bool SDoubleLinkList<T>::findFromHead(T item, DLinkNode<T> * &prev, DLinkNode<T> * &next){
	if(!check(item)){
		printf("ERROR INSERT/REMOVE WRONG VAL");
		assert(0);
	}
	SDLinkNode<T> * cur = (SDLinkNode<T> *)head->get_next();
	while(cur!=tail){
		if(cur->get()<item){
			cur = (SDLinkNode<T> *)cur->get_next();
		}else if(cur->get()>item){
			prev = cur->get_prev();
			next = cur;
			return false;
		}else{
			prev = cur->get_prev();
			next = cur->get_next();
			return true;
		}
	}
	prev = tail->get_prev();
	next = tail;
	return false;
}

template<class T>
bool SDoubleLinkList<T>::findFromEnd(T item, DLinkNode<T> * &prev, DLinkNode<T> * &next){
	if(!check(item)){
		printf("ERROR INSERT/REMOVE WRONG VAL");
		assert(0);
	}
	SDLinkNode<T> * cur =(SDLinkNode<T> *) tail->get_prev();
	while(cur!=head){
		if(cur->get()>item){
			cur =(SDLinkNode<T> *) cur->get_prev();
		}else if(cur->get()<item){
			prev = cur;
			next = cur->get_next();
			return false;
		}else{
			prev = cur->get_prev();
			next = cur->get_next();
			return true;
		}
	}
	prev = head;
	next = head->get_next();
	return false;
}

template<class T>
bool SDoubleLinkList<T>::find(T item, DLinkNode<T> * &prev, DLinkNode<T> * &next){
	bool res;
	if(item>((head->get()+tail->get())/2)){
		//printf("item %d, from END\n", item);
		res = findFromEnd(item, prev, next);
		//printf("item %d, from END done \n", item);
		return res;
	}else{
		//printf("item %d, from HEAD\n", item);
		res = findFromHead(item, prev, next);
		//printf("item %d, from HEAD done \n", item);
		return res;
	}
}

template<class T>
bool SDoubleLinkList<T>::add(T item){
	DLinkNode<T> * prev = NULL, * next = NULL;
	SDLinkNode<T> * newNode = new SDLinkNode<T>();
	newNode->init(item);
	lock();
	if(!find(item, prev, next)){
		((SDLinkNode<T> *)prev)->set_next(newNode);
		//printf("%d done 1\n", item);
		((SDLinkNode<T> *)next)->set_prev(newNode);
		//printf("%d done 2\n", item);
		newNode->set_prev_next(prev, next);
		//printf("%d done 3\n", item);
		unlock();
		return true;
	}else{
		unlock();
		return false;
	}
}

template<class T>
bool SDoubleLinkList<T>::remove(T item){
	DLinkNode<T> * prev = NULL, * next = NULL;
	lock();
	if(find(item, prev, next)){
		((SDLinkNode<T> *)prev)->set_next(next);
		((SDLinkNode<T> *)next)->set_prev(prev);
		unlock();
		return true;
	}else{
		unlock();
		return false;
	}
}

template<class T>
bool SDoubleLinkList<T>::contain(T item){
	if(!check(item)){
		return false;
	}
	lock();
	SDLinkNode<T> * cur = (SDLinkNode<T> *)head->get_next();
	while(cur != tail){
		if(cur->get()==item){
			unlock();
			return true;
		}else{
			cur=(SDLinkNode<T> *)cur->get_next();
		}
	}
	unlock();
	return false;
}

#endif /* SRC_SDOUBLELINKLIST_H_ */
