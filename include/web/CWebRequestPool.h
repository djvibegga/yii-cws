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

class CWebRequestPool: public CApplicationPool, public IWebRequestPool
{
private:
	boost::mutex _queueLocker;
	queue<FCGX_Request*> _requests;

protected:
	int listenSocket;

public:
	CWebRequestPool(const string &configPath, int argc, char * const argv[]);
	virtual ~CWebRequestPool();
	void run() throw (CException);
	virtual FCGX_Request * popRequest() throw (boost::lock_error);

protected:
	virtual void mainLoop() throw (CException);
	virtual void openSocket();
};

#endif /* CWEBREQUESTLICATIONPOOL_H_ */
