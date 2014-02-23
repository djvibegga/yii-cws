/*
 * TestWebAppPool.h
 *
 *  Created on: Feb 18, 2014
 *      Author: djvibegga
 */

#ifndef TESTWEBAPPPOOL_H_
#define TESTWEBAPPPOOL_H_

#include "web/CWebRequestPool.h"

class TestWebRequestPool: public CWebRequestPool
{
public:
	TestWebRequestPool(const string &configPath, int argc, char * const argv[]);

protected:
	virtual CWebApplication * createAppInstance() const;
};

#endif /* TESTWEBAPPPOOL_H_ */
