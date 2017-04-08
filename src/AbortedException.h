/*
 * AbortedException.h
 *
 *  Created on: Nov 18, 2015
 *      Author: yuanfan
 */

#ifndef ABORTEDEXCEPTION_H_
#define ABORTEDEXCEPTION_H_

#include <sys/time.h>
#include <time.h>
#ifdef USEBGF
#include<gc_cpp.h>
class AbortedException : public gc {
#else
class AbortedException{
#endif
public:
	AbortedException();
	//AbortedException(timeval aborttime);
	timespec getaborttime(){return aborttime_;};
	virtual ~AbortedException();
private:
	timespec aborttime_;
};

#endif /* ABORTEDEXCEPTION_H_ */
