/*
 * CHttpResponse.h
 *
 *  Created on: Oct 26, 2013
 *      Author: djvibegga
 */

#ifndef CHTTPRESPONSE_H_
#define CHTTPRESPONSE_H_

#include "base/CApplicationComponent.h"
#include "web/CHttpCookie.h"

class CWebApplication;

typedef map<string, string> THeadersMap;

class CHttpResponse: public CApplicationComponent, public IOutputBuffer
{
private:
	bool _startedOutput;
	bool _startedHeaders;
	const CWebApplication * _app;
	TCookieMap _newCookies;
	TCookieMap _removedCookies;
	THeadersMap _headers;

	void _putCookieHeaders();

public:
    CHttpResponse(CWebApplication * app);
    ~CHttpResponse();

    virtual _string getContent() const;

    virtual void echo(const wstring & content);
    virtual void echo(const string & content);
    virtual IOutputBuffer & operator<< (const _string &right);
    void addHeader(const string & name, const string & value) throw (CException);
    void addCookie(const CHttpCookie & cookie) throw (CException);
    void removeCookie(const CHttpCookie & cookie) throw (CException);
    void removeCookie(const string & cookieName) throw (CException);
    void redirect(TRouteStruct & route, bool terminate = true, unsigned int statusCode = 302);
    void redirect(const string & url, bool terminate = true, unsigned int statusCode = 302);

protected:
    virtual void renderHeaders();
};

#endif /* CHTTPRESPONSE_H_ */
