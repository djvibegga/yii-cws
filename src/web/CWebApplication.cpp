/*
 * CWebApplication.cpp
 *
 *  Created on: Oct 16, 2013
 *      Author: djvibegga
 */

#include "web/CWebApplication.h"
#include "base/CHttpException.h"
#include <sstream>
#include <iostream>

CWebApplication::CWebApplication(const string &configPath)
: CApplication(configPath)
{
}

CWebApplication::~CWebApplication()
{

}

void CWebApplication::run() throw(CException)
{
	try {
		mainLoop();
	} catch (const CHttpException & e) {
		cout << e.getFullMessage() << endl;
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

void CWebApplication::echo(const string & content)
{
	FCGX_FPrintF(request.out, content.c_str());
}
