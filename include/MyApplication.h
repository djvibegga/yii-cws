/*
 * MyApplication.h
 *
 *  Created on: Oct 15, 2013
 *      Author: djvibegga
 */

#ifndef MYAPPLICATION_H_
#define MYAPPLICATION_H_

#include "base/CApplication.h"

class MyApplication: public CApplication
{
public:
	MyApplication(const string &configPath);

protected:
	void processRequest(const FCGX_Request &request);
};

#endif /* MYAPPLICATION_H_ */
