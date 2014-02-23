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
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

const string CHttpSession::SESSION_ID_KEY = "sessid";
const long int CHttpSession::DEFAULT_GC_SESSIONS_TIMEOUT = 1920;
bool CHttpSession::_isGCRunned = false;
boost::mutex CHttpSession::_gcMutexLocker;

CHttpSession::CHttpSession()
: CApplicationComponent("session", Jvibetto::app()),
  gcTimeout(DEFAULT_GC_SESSIONS_TIMEOUT),
  autoOpen(false)
{
}

CHttpSession::CHttpSession(CWebApplication * app)
: CApplicationComponent("session", app),
  gcTimeout(DEFAULT_GC_SESSIONS_TIMEOUT),
  autoOpen(false)
{
}

CHttpSession::CHttpSession(const string &id, CWebApplication * app)
: CApplicationComponent(id, app),
  gcTimeout(DEFAULT_GC_SESSIONS_TIMEOUT),
  autoOpen(false)
{
}

CHttpSession::CHttpSession(const CHttpSession & other)
: CApplicationComponent(other),
  _sessionId(other._sessionId),
  _sessionData(other._sessionData),
  gcTimeout(other.gcTimeout),
  autoOpen(other.autoOpen)
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
}

void CHttpSession::applyConfig(const xml_node & config)
{
	if (!config.child("gcTimeout").empty()) {
		gcTimeout = config
			.child("gcTimeout")
			.attribute("value")
			.as_int();
	}
	if (!config.child("autoOpen").empty()) {
		autoOpen = config
			.child("autoOpen")
			.attribute("value")
			.as_bool();
	}
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
	boost::uuids::basic_random_generator<boost::mt19937> gen;
	boost::uuids::uuid id = gen();
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
	ensureGCRunned();
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

boost::filesystem::path CHttpSession::resolveSessionFilePath() const
{
	return boost::filesystem::path(
		sessionsPath.string() + "/" + _sessionId + ".state"
	);
}

_string CHttpSession::read(const string & sessionId) const
{
	try {
		return utf8_to_(CFile::getContents(
			resolveSessionFilePath().string()
		));
	} catch (CException & e) {
		return _("");
	}
}

bool CHttpSession::write(const string & sessionId, const _string & data) const
{
	try {
		CFile::putContents(
			resolveSessionFilePath().string(),
			_to_utf8(data)
		);
	} catch (CException & e) {
		return false;
	}
	return true;
}

void CHttpSession::destroy(const string & sessionId) const throw (CException)
{

}

void CHttpSession::gcSessions() const throw (CException)
{
#ifdef JV_DEBUG
	Jvibetto::trace("CHttpSession::gcSessions()");
#endif
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
	while (true) {
		sleep(_timeout);
		CHttpSession * session = dynamic_cast<CHttpSession*>(_instance->getComponent("session"));
		session->gcSessions();
	}
}
