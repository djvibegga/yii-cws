/*
 * MyApplication.h
 *
 *  Created on: Oct 15, 2013
 *      Author: djvibegga
 */

#ifndef MYAPPLICATION_H_
#define MYAPPLICATION_H_

#include "web/CWebApplication.h"

class MyApplication: public CWebApplication
{
public:
	MyApplication(const string &configPath, int argc, char * const argv[]);
	virtual ~MyApplication();

protected:
	void registerComponents();
};

#endif /* MYAPPLICATION_H_ */
