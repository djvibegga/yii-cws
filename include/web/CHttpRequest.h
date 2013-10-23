/*
 * CHttpRequest.h
 *
 *  Created on: Oct 16, 2013
 *      Author: djvibegga
 */

#ifndef CHTTPREQUEST_H_
#define CHTTPREQUEST_H_

#include "base/CApplicationComponent.h"
#include "base/CHttpException.h"
#include <map>

using namespace std;

class CHttpRequest: public CApplicationComponent
{
private:
	TRequestParams _params;
	string _method;
	string _body;
	string _pathInfo;
	string _requestUri;
	string _scriptUrl;
	string _baseUrl;
	string _hostInfo;
	int _port;
	int _securePort;

public:
	CHttpRequest();
	virtual ~CHttpRequest();
	virtual void init();

	string getParam(const string &name);
	bool hasParam(const string &name) const;
	void setParam(const string &name, const string &value);
	virtual TRequestParams getParams();
	bool hasEnvVar(const char * name) const;
	string getEnvVar(const char * name) const;
	string getEnvVar(const string &name) const;
	string getRequestBody();
	string getRequestMethod();
	bool getIsPostRequest();
	bool getIsGetRequest();
	virtual bool getIsAjaxRequest();
	virtual bool getIsFlashRequest();
	virtual string getPathInfo();
	virtual string getRequestUri();
	string getScriptUrl();
	string getBaseUrl(bool absolute = false);
	string getHostInfo(const string & schema = "");
	bool getIsSecureConnection() const;
	int getSecurePort();
	int getPort();

protected:
	TRequestParams parseQueryString(string query) throw (CHttpException);
	virtual string decodePathInfo(const string &pathInfo) const;
	TRequestParams parseMultipartFormData(string data) throw (CHttpException);
};

#endif /* CHTTPREQUEST_H_ */
