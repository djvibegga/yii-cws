/*
 * CApplication.cpp
 *
 *  Created on: Oct 15, 2013
 *      Author: djvibegga
 */

#include "base/CApplication.h"
#include "base/CException.h"
#include "base/CEvent.h"
#include "base/CStringUtils.h"
#include "base/CProfiler.h"
#include "base/Jvibetto.h"
#include "web/renderers/CBaseViewRenderer.h"
#include <sstream>
#include <iostream>
#include <boost/filesystem.hpp>
#include <stdlib.h>
#include <signal.h>
#include <boost/thread.hpp>

using namespace std;

TAppInstanceMap CApplication::_instances;
boost::mutex CApplication::_instanceLocker;
bool CApplication::_failHandlerCalled = false;

CApplication::CApplication(const string &configPath, int argc, char * const argv[])
: CModule(""),
  _xmlConfig(0),
  _log(0),
  startTime(0)
{
	signal(SIGSEGV, CApplication::_programFailCallback);
	_configPath = configPath;
	_xmlConfig = new xml_document();
	for (int i = 0; i < argc; ++i) {
		_arguments.push_back(argv[i]);
	}
}

CApplication::CApplication(const xml_document & configDocument, int argc, char * const argv[])
: CModule(""),
  _xmlConfig(0),
  _log(0),
  startTime(0)
{
	signal(SIGSEGV, CApplication::_programFailCallback);
	_xmlConfig = new xml_document();
	_xmlConfig->reset(configDocument);
	for (int i = 0; i < argc; ++i) {
		_arguments.push_back(argv[i]);
	}
}

CApplication::~CApplication()
{
	if (_xmlConfig != 0) {
		delete _xmlConfig;
	}
}

TArgumentsList CApplication::getArguments() const
{
	return _arguments;
}

string CApplication::getId() const
{
	return _id;
}

void CApplication::setId(const string &id)
{
	_id = id;
}

void CApplication::init() throw(CException)
{
	_instanceLocker.lock();
	_instances[getThreadId()] = this;
	_instanceLocker.unlock();

	attachEventHandler("onFatalError", this, EVENT_HANDLER(&CApplication::onProgramError));

	if (_xmlConfig->empty() && !_configPath.empty()) {
		xml_parse_result result;
		result = _xmlConfig->load_file(_configPath.c_str());
		if (result.status != status_ok) {
			stringstream ss;
			ss << "Can\'t parse application config: '" << _configPath << "'.";
			throw CException(ss.str());
		}
	}
	boost::filesystem::path executablePath(_arguments[0]);
	_executablePath = boost::filesystem::absolute(executablePath).normalize();

	boost::filesystem::path basePath = resolveBasePath();
	setBasePath(basePath);
	Jvibetto::setPathOfAlias("application", basePath);

	if (_runtimePath.empty()) {
		setRuntimePath(string(_executablePath.parent_path().c_str()) + "/runtime");
	}

	_log = createLogRouter();

#ifdef JV_DEBUG
	attachEventHandler("onEndRequest", this, EVENT_HANDLER(&CApplication::logProfileItems));
#endif

	CModule::init();

	createViewRenderer();
}

void CApplication::run() throw(CException)
{
	startTime = time(0);
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

xml_node CApplication::getConfigRoot() const
{
	return _xmlConfig->root();
}

xml_node CApplication::getConfigByNamePath(const TNamesPath & path) const
{
	xml_node node = getConfigRoot().child("server");
	for (TNamesPath::const_reverse_iterator iter = path.rbegin(); iter != path.rend(); ++iter) {
		node = node.child(iter->c_str());
	}
	return node;
}

CLogger & CApplication::getLogger()
{
	return _logger;
}

CProfiler & CApplication::getProfiler()
{
	return _profiler;
}

boost::filesystem::path CApplication::getRuntimePath() const
{
	return _runtimePath;
}

void CApplication::setRuntimePath(const boost::filesystem::path & path)
{
	_runtimePath = path;
}

void CApplication::setRuntimePath(const string & path)
{
	setRuntimePath(boost::filesystem::path(path));
}

boost::filesystem::path CApplication::resolveBasePath() const
{
    return _executablePath.parent_path();
}

boost::filesystem::path CApplication::getExecutablePath() const
{
	return _executablePath;
}

IViewRenderer * CApplication::getViewRenderer() const
{
    return dynamic_cast<IViewRenderer*>(getComponent("viewRenderer"));
}

IViewRenderer * CApplication::createViewRenderer()
{
	CBaseViewRenderer * renderer = new CBaseViewRenderer(this);
	renderer->init();
	return renderer;
}

CLogRouter * CApplication::createLogRouter()
{
	CLogRouter * log = new CLogRouter(this);
	log->init();
	return log;
}

CLogRouter * CApplication::getLog()
{
	return _log;
}

void CApplication::onProgramError()
{
	getLogger().flush(true);
}

CApplication * CApplication::getInstance()
{
	_instanceLocker.lock();
	CApplication * instance = _instances[getThreadId()];
	_instanceLocker.unlock();
	return instance;
}

long CApplication::getThreadId()
{
	stringstream threadId;
	threadId << boost::this_thread::get_id();
	return strtol(threadId.str().c_str(), 0, 16);
}

TOutputStack & CApplication::getOutputStack()
{
	return _outputStack;
}

void CApplication::logProfileItems(CEvent & event)
{
	_profiler.logItems();
}

void CApplication::_programFailCallback(int signum)
{
	if (!_failHandlerCalled) {
		_failHandlerCalled = true;
		CApplication * app = getInstance();
		CEvent event(app);
		app->raiseEvent("onFatalError", event);
	}
	signal(signum, SIG_DFL);
	exit(3);
}
