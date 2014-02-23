/*
 * CWebApplicationPool.cpp
 *
 *  Created on: Feb 18, 2014
 *      Author: djvibegga
 */

#include "web/CWebRequestPool.h"
#include "fcgi_stdio.h"
#include <boost/thread.hpp>

CWebRequestPool::CWebRequestPool(const string &configPath, int argc, char * const argv[])
: configPath(configPath),
  argc(argc),
  argv(argv),
  listenSocket(0)
{
	_xmlConfig = new xml_document();
}

CWebRequestPool::~CWebRequestPool()
{
}

void CWebRequestPool::init() throw(CException)
{
	xml_parse_result result = _xmlConfig->load_file(configPath.c_str());
	if (result.status != status_ok) {
		stringstream ss;
		ss << "Can\'t parse application config: '" << configPath << "'.";
		throw CException(ss.str());
	}
}

void CWebRequestPool::run() throw (CException)
{
	openSocket();
	startInstances();
	mainLoop();
}

void CWebRequestPool::openSocket()
{
	string port = ":";
	const xml_node &configRoot = _xmlConfig->root();
	port.append(configRoot.child("server").attribute("port").value());
	cout << "Server is trying to start on port: " << port << endl;
	int listenQueueBacklog = configRoot.child("server")
		.child("requestConcurrency")
		.attribute("value").as_int();

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

void CWebRequestPool::startInstances()
{
	unlink("server.pid");
	const xml_node &configRoot = _xmlConfig->root();
	int instancesCount = configRoot
		.child("server")
		.child("instance")
		.child("count").attribute("value").as_int();
	for (int i = 0; i < instancesCount; ++i) {
		CWebApplication * instance = createAppInstance();
		instance->setWebRequestPool(this);
		boost::thread threadObj(boost::bind(&CWebRequestPool::_runInstance, this, boost::ref(*instance)));
	}
}

FCGX_Request * CWebRequestPool::popRequest()
{
	_queueLocker.lock();
	if (_requests.empty()) {
		_queueLocker.unlock();
		return 0;
	}
	FCGX_Request * incoming = _requests.front();
	_requests.pop();
	_queueLocker.unlock();
	return incoming;
}

const xml_document & CWebRequestPool::getConfigDocument()
{
	return *_xmlConfig;
}

void CWebRequestPool::_runInstance(CWebApplication & instance)
{
	instance.init();
	instance.run();
}
