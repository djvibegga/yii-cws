/*
 * CHttpCookie.h
 *
 *  Created on: Feb 24, 2014
 *      Author: djvibegga
 */

#ifndef CHTTPCOOKIE_H_
#define CHTTPCOOKIE_H_

#include "base/CComponent.h"

class CHttpRequest;

class CHttpCookie: public CComponent
{
public:
	string name;
	string value;
	string domain;
	string path;
	time_t expire;
	bool secure;
	bool httpOnly;

	CHttpCookie();
	CHttpCookie(
		const string & name,
		const string & value,
		const string & domain = "",
		const string & path = "/",
		unsigned int expire = 0
		);
	CHttpCookie(const CHttpCookie & other);
	CHttpCookie & operator=(const CHttpCookie & other);
};

typedef map<string, CHttpCookie> TCookieMap;

class CCookieCollection: public TCookieMap
{
private:
	const CHttpRequest & _request;
	static const char * _cookieVar;

public:
	CCookieCollection(const CHttpRequest & request);
	void add(const string & name, const CHttpCookie & cookie);
	void add(const CHttpCookie & cookie);
	void remove(const string & name, const CHttpCookie & cookie);
	void remove(const CHttpCookie & cookie);
	void remove(const string & name);
	TCookieMap getValidated() const;
};

#endif /* CHTTPCOOKIE_H_ */
