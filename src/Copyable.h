/*
 * Copyable.h
 *
 *  Created on: Nov 23, 2015
 *      Author: yuanfan
 */

#ifndef SRC_COPYABLE_H_
#define SRC_COPYABLE_H_
template<class T>
class Copyable {
public:
	virtual void copyTo(T* target)=0;
	virtual ~Copyable(){};
};

#endif /* SRC_COPYABLE_H_ */
