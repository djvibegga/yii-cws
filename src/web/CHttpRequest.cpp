/*
 * CHttpRequest.cpp
 *
 *  Created on: Oct 16, 2013
 *      Author: djvibegga
 */

#include "web/CHttpRequest.h"
#include "web/CWebApplication.h"
#include "base/YiiBase.h"
#include "fcgi_stdio.h"

#include <iostream>
#include <sstream>
#include <algorithm>

using namespace std;

CHttpRequest::CHttpRequest()
: CApplicationComponent("request", 0)
{

}

CHttpRequest::~CHttpRequest()
{

}

void CHttpRequest::init()
{
	getParams();
}

string CHttpRequest::getParam(const string &name)
{
	return _params[name];
}

void CHttpRequest::setParam(const string &name, const string &value)
{
	_params[name] = value;
}

string CHttpRequest::getEnvVar(const char * name) const
{
	CWebApplication * app = dynamic_cast<CWebApplication*>(YiiBase::app());
	return FCGX_GetParam(name, app->request.envp);
}

string CHttpRequest::getEnvVar(const string &name) const
{
	return getEnvVar(name.c_str());
}

TRequestParams CHttpRequest::parseQueryString(string query) throw (CHttpException)
{
	TRequestParams params;
	if (query.length() > 0) {
		int pos = 0;
		int namePos = 0;
		string token;
		do {
			pos = query.find_first_of('&');
			token = pos == -1 ? query : query.substr(0, pos);
			query = query.substr(pos + 1);
			namePos = token.find_first_of('=');
			params[token.substr(0, namePos)] = token.substr(namePos + 1);
		} while (pos != -1);
	}
	return params;
}

TRequestParams CHttpRequest::parseMultipartFormData(string data) throw (CHttpException)
{
	throw CException("Not implemented yet!");
}

bool CHttpRequest::hasParam(const string &name) const
{
	return _params.find(name) != _params.end();
}

TRequestParams CHttpRequest::getParams()
{
	if (_params.empty()) {
		string query;
		if (getEnvVar("REQUEST_METHOD") == "POST") {
			query = getRequestBody();
		} else {
			query = getEnvVar("QUERY_STRING");
		}
		_params = parseQueryString(query);
	}
	return _params;
}

string CHttpRequest::getRequestBody()
{
	if (_body.empty()) {
		string method = getEnvVar("REQUEST_METHOD");
		if (method == "GET" || method == "HEAD") {
			return "";
		}
		CWebApplication * app = dynamic_cast<CWebApplication*>(YiiBase::app());
		string contentLength = getEnvVar("CONTENT_LENGTH");
		int len = 0;
		sscanf(contentLength.c_str(), "%d", &len);
		stringstream body;
		int ch;
		while ((ch = FCGX_GetChar(app->request.in)) >= 0) {
			body << (char)ch;
		}
		_body = body.str();
	}
	return _body;
}

string CHttpRequest::getRequestMethod()
{
	if (_method.empty()) {
		_method = getEnvVar("REQUEST_METHOD");
		::transform(_method.begin(), _method.end(), _method.begin(), ::toupper);
	}
	return _method;
}

bool CHttpRequest::getIsPostRequest()
{
	return getRequestMethod() == "POST";
}

bool CHttpRequest::getIsGetRequest()
{
	return getRequestMethod() == "GET";
}

bool CHttpRequest::getIsAjaxRequest()
{
	if (hasParam("HTTP_X_REQUESTED_WITH")) {
		return _params["HTTP_X_REQUESTED_WITH"] == "XMLHttpRequest";
	}
	return false;
}

bool CHttpRequest::getIsFlashRequest()
{
	if (hasParam("HTTP_USER_AGENT")) {
		string userAgent = _params["HTTP_USER_AGENT"];
		if (userAgent.find("Shockwave") != ::string::npos) {
			return true;
		}
		if (userAgent.find("Flash") != ::string::npos) {
			return true;
		}
	}
	return false;
}
