/*
 * CWebApplication.cpp
 *
 *  Created on: Oct 16, 2013
 *      Author: djvibegga
 */

#include "web/CWebApplication.h"
#include "fcgi_stdio.h"
#include "web/CController.h"
#include "base/CHttpException.h"
#include "base/CStringUtils.h"
#include "base/CProfiler.h"
#include <sstream>
#include <iostream>
#include <algorithm>
#include <boost/regex.hpp>
#include <config.h>

using namespace boost;
using namespace std;

CWebApplication::CWebApplication(const string &configPath, int argc, char * const argv[])
: CApplication(configPath, argc, argv),
  _requestPool(0),
  enableSessions(true),
  request(0),
  idleTimeout(100000),
  idleTime(0),
  idleMedian(0)
{
}

CWebApplication::CWebApplication(const xml_document & configDocument, int argc, char * const argv[])
: CApplication(configDocument, argc, argv),
  _requestPool(0),
  enableSessions(true),
  request(0),
  idleTimeout(100000),
  idleTime(0),
  idleMedian(0)
{
}

CWebApplication::~CWebApplication()
{

}

void CWebApplication::registerComponents()
{
}

void CWebApplication::init() throw(CException)
{
	CApplication::init();
	createUrlManager();
	if (enableSessions) {
		createHttpSession();
	}
}

void CWebApplication::applyConfig(const xml_node & config)
{
	CApplication::applyConfig(config);

	idleTimeout = getConfigRoot()
		.child("server")
		.child("instance")
		.child("idleTimeout")
		.attribute("value").as_uint();
	_layoutPath = resolveLayoutPath();
}

void CWebApplication::run() throw(CException)
{
	CApplication::run();
	mainLoop();
}

void CWebApplication::renderException(const CException & e) const
{
	CHttpResponse * response = getResponse();
#ifdef JV_DEBUG
	string message = e.getFullMessage();
#else
	string message = e.getMessage();
#endif
	response->echo(utf8_to_(message));
}

void CWebApplication::mainLoop() throw(CException)
{
	IWebRequestPool * pool = getWebRequestPool();
	if (pool) {
		while (true) {
			request = pool->popRequest();
			if (request == 0) {
				usleep(idleTimeout);
				idleTime += idleTimeout;
				idleMedian = (double)idleTime / 1000000 / (double)(time(0) - startTime);
			} else {
				handleRequest();
				FCGX_Finish_r(request);
				delete request;
				request = 0;
			}
		}
	}
}

void CWebApplication::handleRequest()
{
	PROFILE_BEGIN("CWebApplication::handleRequest()");
	try {
		CApplication::handleRequest();
	} catch (const CHttpException & e) {
		FCGX_SetExitStatus(e.getStatus(), request->out);
		renderException(e);
	} catch (const CException & e) {
		renderException(e);
	}
	PROFILE_END();
}

void CWebApplication::beginRequest()
{
	CApplication::beginRequest();
	createHttpRequest();
	if (enableSessions) {
		CHttpSession * session = dynamic_cast<CHttpSession*>(getComponent("session"));
		if (session->autoOpen) {
			session->open();
		}
	}
	getOutputStack().push(createHttpResponse());
}

void CWebApplication::processRequest()
{
	if (!catchAllRequest.path.empty()) {
		CHttpRequest * request = getRequest();
		string route = catchAllRequest.path;
		for (TRequestParams::iterator iter = ++(catchAllRequest.params.begin()); iter != catchAllRequest.params.end(); ++iter) {
			request->setParam(iter->first, iter->second);
		}
	} else {
		string route = getUrlManager()->parseUrl(getRequest());
		runController(route);
	}
}

void CWebApplication::endRequest()
{
	while (!getOutputStack().empty()) {
		getOutputStack().pop();
	}
	if (enableSessions) {
		dynamic_cast<CHttpSession*>(getComponent("session"))->close();
	}
	delete getComponent("request");
	delete getComponent("response");
	CApplication::endRequest();
}

CHttpRequest * CWebApplication::getRequest() const
{
	return dynamic_cast<CHttpRequest*>(getComponent("request"));
}

CHttpResponse * CWebApplication::getResponse() const
{
	return dynamic_cast<CHttpResponse*>(getComponent("response"));
}

CUrlManager * CWebApplication::getUrlManager() const
{
	return dynamic_cast<CUrlManager*>(getComponent("urlManager"));
}

bool CWebApplication::hasController(const string & name) const
{
	return _controllerMap.find(name) != _controllerMap.end();
}

CController * CWebApplication::getController(const string & name) const
{
	return _controllerMap.find(name)->second;
}

void CWebApplication::setController(const string & name, CController * instance)
{
	_controllerMap[name] = instance;
}

CModule * CWebApplication::getParent() const
{
	return 0;
}

boost::filesystem::path CWebApplication::getLayoutPath() const
{
	return _layoutPath;
}

void CWebApplication::setLayoutPath(const boost::filesystem::path & path)
{
	_layoutPath = path;
}

double CWebApplication::getIdleMedian() const
{
	return idleMedian;
}

boost::filesystem::path CWebApplication::resolveLayoutPath()
{
	return boost::filesystem::path(getViewPath().normalize().string() + "/layouts");
}

void CWebApplication::runController(const string &route)
{
	PROFILE_BEGIN("CWebApplication::runController(" + route + ")");
	SControllerToRun ca = resolveController(route, 0);
	if (ca.actionId.empty()) {
		throw CHttpException(404, string("Unable to resolve the request."));
	} else {
		CController * controller = ca.controller;
		CController * oldController = dynamic_cast<CController*>(getComponent("controller"));
		setComponent("controller", controller);
		controller->run(ca.actionId, getRequest(), this->getResponse());
		setComponent("controller", oldController);
	}
	PROFILE_END();
}

SControllerToRun CWebApplication::resolveController(string route, const IModule * owner)
{
	SControllerToRun ret;
	if (owner == 0) {
		owner = this;
	}
	route = CStringUtils::trim(route, " /");
	if (route.empty()) {
		route = defaultControllerRoute;
	}
	CUrlManager * urlManager = getUrlManager();
	string id;
	::string::size_type pos = 0;

	while ((pos = route.find("/")) != ::string::npos) {
		id = route.substr(0, pos);
		boost::regex pattern("^\\w+$");
		boost::smatch match;
		if (!boost::regex_match(id, match, pattern)) {
			return ret;
		}

		if (!urlManager->caseSensitive) {
			::transform(id.begin(), id.end(), id.begin(), ::tolower);
		}
		route = route.substr(pos + 1);

		if (owner->hasController(id)) {
			ret.controller = owner->getController(id);
			pos = route.find("/");
			ret.actionId = pos == ::string::npos ? route : route.substr(0, pos);
			urlManager->parsePathInfo(route.substr(pos + 1));
			return ret;
		} else {
			return resolveController(route, owner->getModule(id));
		}
	}
	return ret;
}

CAssetManager * CWebApplication::getAssetManager() const
{
	return dynamic_cast<CAssetManager*>(getComponent("assetManager"));
}

CClientScript * CWebApplication::getClientScript() const
{
	return dynamic_cast<CClientScript*>(getComponent("clientScript"));
}

void CWebApplication::setWebRequestPool(IWebRequestPool * pool)
{
	_requestPool = pool;
}

IWebRequestPool * CWebApplication::getWebRequestPool() const
{
	return _requestPool;
}

CHttpRequest * CWebApplication::createHttpRequest()
{
	CHttpRequest * request = new CHttpRequest(this);
	request->init();
	return request;
}

CHttpResponse * CWebApplication::createHttpResponse()
{
	CHttpResponse * response = new CHttpResponse(this);
	response->init();
	return response;
}

CHttpSession * CWebApplication::createHttpSession()
{
	CHttpSession * session = new CHttpSession(this);
	session->init();
	return session;
}

CUrlManager * CWebApplication::createUrlManager()
{
	CUrlManager * urlManager = new CUrlManager(this);
	urlManager->init();
	return urlManager;
}
