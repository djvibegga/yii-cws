/*
 * CHttpResponse.cpp
 *
 *  Created on: Oct 26, 2013
 *      Author: djvibegga
 */

#include "web/CHttpResponse.h"
#include "web/CWebApplication.h"
#include "base/CStringUtils.h"
#include <fcgi_stdio.h>


CHttpResponse::CHttpResponse(CWebApplication * app)
: CApplicationComponent("response", app),
  _startedOutput(false),
  _startedHeaders(false),
  _app(app)
{
}

CHttpResponse::~CHttpResponse()
{

}

_string CHttpResponse::getContent() const
{
	return _("");
}

void CHttpResponse::echo(const wstring & content)
{
	echo(cpptempl::wide_to_utf8(content));
}

void CHttpResponse::echo(const string & content)
{
	if (!_startedOutput && !_startedHeaders) {
		_startedHeaders = true;
		renderHeaders();
		_startedOutput = true;
	}
	FCGX_FPrintF(_app->request->out, content.c_str());
}

IOutputBuffer & CHttpResponse::operator<< (const _string &right)
{
	echo(right);
	return *this;
}

void CHttpResponse::renderHeaders()
{
	if (_headers.empty()) {
		addHeader("Content-type", "text/html");
	}
	string headers;
	for (THeadersMap::const_iterator iter = _headers.begin(); iter != _headers.end(); ++iter) {
		headers.append(iter->first + ": " + iter->second + "\r\n");
	}
	echo(headers);
	_putCookieHeaders();
	echo("\r\n");
}

void CHttpResponse::addHeader(const string & name, const string & value) throw (CException)
{
	if (_startedOutput) {
		throw CException("Can't modify headers because output already have started.");
	}
	_headers[name] = value;
}

void CHttpResponse::addCookie(const CHttpCookie & cookie) throw (CException)
{
	if (_startedOutput) {
		throw CException("Can't add cookie because output already have started.");
	}
	_newCookies[cookie.name] = cookie;
}

void CHttpResponse::removeCookie(const CHttpCookie & cookie) throw (CException)
{
	if (_startedOutput) {
		throw CException("Can't remove cookie because output already have started.");
	}
	_removedCookies[cookie.name] = cookie;
}

void CHttpResponse::_putCookieHeaders()
{
	string headers;
	for (TCookieMap::const_iterator iter = _removedCookies.begin(); iter != _removedCookies.end(); ++iter) {
		headers.append("Set-Cookie:" + iter->first + "=deleted;path=" + iter->second.path);
		if (!iter->second.domain.empty()) {
			headers.append(";domain=" + iter->second.domain);
		}
		if (iter->second.secure) {
			headers.append(";secure");
		}
		if (iter->second.httpOnly) {
			headers.append(";httpOnly");
		}
		headers.append(";expires=Thu, 01 Jan 1970 00:00:00 GMT");
		headers.append("\r\n");
	}
	for (TCookieMap::const_iterator iter = _newCookies.begin(); iter != _newCookies.end(); ++iter) {
		headers.append("Set-Cookie:" + iter->first + "=" + iter->second.value + ";path=" + iter->second.path);
		if (!iter->second.domain.empty()) {
			headers.append(";domain=" + iter->second.domain);
		}
		if (iter->second.expire) {
			headers.append(";expires=" + CStringUtils::fromTimestamp("%a, %d %b %G %T GMT", iter->second.expire, true));
		}
		if (iter->second.secure) {
			headers.append(";secure");
		}
		if (iter->second.httpOnly) {
			headers.append(";httpOnly");
		}
		headers.append("\r\n");
	}
	echo(headers);
}
