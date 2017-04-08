/*
 * Transaction.h
 *
 *  Created on: Nov 18, 2015
 *      Author: yuanfan
 */

#ifndef SRC_TRANSACTION_H_
#define SRC_TRANSACTION_H_
#include <sys/time.h>
#include<atomic>
//#include<map>
#ifdef USEBGC
#include<gc_cpp.h>
class Transaction : public gc {
#else
class Transaction{
#endif
public:
	enum Stat{ACTIVE, COMMIT, ABORT};
	typedef int Status;
	Transaction();
	virtual ~Transaction();

	Status getStatus();

	virtual void init();
	virtual void init(__time_t mintime);
	bool abort();
	bool commit();
	void addsleeptime(int t){sleeptime+=t;};
	int getsleeptime(){return sleeptime;};
	__time_t getminruntime(){return mintime_;};
	timespec getstarttime(){return starttime_;};
	void setstarttime(timespec starttime){starttime_ = starttime;};
	void setpredictabort(){abortByPredict = true;};
	bool getabortstatus(){return abortByPredict;};
	//static void setLocal(Transaction &t);
	//static void getLocal();
	//std::map<int, Transaction> localtransaction;
private:
	timespec starttime_;
	__time_t mintime_;
	std::atomic_int status;
	bool abortByPredict;
	int sleeptime;
};

#endif /* SRC_TRANSACTION_H_ */
