/*
 * CApplication.cpp
 *
 *  Created on: Oct 15, 2013
 *      Author: djvibegga
 */

#include "base/CApplication.h"
#include "base/CException.h"
#include <sstream>
#include <stdlib.h>

using namespace std;

CApplication::CApplication(const string &configPath)
: CModule(""),
  _request()
{
	_configPath = configPath;
}

CApplication::~CApplication()
{
}

void CApplication::run()
{
	try {
		init();
		mainLoop();
	} catch (const CException & e) {
		//log e;
	}
}

void CApplication::init()
{
	xml_document config;
	xml_parse_result result = config.load_file(_configPath.c_str());
	if (result.status != status_ok) {
		stringstream ss;
		ss << "Can\'t parse application config: '" << _configPath << "'.";
		throw CException(ss.str());
	}
	_xmlConfig = config.root();
}

void CApplication::mainLoop()
{
	string port = ":";
	port.append(_xmlConfig.child("server").attribute("port").value());
	//cout << "Server started on port: " << port << endl;
	int listenQueueBacklog = _xmlConfig.child("server")
		.child("threadsCount")
		.attribute("value").as_int();

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

	if (FCGX_InitRequest(&this->_request, listen_socket, 0)) {
		throw CException("Can\'t initialize FCGX Request.");
	}

	while (FCGX_Accept_r(&this->_request) == 0) {
		processRequest(this->_request);
		FCGX_Finish_r(&this->_request);
	}
}

void CApplication::echo(const string & content)
{
	FCGX_FPrintF(_request.out, content.c_str());
}
