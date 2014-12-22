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
#include "base/CApplicationPool.h"
#include "web/renderers/CBaseViewRenderer.h"
#include "utils/CFile.h"
#include <sstream>
#include <iostream>
#include <boost/filesystem.hpp>
#include <stdlib.h>
#include <signal.h>
#include <boost/thread.hpp>
#include <boost/assign.hpp>

using namespace std;

TAppInstanceMap CApplication::_instances;
boost::mutex CApplication::_instanceLocker;
bool CApplication::_failHandlerCalled = false;
long CApplication::_mainThreadId = 0;
map<string, std::locale> CApplication::_locales;

CApplication::CApplication(const string &configPath, int argc, char * const argv[])
: CModule(""),
  _xmlConfig(0),
  _log(0),
  _pool(0),
  name(""),
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
  _pool(0),
  name(""),
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
	setLanguage("en");

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

	createViewRenderer();

	CModule::init();

	initLocales();
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

void CApplication::applyConfig(const xml_node & config)
{
	CModule::applyConfig(config);

	xml_node serverNode = getConfigRoot().child("server");

	PARSE_XML_CONF_STRING_PROPERTY(serverNode, _language, "language");
	PARSE_XML_CONF_STRING_PROPERTY(serverNode, name, "name");

	if (_language.empty()) {
		_languages.push_back(_language);
	}
	xml_node languages = serverNode.child("languages");
	if (!languages.empty()) {
		xml_object_range<xml_named_node_iterator> languagesList = languages.children("language");
		for (xml_object_range<xml_named_node_iterator>::const_iterator iter = languagesList.begin(); iter != languagesList.end(); ++iter) {
			_languages.push_back(iter->attribute("name").as_string());
		}
	}
	std::unique(_languages.begin(), _languages.end());
}

void CApplication::initLocales()
{
	boost::locale::generator gen;
	string path = getBasePath().normalize().string() + "/messages";
	gen.add_messages_path(path);

	TExtensionList extensions = boost::assign::list_of (".mo");
	TFileList translations = CFile::find(boost::filesystem::path(path), extensions, TExcludeList(), -1, false);

	vector<string> domains;
	for (TFileList::const_iterator iter = translations.begin(); iter != translations.end(); ++iter) {
		domains.push_back(boost::filesystem::basename(*iter));
	}

	std::unique(domains.begin(), domains.end());
	for (vector<string>::const_iterator iter = domains.begin(); iter != domains.end(); ++iter) {
		gen.add_messages_domain(*iter);
	}

	for (vector<string>::const_iterator iter = _languages.begin(); iter != _languages.end(); ++iter) {
		_locales[*iter] = gen(*iter + ".UTF-8");
	}
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
	return convertThreadIdToLong(boost::this_thread::get_id());
}

long CApplication::getMainThreadId()
{
	return _mainThreadId;
}

bool CApplication::getIsWorkerInstance()
{
	return getMainThreadId() != getThreadId();
}

bool CApplication::getIsWebWorkerInstance()
{
	return getIsWorkerInstance() && getInstance()->getComponent("request") != 0;
}

TOutputStack & CApplication::getOutputStack()
{
	return _outputStack;
}

void CApplication::setLanguage(const string & language)
{
	_language = language;
}

string CApplication::getLanguage() const
{
	return _language;
}

void CApplication::end(unsigned int status, bool needExit)
{
	if (hasEventHandler("onEndRequest")) {
		CEvent event(this);
		onEndRequest(event);
	}
	if (needExit) {
		exit(status);
	}
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

long CApplication::convertThreadIdToLong(boost::thread::id threadId)
{
	stringstream threadIdStr;
	threadIdStr << threadId;
	return strtol(threadIdStr.str().c_str(), 0, 16);
}

void CApplication::setPool(CApplicationPool * pool)
{
	_pool = pool;
}

CApplicationPool * CApplication::getPool() const
{
	return _pool;
}

vector<string> CApplication::getLanguages() const
{
	return _languages;
}

std::locale CApplication::getLocaleByLanguage(const string & language) const
{
	map<string, std::locale>::const_iterator found = _locales.find(language);
	if (found != _locales.end()) {
		return found->second;
	}
	return std::locale();
}
