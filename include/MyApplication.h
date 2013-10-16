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
	MyApplication(const string &configPath);

protected:
	virtual void processRequest() throw(CException);
};

#endif /* MYAPPLICATION_H_ */
