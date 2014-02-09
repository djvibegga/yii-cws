/*
 * CWebApplication.cpp
 *
 *  Created on: Oct 16, 2013
 *      Author: djvibegga
 */

#include "web/CWebApplication.h"
#include "web/CController.h"
#include "base/CHttpException.h"
#include "base/CStringUtils.h"
#include <sstream>
#include <iostream>
#include <algorithm>
#include <boost/regex.hpp>
#include <config.h>

using namespace boost;
using namespace std;

CWebApplication::CWebApplication(const string &configPath, int argc, char * const argv[])
: CApplication(configPath, argc, argv)
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
#ifdef _UNICODE
	response->echo(cpptempl::utf8_to_wide(message));
#else
	response->echo(message);
#endif
}

void CWebApplication::mainLoop() throw(CException)
{
	string port = ":";
	const xml_node &configRoot = getConfigRoot();
	port.append(configRoot.child("server").attribute("port").value());
	cout << "Server started on port: " << port << endl;
	int listenQueueBacklog = configRoot.child("server")
		.child("threadsCount")
		.attribute("value").as_int();
	cout << "Server started with listeners: " << listenQueueBacklog << endl;

	if (FCGX_Init()) {
		throw CException("Can\'t initialize FCGX.");
	} else {
		//log << "Initialized FCGX." << endl;
	}

	int  listen_socket = FCGX_OpenSocket(port.c_str(), listenQueueBacklog);
	if (listen_socket < 0) {
		throw CException("Can\'t open socket.");
	} else {
		//log << "Socket opened successfully." << endl;
	}

	if (FCGX_InitRequest(&this->request, listen_socket, 0)) {
		throw CException("Can\'t initialize FCGX Request.");
	}

	while (FCGX_Accept_r(&this->request) == 0) {
		handleRequest();
		FCGX_Finish_r(&this->request);
	}
}

void CWebApplication::handleRequest()
{
	try {
		CApplication::handleRequest();
	} catch (const CHttpException & e) {
		FCGX_SetExitStatus(e.getStatus(), request.out);
		renderException(e);
	} catch (const CException & e) {
		renderException(e);
	}
}

void CWebApplication::beginRequest()
{
	CApplication::beginRequest();
	CHttpRequest * request = new CHttpRequest();
	CHttpResponse * response = new CHttpResponse(this);
	request->init();
	response->init();
	setComponent("request", request);
	setComponent("response", response);
	getOutputStack().push(response);
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

boost::filesystem::path CWebApplication::resolveLayoutPath()
{
	return boost::filesystem::path(getViewPath().normalize().string() + "/layouts");
}

void CWebApplication::runController(const string &route)
{
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
