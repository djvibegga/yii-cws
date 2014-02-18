/*
 * CWebApplicationPull.h
 *
 *  Created on: Feb 18, 2014
 *      Author: djvibegga
 */

#ifndef CWEBREQUESTLICATIONPOOL_H_
#define CWEBREQUESTLICATIONPOOL_H_

#include "defines.h"
#include "fcgi_stdio.h"
#include "interfaces.h"
#include "CWebApplication.h"
#include <boost/thread/mutex.hpp>
#include <queue>

class CWebRequestPool: public IWebRequestPool
{
private:
	boost::mutex _queueLocker;
	queue<FCGX_Request*> _requests;
	xml_document * _xmlConfig;
	void _runInstance(CWebApplication & instance);

protected:
	string configPath;
	int argc;
	char * const * argv;

public:
	CWebRequestPool(const string &configPath, int argc, char * const argv[]);
	virtual ~CWebRequestPool();
	virtual void init() throw(CException);
	virtual void run() throw(CException);
	virtual FCGX_Request * popRequest();
	const xml_document & getConfigDocument();

protected:
	virtual CWebApplication * createAppInstance() = 0;
};

#endif /* CWEBREQUESTLICATIONPOOL_H_ */
