/*
 * CWebApplication.h
 *
 *  Created on: Oct 16, 2013
 *      Author: djvibegga
 */

#ifndef CWEBAPPLICATION_H_
#define CWEBAPPLICATION_H_

#include "fcgi_stdio.h"
#include "interfaces.h"
#include "base/CException.h"
#include "base/CApplication.h"
#include "web/CHttpRequest.h"
#include "web/CHttpResponse.h"
#include "web/CWebModule.h"
#include "web/CUrlManager.h"
#include <web/CController.h>

struct SControllerToRun
{
	string actionId;
	CController * controller;
};

class CWebApplication: public CApplication
{
	friend class CHttpRequest;
	friend class CHttpResponse;

private:
	TControllerMap _controllerMap;

public:
	TRouteStruct catchAllRequest;
	string defaultControllerRoute;

	CWebApplication(const string &configPath, int argc, char * const argv[]);
	~CWebApplication();
	virtual void run() throw(CException);
	CHttpRequest * getRequest() const;
	CHttpResponse * getResponse() const;
	CUrlManager * getUrlManager() const;

	virtual bool hasController(const string & name) const;
	virtual CController * getController(const string & name) const;
	virtual void setController(const string & name, CController * instance);
	virtual CModule * getParent() const;

protected:
	FCGX_Request request;
	void mainLoop() throw(CException);
	virtual void handleRequest();
	virtual void beginRequest();
	virtual void processRequest();
	virtual void endRequest();
	virtual void registerComponents();

	virtual void runController(const string &route);
	virtual SControllerToRun resolveController(string route, const IModule * owner);

	virtual void renderException(const CException & e) const;
};

#endif /* CWEBAPPLICATION_H_ */
