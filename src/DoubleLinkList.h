/*
 * DoubleLinkList.h
 *
 *  Created on: Dec 2, 2015
 *      Author: yuanfan
 */

#ifndef SRC_DOUBLELINKLIST_H_
#define SRC_DOUBLELINKLIST_H_

#ifdef USECDS
#include <model-assert.h>
#endif

#include <assert.h>

#include "TDLinkNode.h"
#include "SDLinkNode.h"

template<class T>
#ifdef USEBGC
class DoubleLinkList : public gc {
#else
class DoubleLinkList {
#endif
public:
	DoubleLinkList() {
		// TODO Auto-generated constructor stub
		head = NULL;
		tail = NULL;
		atype_ = 0;
	}
	virtual ~DoubleLinkList() {
		// TODO Auto-generated destructor stub
	}
	//void init(T minvar, T maxvar, TDLinkNode<T>::AtomicObjectType atype);
	void init(T minvar, T maxvar, int atype);
	bool add(Transaction *me, ContentionManager *cm, T item);
	bool remove(Transaction *me, ContentionManager *cm, T item);
	bool contain(Transaction *me, ContentionManager *cm, T item);
	void print(Transaction *me, ContentionManager *cm){};
private:
	bool check(Transaction *me, ContentionManager *cm, T item);
	bool findFromHead(Transaction *me, ContentionManager *cm, T item, DLinkNode<T> * &prev, DLinkNode<T> * &next);
	bool findFromEnd(Transaction *me, ContentionManager *cm, T item, DLinkNode<T> * &prev, DLinkNode<T> * &next);
	bool find(Transaction *me, ContentionManager *cm, T item, DLinkNode<T> * &prev, DLinkNode<T> * &next);
	TDLinkNode<T> * head, * tail;
	int atype_;
	T maxvar_;
	T minvar_;
};

template<>
void DoubleLinkList<int>::print(Transaction *me, ContentionManager *cm){
	TDLinkNode<int> * cur = head;
	int i=0;
	printf("%x, %x\n", (unsigned int)head, (unsigned int)tail);
	while(cur!=tail){
		printf("%d , next %x \n", cur->get(me, cm), (unsigned int)cur->get_next(me, cm));
		cur = (TDLinkNode<int> * )cur->get_next(me, cm);
		i++;
	}
	printf("%d , next %x \n", cur->get(me, cm), (unsigned int)cur->get_next(me, cm));
}

template<class T>
//void DoubleLinkList<T>::init(T minvar, T maxvar, TDLinkNode<T>::AtomicObjectType atype){
void DoubleLinkList<T>::init(T minvar, T maxvar,int atype){
	head = new TDLinkNode<T>();
	tail = new TDLinkNode<T>();
	head->init(minvar, (TDLinkNode<int>::AtomicObjectType)atype, NULL, tail);
	tail->init(maxvar, (TDLinkNode<int>::AtomicObjectType)atype, head, NULL);
	atype_ = atype;
	minvar_ = minvar;
	maxvar_ = maxvar;
}
template<class T>
bool DoubleLinkList<T>::check(Transaction *me, ContentionManager *cm, T item){
	if(minvar_>=item||maxvar_<=item){
		return false;
	}
	return true;
}
template<class T>
bool DoubleLinkList<T>::findFromHead(Transaction *me, ContentionManager *cm,
							 	 	 T item, DLinkNode<T> * &prev, DLinkNode<T> * &next){
	if(!check(me, cm, item)){
		printf("ERROR INSERT/REMOVE WRONG VAL");
		assert(0);
	}
	TDLinkNode<T> * cur = (TDLinkNode<T> *)head->get_next(me, cm);
	while(cur!=tail){
		if(cur->get(me, cm)<item){
			cur = (TDLinkNode<T> *)cur->get_next(me, cm);
		}else if(cur->get(me, cm)>item){
			prev = cur->get_prev(me, cm);
			next = cur;
			return false;
		}else{
			prev = cur->get_prev(me, cm);
			next = cur->get_next(me, cm);
			return true;
		}
	}
	prev = tail->get_prev(me, cm);
	next = tail;
	return false;
}

template<class T>
bool DoubleLinkList<T>::findFromEnd(Transaction *me, ContentionManager *cm,
							 	 	T item, DLinkNode<T> * &prev, DLinkNode<T> * &next){
	if(!check(me, cm, item)){
		printf("ERROR INSERT/REMOVE WRONG VAL");
		assert(0);
	}
	TDLinkNode<T> * cur =(TDLinkNode<T> *) tail->get_prev(me, cm);
	while(cur!=head){
		if(cur->get(me, cm)>item){
			cur =(TDLinkNode<T> *) cur->get_prev(me, cm);
		}else if(cur->get(me, cm)<item){
			prev = cur;
			next = cur->get_next(me, cm);
			return false;
		}else{
			prev = cur->get_prev(me, cm);
			next = cur->get_next(me, cm);
			return true;
		}
	}
	prev = head;
	next = head->get_next(me, cm);
	return false;
}
template<class T>
bool DoubleLinkList<T>::find(Transaction *me, ContentionManager *cm,
		 	 	 	 	 	 T item, DLinkNode<T> * &prev, DLinkNode<T> * &next){
	bool res;
	if(item>((head->get(me, cm)+tail->get(me, cm))/2)){
		//printf("item %d, from END\n", item);
		res = findFromEnd(me, cm, item, prev, next);
		//printf("item %d, from END done \n", item);
		return res;
	}else{
		//printf("item %d, from HEAD\n", item);
		res = findFromHead(me, cm, item, prev, next);
		//printf("item %d, from HEAD done \n", item);
		return res;
	}
}
template<class T>
bool DoubleLinkList<T>::add(Transaction *me, ContentionManager *cm, T item){
	DLinkNode<T> * prev = NULL, * next = NULL;
	TDLinkNode<T> * newNode = new TDLinkNode<T>();
	newNode->init(item, me, (TDLinkNode<int>::AtomicObjectType)atype_);
	if(!find(me, cm, item, prev, next)){
		((TDLinkNode<T> *)prev)->set_next(me, cm, newNode);
		//printf("%d done 1\n", item);
		((TDLinkNode<T> *)next)->set_prev(me, cm, newNode);
		//printf("%d done 2\n", item);
		newNode->set_prev_next(me, cm, prev, next);
		//printf("%d done 3\n", item);
		return true;
	}else{
		return false;
	}
}
template<class T>
bool DoubleLinkList<T>::remove(Transaction *me, ContentionManager *cm, T item){
	DLinkNode<T> * prev = NULL, * next = NULL;
	if(find(me, cm, item, prev, next)){
		((TDLinkNode<T> *)prev)->set_next(me, cm, next);
		((TDLinkNode<T> *)next)->set_prev(me, cm, prev);
		return true;
	}else{
		return false;
	}
}
template<class T>
bool DoubleLinkList<T>::contain(Transaction *me, ContentionManager *cm, T item){
	if(!check(me, cm, item)){
		return false;
	}
	TDLinkNode<T> * cur = (TDLinkNode<T> *)head->get_next(me, cm);
	while(cur != tail){
		if(cur->get(me, cm)==item){
			return true;
		}else{
			cur=(TDLinkNode<T> *)cur->get_next(me, cm);
		}
	}
	return false;
}

#endif /* SRC_DOUBLELINKLIST_H_ */
