/*
 * CHttpSession.cpp
 *
 *  Created on: Feb 22, 2014
 *      Author: djvibegga
 */

#include "web/CHttpSession.h"
#include "web/CWebApplication.h"
#include "base/Jvibetto.h"
#include <boost/thread.hpp>

const string CHttpSession::SESSION_ID_KEY = "sessid";
const long int CHttpSession::DEFAULT_GC_SESSIONS_TIMEOUT = 1920;
bool CHttpSession::_isGCRunned = false;

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

data_map & CHttpSession::getData()
{
	return _sessionData;
}

void CHttpSession::reset()
{
	_sessionData = data_map();
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
			return unserializeData(strData);
		}
	}
	return false;
}

void CHttpSession::close() throw (CException)
{
	if (_sessionId.empty()) {
		return;
	}
	_string strData;
	if (serializeData(strData)) {
		write(_sessionId, strData);
	}
}

bool CHttpSession::serializeData(_string & dest)
{
	if (_sessionData.empty()) {
		return false;
	}
	//TODO: serialize into dest
	return true;
}

bool CHttpSession::unserializeData(const _string & src)
{
	if (src.empty()) {
		return false;
	}
	//TODO: unserialize into _sessionData
	return true;
}

_string CHttpSession::read(const string & sessionId) const throw (CException)
{
	return _("");
}

bool CHttpSession::write(const string & sessionId, const _string & data) const throw (CException)
{
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

void CHttpSession::_runGarbageCollector(long int & interval)
{
	while (true) {
		sleep(interval);
		gcSessions();
	}
}
