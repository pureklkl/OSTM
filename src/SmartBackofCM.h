/*
 * SmartBackofCM.h
 *
 *  Created on: Dec 2, 2015
 *      Author: yuanfan
 */

#ifndef SRC_SMARTBACKOFCM_H_
#define SRC_SMARTBACKOFCM_H_

#include "BackofManager.h"
#include <sys/time.h>
#include <time.h>
#include <stdio.h>
class SmartBackofCM: public BackofManager {
public:
	SmartBackofCM() {
		// TODO Auto-generated constructor stub
	}
	virtual ~SmartBackofCM() {
		// TODO Auto-generated destructor stub
	}
	void resolve(Transaction* me, Transaction* other);
};

#endif /* SRC_SMARTBACKOFCM_H_ */
