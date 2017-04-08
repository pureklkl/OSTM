/*
 * DLinkNode.h
 *
 *  Created on: Dec 2, 2015
 *      Author: yuanfan
 */

#ifndef SRC_DLINKNODE_H_
#define SRC_DLINKNODE_H_
#ifdef USEBGC
#include<gc_cpp.h>
#endif
template<class T>
#ifdef USEBGC
class DLinkNode : public gc {
#else
class DLinkNode {
#endif
public:
	virtual ~DLinkNode() {};
};

#endif /* SRC_DLINKNODE_H_ */
