/*
 * CApplication.cpp
 *
 *  Created on: Oct 15, 2013
 *      Author: djvibegga
 */

#include "base/CApplication.h"
#include "base/CException.h"
#include "base/CEvent.h"
#include <sstream>
#include <iostream>
#include <stdlib.h>

using namespace std;

CApplication * CApplication::_instance = 0;

CApplication::CApplication(const string &configPath)
: CModule(""),
  _xmlConfig(0)
{
	_configPath = configPath;
	_xmlConfig = new xml_document();
}

CApplication::~CApplication()
{
	delete _xmlConfig;
}

void CApplication::init() throw(CException)
{
	_instance = this;
	xml_parse_result result = _xmlConfig->load_file(_configPath.c_str());
	if (result.status != status_ok) {
		stringstream ss;
		ss << "Can\'t parse application config: '" << _configPath << "'.";
		throw CException(ss.str());
	}
}

void CApplication::run() throw(CException)
{
	throw CException(
		"You should override CApplication::run() method",
		0, __FILE__, __LINE__
	);
}

void CApplication::handleRequest()
{
	if (hasEventHandler("onBeginRequest")) {
		CEvent event(this);
		onBeginRequest(event);
	}
	processRequest();
	if (hasEventHandler("onEndRequest")) {
		CEvent event(this);
		onEndRequest(event);
	}
}

void CApplication::onBeginRequest(CEvent & event)
{
	raiseEvent("onBeginRequest", event);
}

void CApplication::onEndRequest(CEvent & event)
{
	raiseEvent("onEndRequest", event);
}

xml_node CApplication::getConfigRoot()
{
	return _xmlConfig->root();
}

CApplication * CApplication::getInstance()
{
	return _instance;
}
