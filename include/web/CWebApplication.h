/*
 * CWebApplication.h
 *
 *  Created on: Oct 16, 2013
 *      Author: djvibegga
 */

#ifndef CWEBAPPLICATION_H_
#define CWEBAPPLICATION_H_

#include "interfaces.h"
#include "base/CException.h"
#include "base/CApplication.h"
#include "web/CHttpRequest.h"
#include "web/CHttpResponse.h"
#include "web/CWebModule.h"
#include "web/CUrlManager.h"
#include "web/CClientScript.h"
#include "web/CAssetManager.h"
#include "web/CController.h"
#include "web/CHttpSession.h"
#include "web/CWebUser.h"

class CWebApplication;

struct SControllerToRun
{
	string actionId;
	CController * controller;
};

class CWebRequestPool;

class CWebApplication: public CApplication, public IHasLayout
{
	friend class CHttpRequest;
	friend class CHttpResponse;

private:
	CWebRequestPool * _requestPool;
	TControllerMap _controllerMap;
	boost::filesystem::path _layoutPath;

public:
	TRouteStruct catchAllRequest;
	string defaultControllerRoute;
	bool enableSessions;
	bool enableAuth;

	CWebApplication(const string &configPath, int argc, char * const argv[]);
	CWebApplication(const xml_document & configDocument, int argc, char * const argv[]);
	virtual string getClassName() const;
	~CWebApplication();
	virtual void init() throw(CException);
	virtual void run() throw(CException);
	CHttpRequest * getRequest() const;
	CHttpResponse * getResponse();
	CUrlManager * getUrlManager() const;
	CAssetManager * getAssetManager() const;
	CClientScript * getClientScript() const;
	CHttpSession * getSession() const;

	virtual bool hasController(const string & name) const;
	virtual CController * getController(const string & name) const;
	virtual void setController(const string & name, CController * instance);
	virtual CModule * getParent() const;
	virtual boost::filesystem::path getLayoutPath() const;
	virtual void setLayoutPath(const boost::filesystem::path & path);
	double getIdleMedian() const;

protected:
	FCGX_Request * request;
	useconds_t idleTime;
	double idleMedian;

	virtual void applyConfig(const xml_node & config);
	void mainLoop() throw(CException);
	virtual void handleRequest();
	virtual void beginRequest();
	virtual void processRequest();
	virtual void endRequest();
	virtual void registerComponents();
	virtual boost::filesystem::path resolveLayoutPath();
	virtual void runController(const string &route);
	virtual SControllerToRun resolveController(string route, const IModule * owner);
	virtual void renderException(const CException & e);

	virtual CHttpRequest * createHttpRequest();
	virtual CHttpResponse * createHttpResponse();
	virtual CHttpSession * createHttpSession();
	virtual CWebUser * createWebUser();
	virtual CUrlManager * createUrlManager();
};

#endif /* CWEBAPPLICATION_H_ */
