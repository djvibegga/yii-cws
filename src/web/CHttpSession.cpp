/*
 * CHttpSession.cpp
 *
 *  Created on: Feb 22, 2014
 *      Author: djvibegga
 */

#include "web/CHttpSession.h"
#include "web/CWebApplication.h"
#include "base/Jvibetto.h"

const string CHttpSession::SESSION_ID_KEY = "sessid";

CHttpSession::CHttpSession(CModule * module)
: CApplicationComponent("session", module)
{
}

CHttpSession::CHttpSession(const string &id, CModule * module)
: CApplicationComponent(id, module)
{
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

bool CHttpSession::open() throw (CException)
{
	if (_sessionId.empty()) {
		_sessionId = resolveSessionId();
	}
	if (_sessionId.empty()) {
		_sessionId = generateUniqueSessionId();
	}
	_string strData = read(_sessionId);
	if (!strData.empty()) {
		return unserializeData(strData);
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
