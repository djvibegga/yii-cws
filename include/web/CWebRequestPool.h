/*
 * CWebApplicationPull.h
 *
 *  Created on: Feb 18, 2014
 *      Author: djvibegga
 */

#ifndef CWEBREQUESTLICATIONPOOL_H_
#define CWEBREQUESTLICATIONPOOL_H_

#include "defines.h"
#include "interfaces.h"
#include "web/CWebApplication.h"
#include "base/CApplicationPool.h"
#include <boost/thread/mutex.hpp>
#include <queue>

class CWebRequestPool: public CApplicationPool
{
protected:
	int listenSocket;
	useconds_t idleTimeout;

public:
	CWebRequestPool(const string &configPath, int argc, char * const argv[]);
	virtual ~CWebRequestPool();
	virtual void init() throw (CException);
	virtual void run() throw (CException);
	int getListenSocket() const;

protected:
	virtual void openSocket();
	virtual void mainLoop();
};

#endif /* CWEBREQUESTLICATIONPOOL_H_ */
