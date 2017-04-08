/*
 * AbortedException.cpp
 *
 *  Created on: Nov 18, 2015
 *      Author: yuanfan
 */

#include "AbortedException.h"

AbortedException::AbortedException() {
	// TODO Auto-generated constructor stub
	clock_gettime(CLOCK_REALTIME , &aborttime_);
}

AbortedException::~AbortedException() {
	// TODO Auto-generated destructor stub
}

