/*
 * CHttpCookie.cpp
 *
 *  Created on: Feb 24, 2014
 *      Author: djvibegga
 */

#include "web/CHttpCookie.h"
#include "web/CHttpRequest.h"
#include "web/CHttpResponse.h"
#include "base/CStringUtils.h"
#include "base/Jvibetto.h"

CHttpCookie::CHttpCookie()
: CComponent(),
  name(""),
  value(""),
  domain(""),
  path("/"),
  expire(0),
  secure(false),
  httpOnly(false)
{
}

CHttpCookie::CHttpCookie(
	const string & name,
	const string & value,
	const string & domain,
	const string & path,
	unsigned int expire)
: CComponent(),
  name(name),
  value(value),
  domain(domain),
  path(path),
  expire(expire),
  secure(false),
  httpOnly(false)
{
}

CHttpCookie::CHttpCookie(const CHttpCookie & other)
: CComponent(),
  name(other.name),
  value(other.value),
  domain(other.domain),
  path(other.path),
  expire(other.expire),
  secure(other.secure),
  httpOnly(other.httpOnly)
{
}

CHttpCookie & CHttpCookie::operator=(const CHttpCookie & other)
{
	name = other.name;
	value = other.value;
	domain = other.domain;
	path = other.path;
    expire = other.expire;
    secure = other.secure;
    httpOnly = other.httpOnly;
    return *this;
}

const char * CCookieCollection::_cookieVar = "HTTP_COOKIE";

CCookieCollection::CCookieCollection(const CHttpRequest & request)
: TCookieMap(),
  _request(request)
{
	if (request.hasEnvVar(_cookieVar)) {
		string cookiesStr = request.getEnvVar(_cookieVar);
		vector<string> cookies = CStringUtils::explode(";", cookiesStr);
		vector<string> cookieItem;
		string key;
		string value;
		for (vector<string>::const_iterator iter = cookies.begin(); iter != cookies.end(); ++iter) {
			cookieItem = CStringUtils::explode("=", *iter, 2);
			if (cookieItem.size() == 2) {
				key = CStringUtils::trim(cookieItem[0]);
				value = CStringUtils::trim(cookieItem[1]);
				TCookieMap::operator [](key) = CHttpCookie(key, value);
			}
		}
	}
}

void CCookieCollection::add(const string & name, const CHttpCookie & cookie)
{
	if (name.empty()) {
		return;
	}
	//remove(name, cookie);
	TCookieMap::operator [](name) = cookie;
	CHttpResponse * response = dynamic_cast<CHttpResponse*>(Jvibetto::app()->getComponent("response"));
	response->addCookie(cookie);
}

void CCookieCollection::add(const CHttpCookie & cookie)
{
	add(cookie.name, cookie);
}

void CCookieCollection::remove(const string & name, const CHttpCookie & cookie)
{
	if (name.empty()) {
		return;
	}
	TCookieMap::erase(name);
	CHttpResponse * response = dynamic_cast<CHttpResponse*>(Jvibetto::app()->getComponent("response"));
	response->removeCookie(cookie);
}

void CCookieCollection::remove(const CHttpCookie & cookie)
{
	remove(cookie.name, cookie);
}

void CCookieCollection::remove(const string & name)
{
	remove(name, CHttpCookie(name, ""));
}

TCookieMap CCookieCollection::getValidated() const
{
	TCookieMap validated;
	return validated;
}
