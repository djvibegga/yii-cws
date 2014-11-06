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
  listenSocket(0)
{
}

CWebRequestPool::~CWebRequestPool()
{
}

void CWebRequestPool::run() throw (CException)
{
	openSocket();
	CApplicationPool::run();
	mainLoop();
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

void CWebRequestPool::mainLoop() throw (CException)
{
	while (true) {
		FCGX_Request * request = new FCGX_Request();
		if (FCGX_InitRequest(request, listenSocket, 0)) {
			throw CException("Can\'t initialize FCGX Request.");
		}
		if (FCGX_Accept_r(request) == 0) {
			_queueLocker.lock();
			_requests.push(request);
			_queueLocker.unlock();
		}
	}
}

FCGX_Request * CWebRequestPool::popRequest() throw (boost::lock_error)
{
	boost::lock_guard<boost::mutex> guard(_queueLocker);
	if (_requests.empty()) {
		return 0;
	}
	FCGX_Request * incoming = _requests.front();
	_requests.pop();
	return incoming;
}
