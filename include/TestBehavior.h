/*
 * TestBehavior.h
 *
 *  Created on: Oct 25, 2013
 *      Author: djvibegga
 */

#ifndef TESTBEHAVIOR_H_
#define TESTBEHAVIOR_H_

#include <base/CBehavior.h>

class TestBehavior: public CBehavior
{
public:
	virtual TEventsMap events();
	void logRequest(CEvent & event);
};

#endif /* TESTBEHAVIOR_H_ */
