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

CApplication::CApplication(const string &configPath, int argc, char * const argv[])
: CModule(""),
  _xmlConfig(0)
{
	_configPath = configPath;
	_xmlConfig = new xml_document();
	for (int i = 0; i < argc; ++i) {
		_arguments.push_back(argv[i]);
	}
}

CApplication::~CApplication()
{
	delete _xmlConfig;
}

TArgumentsList CApplication::getArguments() const
{
	return _arguments;
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
	CModule::init();
}

void CApplication::run() throw(CException)
{
}

void CApplication::handleRequest()
{
	beginRequest();
	processRequest();
	endRequest();
}

void CApplication::beginRequest()
{
	if (hasEventHandler("onBeginRequest")) {
		CEvent event(this);
		onBeginRequest(event);
	}
}

void CApplication::onBeginRequest(CEvent & event)
{
	raiseEvent("onBeginRequest", event);
}

void CApplication::endRequest()
{
	if (hasEventHandler("onEndRequest")) {
		CEvent event(this);
		onEndRequest(event);
	}
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
