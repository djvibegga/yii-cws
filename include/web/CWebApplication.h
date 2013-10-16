/*
 * CWebApplication.h
 *
 *  Created on: Oct 16, 2013
 *      Author: djvibegga
 */

#ifndef CWEBAPPLICATION_H_
#define CWEBAPPLICATION_H_

#include "fcgi_stdio.h"
#include "base/CException.h"
#include "base/CApplication.h"

class CWebApplication: public CApplication
{
public:
	CWebApplication(const string &configPath);
	~CWebApplication();
	virtual void run() throw(CException);
	void echo(const string & content);

protected:
	FCGX_Request request;
	void mainLoop() throw(CException);
};

#endif /* CWEBAPPLICATION_H_ */
