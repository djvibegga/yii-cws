/*
 * CHttpCookie.cpp
 *
 *  Created on: Feb 24, 2014
 *      Author: djvibegga
 */

#include "web/CHttpCookie.h"
#include "web/CHttpRequest.h"
#include "web/CHttpResponse.h"
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

CCookieCollection::CCookieCollection(const CHttpRequest & request)
: TCookieMap(),
  _request(request)
{
	//TODO: parse cookies from request...
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
