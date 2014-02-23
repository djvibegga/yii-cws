/*
 * CHttpSession.cpp
 *
 *  Created on: Feb 22, 2014
 *      Author: djvibegga
 */

#include "web/CHttpSession.h"
#include "web/CWebApplication.h"
#include "base/Jvibetto.h"
#include "base/CStringUtils.h"
#include <boost/thread.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

const string CHttpSession::SESSION_ID_KEY = "sessid";
const long int CHttpSession::DEFAULT_GC_SESSIONS_TIMEOUT = 1920;
bool CHttpSession::_isGCRunned = false;

CHttpSession::CHttpSession()
: CApplicationComponent("session", Jvibetto::app()),
  gcTimeout(DEFAULT_GC_SESSIONS_TIMEOUT),
  autoOpen(false)
{

}

CHttpSession::CHttpSession(CModule * module)
: CApplicationComponent("session", module),
  gcTimeout(DEFAULT_GC_SESSIONS_TIMEOUT),
  autoOpen(false)
{
}

CHttpSession::CHttpSession(const string &id, CModule * module)
: CApplicationComponent(id, module),
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
	return "test";
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
	if (!_isGCRunned) {
		boost::thread gcThread(boost::bind(
			&CHttpSession::_runGarbageCollector, this,
			boost::ref(gcTimeout)
		));
		_isGCRunned = true;
	}
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

boost::filesystem::path CHttpSession::resolveSessionFilePath() const
{
	return boost::filesystem::path(
		sessionsPath.string() + "/" + _sessionId + ".state"
	);
}

_string CHttpSession::read(const string & sessionId) const
{
	try {
		return utf8_to_(CStringUtils::fileGetContents(
			resolveSessionFilePath().string()
		));
	} catch (CException & e) {
		return _("");
	}
}

bool CHttpSession::write(const string & sessionId, const _string & data) const
{
	try {
		CStringUtils::filePutContents(
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
	//Jvibetto::trace("CHttpSession::gcSessions()");
#endif
}

void CHttpSession::_runGarbageCollector(long int & interval)
{
	while (true) {
		sleep(interval);
		gcSessions();
	}
}
