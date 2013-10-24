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

void CWebApplication::run() throw(CException)
{
	CApplication::run();
	mainLoop();
}

void CWebApplication::renderException(const CException & e) const
{
	echo("Content-type: text/html\r\n\r\n<TITLE>fastcgi</TITLE>\n<H1>Fastcgi: 10050.</H1>\n");
	if (APP_DEBUG) {
		echo(e.getFullMessage());
	} else {
		echo(e.getMessage());
	}
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

void CWebApplication::echo(const string & content) const
{
	FCGX_FPrintF(request.out, content.c_str());
}

const CWebApplication & CWebApplication::operator<< (const string &right) const
{
	echo(right);
	return *this;
}

void CWebApplication::beginRequest()
{
	CApplication::beginRequest();
	CHttpRequest * request = new CHttpRequest();
	request->init();
	setComponent("request", request);
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
	delete getComponent("request");
	CApplication::endRequest();
}

CHttpRequest * CWebApplication::getRequest() const
{
	return dynamic_cast<CHttpRequest*>(getComponent("request"));
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

void CWebApplication::runController(const string &route)
{
	SControllerToRun ca = resolveController(route, 0);
	if (ca.actionId.empty()) {
		throw CHttpException(404, string("Unable to resolve the request."));
	} else {
		CController * controller = ca.controller;
		CController * oldController = dynamic_cast<CController*>(getComponent("controller"));
		setComponent("controller", controller);
		controller->run(ca.actionId, getRequest(), this);
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
	bool caseSensitive = getUrlManager()->caseSensitive;
	string id;
	int pos = 0;

	while ((pos = route.find("/")) != ::string::npos) {
		id = route.substr(0, pos);
		boost::regex pattern("^\\w+$");
		boost::smatch match;
		if (!boost::regex_match(id, match, pattern)) {
			return ret;
		}

		if (!caseSensitive) {
			::transform(id.begin(), id.end(), id.begin(), ::tolower);
		}
		route = route.substr(pos + 1);

		if (owner->hasController(id)) {
			ret.controller = owner->getController(id);
			ret.actionId = route;
			return ret;
		} else {
			return resolveController(route, owner->getModule(id));
		}
	}
	return ret;
}
