/*
 * TestWebRequestPool.cpp
 *
 *  Created on: Feb 18, 2014
 *      Author: djvibegga
 */

#include "TestWebRequestPool.h"
#include "MyApplication.h"

TestWebRequestPool::TestWebRequestPool(const string &configPath, int argc, char * const argv[])
: CWebRequestPool(configPath, argc, argv)
{
}

CWebApplication * TestWebRequestPool::createAppInstance() const
{
	return new MyApplication(getConfigDocument(), argc, argv);
}
