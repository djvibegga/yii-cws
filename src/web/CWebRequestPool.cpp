/*
 * CWebApplicationPool.cpp
 *
 *  Created on: Feb 18, 2014
 *      Author: djvibegga
 */

#include "web/CWebRequestPool.h"
#include "fcgi_stdio.h"

CWebRequestPool::CWebRequestPool(const string &configPath, int argc, char * const argv[])
: CApplicationPool(configPath, argc, argv),
  listenSocket(0),
  idleTimeout(100000)
{
}

CWebRequestPool::~CWebRequestPool()
{
}

void CWebRequestPool::init() throw (CException)
{
	CApplicationPool::init();

	const xml_node &configRoot = getConfigDocument().root();
	PARSE_XML_CONF_UINT_PROPERTY(
		configRoot.child("server").child("instance"),
		idleTimeout, "idleTimeout"
	);
}

void CWebRequestPool::run() throw (CException)
{
	openSocket();
	CApplicationPool::run();
	mainLoop();
}

int CWebRequestPool::getListenSocket() const
{
	return listenSocket;
}

void CWebRequestPool::openSocket()
{
	string port = ":";
	const xml_node &configRoot = getConfigDocument().root();
	port.append(configRoot.child("server").attribute("port").value());
	cout << "Server is trying to start on port: " << port << endl;
	int listenQueueBacklog = 0;
	PARSE_XML_CONF_INT_PROPERTY(configRoot.child("server"), listenQueueBacklog, "requestConcurrency");

	if (FCGX_Init()) {
		throw CException("Can\'t initialize FCGX.");
	} else {
		cout << "Initialized FCGX." << endl;
	}

	listenSocket = FCGX_OpenSocket(port.c_str(), listenQueueBacklog);
	if (listenSocket < 0) {
		throw CException("Can't open socket.");
	} else {
		cout << "Server has started with concurrent listeners limit: " << listenQueueBacklog << endl;
	}
}

void CWebRequestPool::mainLoop()
{
	while (getIsActive()) {
		usleep(idleTimeout);
	}
}
