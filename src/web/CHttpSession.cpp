/*
 * CHttpSession.cpp
 *
 *  Created on: Feb 22, 2014
 *      Author: djvibegga
 */

#include "web/CHttpSession.h"
#include "web/CWebApplication.h"
#include "base/Jvibetto.h"
#include "base/CAsyncTask.h"
#include "utils/CFile.h"
#include <boost/uuid/uuid_io.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

const string CHttpSession::SESSION_ID_KEY = "sessid";
const long int CHttpSession::DEFAULT_GC_SESSIONS_TIMEOUT = 1920;
const long int CHttpSession::DEFAULT_SESSION_LIFE_TIME = 1800;
const string CHttpSession::DEFAULT_SESSION_STATE_FILE_EXTENSION = ".state";
bool CHttpSession::_isGCRunned = false;
boost::uuids::basic_random_generator<boost::mt19937> CHttpSession::_gen;
boost::mutex CHttpSession::_gcMutexLocker;
boost::mutex CHttpSession::_idGeneratorLocker;

CHttpSession::CHttpSession()
: CApplicationComponent("session", Jvibetto::app()),
  gcTimeout(DEFAULT_GC_SESSIONS_TIMEOUT),
  autoOpen(false),
  lifeTime(DEFAULT_SESSION_LIFE_TIME)
{
}

CHttpSession::CHttpSession(CWebApplication * app)
: CApplicationComponent("session", app),
  gcTimeout(DEFAULT_GC_SESSIONS_TIMEOUT),
  autoOpen(false),
  lifeTime(DEFAULT_SESSION_LIFE_TIME)
{
}

CHttpSession::CHttpSession(const string &id, CWebApplication * app)
: CApplicationComponent(id, app),
  gcTimeout(DEFAULT_GC_SESSIONS_TIMEOUT),
  autoOpen(false),
  lifeTime(DEFAULT_SESSION_LIFE_TIME)
{
}

CHttpSession::CHttpSession(const CHttpSession & other)
: CApplicationComponent(other),
  _sessionId(other._sessionId),
  _sessionData(other._sessionData),
  gcTimeout(other.gcTimeout),
  autoOpen(other.autoOpen),
  lifeTime(other.lifeTime)
{
}

void CHttpSession::init()
{
	CApplicationComponent::init();
	if (sessionsPath.empty()) {
		sessionsPath = boost::filesystem::path(
			Jvibetto::app()->getRuntimePath().string() + "/sessions"
		);
		if (!boost::filesystem::is_directory(sessionsPath)) {
			boost::filesystem::create_directories(sessionsPath);
		}
	}
	ensureGCRunned();
}

void CHttpSession::applyConfig(const xml_node & config)
{
	PARSE_XML_CONF_INT_PROPERTY(config, gcTimeout, "gcTimeout");
	PARSE_XML_CONF_BOOL_PROPERTY(config, autoOpen, "autoOpen");
	PARSE_XML_CONF_INT_PROPERTY(config, lifeTime, "lifeTime");
}

void CHttpSession::setSessionId(const string & sessionId)
{
	_sessionId = sessionId;
}

string CHttpSession::getSessionId() const
{
	return _sessionId;
}

string CHttpSession::resolveSessionId() const
{
	return (dynamic_cast<CWebApplication*>(Jvibetto::app()))
		->getRequest()
		->getParam(SESSION_ID_KEY);
}

string CHttpSession::generateUniqueSessionId() const
{
	_idGeneratorLocker.lock();
	boost::uuids::uuid id = _gen();
	_idGeneratorLocker.unlock();
	stringstream ss;
	ss << id;
	return ss.str();
}

TSessionDataMap & CHttpSession::getData()
{
	return _sessionData;
}

string & CHttpSession::operator[](const string & key)
{
	return _sessionData[key];
}

void CHttpSession::reset()
{
	_sessionData = TSessionDataMap();
	_sessionId.clear();
}

bool CHttpSession::open() throw (CException)
{
	if (_sessionId.empty()) {
		_sessionId = resolveSessionId();
	}
	if (_sessionId.empty()) {
		_sessionId = generateUniqueSessionId();
	} else {
		_string strData = read(_sessionId);
		if (!strData.empty()) {
			return unserializeData(_to_utf8(strData));
		}
	}
	return false;
}

void CHttpSession::close() throw (CException)
{
	if (_sessionId.empty()) {
		return;
	}
	string strData;
	if (serializeData(strData)) {
		write(_sessionId, utf8_to_(strData));
	}
	reset();
}

bool CHttpSession::serializeData(string & dest)
{
	if (_sessionData.empty()) {
		return false;
	}

	stringstream os;
	boost::archive::text_oarchive oa(os);
	oa << *this;
	dest = os.str();

	return true;
}

bool CHttpSession::unserializeData(const string & src)
{
	if (src.empty()) {
		return false;
	}

	stringstream is;
	is << src;
	boost::archive::text_iarchive ia(is);
	ia >> *this;

	return true;
}

void CHttpSession::ensureGCRunned() throw (CException)
{
	_gcMutexLocker.lock();
	if (!_isGCRunned) {
		IWebRequestPool * pool = dynamic_cast<CWebApplication*>(getModule())
			->getWebRequestPool();
		if (pool) {
			CWebApplication * instance = pool->createAppInstance();
			CHttpSessionGCRunner * gcRunner = new CHttpSessionGCRunner(instance, gcTimeout);
			CAsyncTask * gcTask = new CAsyncTask(gcRunner);
			gcTask->init();
			gcTask->run();
			_isGCRunned = true;
		} else {
			throw CException("You can't enable http sessions without running CWebRequestPool.");
		}
	}
	_gcMutexLocker.unlock();
}

boost::filesystem::path CHttpSession::resolveSessionFilePath(const string & sessionId) const
{
	return boost::filesystem::path(
		sessionsPath.string() + "/" + sessionId + ".state"
	);
}

_string CHttpSession::read(const string & sessionId) const
{
	try {
		return utf8_to_(CFile::getContents(
			resolveSessionFilePath(_sessionId).string()
		));
	} catch (CException & e) {
		return _("");
	}
}

bool CHttpSession::write(const string & sessionId, const _string & data) const
{
	try {
		CFile::putContents(
			resolveSessionFilePath(_sessionId).string(),
			_to_utf8(data)
		);
	} catch (CException & e) {
		return false;
	}
	return true;
}

void CHttpSession::destroy(const string & sessionId) const throw (CException)
{
	boost::filesystem::remove(resolveSessionFilePath(_sessionId).string());
}

void CHttpSession::gcSessions() const throw (CException)
{
	TExtensionList extensions;
	extensions.push_back(DEFAULT_SESSION_STATE_FILE_EXTENSION);
	TFileList sessions = CFile::find(sessionsPath, extensions, TExcludeList(), 1, true);
	time_t now = time(0);
	boost::filesystem::path sessionFile;
	for (TFileList::const_iterator iter = sessions.begin(); iter != sessions.end(); ++iter) {
		sessionFile = *iter;
		if (now - boost::filesystem::last_write_time(sessionFile) > lifeTime) {
			boost::filesystem::remove(sessionFile);
		}
	}
}

CHttpSessionGCRunner::CHttpSessionGCRunner(CWebApplication * app, long int timeout)
: _instance(app),
  _timeout(timeout)
{
}

CHttpSessionGCRunner::~CHttpSessionGCRunner()
{
	if (_instance != 0) {
		delete _instance;
	}
}

void CHttpSessionGCRunner::init() throw (CException)
{
	if (_instance == 0) {
		throw CException("Bad pointer to an instance of web application was passed.");
	}
	_instance->init();
}

void CHttpSessionGCRunner::run() throw (CException)
{
	cout << "Session garbage collector has runned with timeout: " << _timeout << "." << endl;
	while (true) {
		sleep(_timeout);
		CHttpSession * session = dynamic_cast<CHttpSession*>(_instance->getComponent("session"));
#ifdef JV_DEBUG
		Jvibetto::trace("CHttpSession::gcSessions()");
#endif
		session->gcSessions();
	}
}
