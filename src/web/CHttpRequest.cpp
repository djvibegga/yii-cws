/*
 * CHttpRequest.cpp
 *
 *  Created on: Oct 16, 2013
 *      Author: djvibegga
 */

#include "web/CHttpRequest.h"
#include "web/CWebApplication.h"
#include "base/Cws.h"
#include "base/CStringUtils.h"
#include "utils/CFile.h"
#include "fcgi_stdio.h"

#include <iostream>
#include <sstream>
#include <algorithm>
#include <boost/regex.hpp>

using namespace std;

CHttpRequest::CHttpRequest(CWebApplication * app)
: CApplicationComponent("request", app),
  _paramsRead(false),
  _port(0),
  _securePort(0),
  _cookies(*this)
{
}

string CHttpRequest::getClassName() const
{
	return "CHttpRequest";
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

bool CHttpRequest::hasEnvVar(const char * name) const
{
	CWebApplication * app = dynamic_cast<CWebApplication*>(Cws::app());
	return FCGX_GetParam(name, app->request->envp) != 0;
}

string CHttpRequest::getEnvVar(const char * name) const
{
	CWebApplication * app = dynamic_cast<CWebApplication*>(Cws::app());
	return (const char *)FCGX_GetParam(name, app->request->envp);
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
	if (!_paramsRead) {
		string query;
		if (getEnvVar("REQUEST_METHOD") == "POST") {
			query = getRequestBody();
		} else {
			query = getEnvVar("QUERY_STRING");
		}
		_params = parseQueryString(query);
		_paramsRead = true;
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
		CWebApplication * app = dynamic_cast<CWebApplication*>(Cws::app());
		string contentLength = getEnvVar("CONTENT_LENGTH");
		int len = 0;
		sscanf(contentLength.c_str(), "%d", &len);
		stringstream body;
		int ch;
		while ((ch = FCGX_GetChar(app->request->in)) >= 0) {
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

string CHttpRequest::getRequestUri()
{
	if (_requestUri.empty()) {
		if (hasEnvVar("HTTP_X_REWRITE_URL")) {// IIS
			_requestUri = getEnvVar("HTTP_X_REWRITE_URL");
		} else if (hasEnvVar("REQUEST_URI")) {
			_requestUri = getEnvVar("REQUEST_URI");
			string host = getEnvVar("HTTP_HOST");
			if (!host.empty()) {
				if (_requestUri.find(host) != ::string::npos) {
					boost::regex pattern("^\\w+://[^/]+");
					_requestUri = boost::regex_replace(_requestUri, pattern, "");
				}
			} else {
				boost::regex pattern("^(http|https)://[^/]+");
				_requestUri = boost::regex_replace(_requestUri, pattern, "");
			}
		} else if (hasEnvVar("ORIG_PATH_INFO")) { // IIS 5.0 CGI
			_requestUri = getEnvVar("ORIG_PATH_INFO");
			if (hasEnvVar("QUERY_STRING") && !getEnvVar("QUERY_STRING").empty()) {
				_requestUri += "?" + getEnvVar("QUERY_STRING");
			}
		} else {
			throw CException("CHttpRequest is unable to determine the request URI.");
		}
	}
	return _requestUri;
}

string CHttpRequest::decodePathInfo(const string &pathInfo) const
{
	return CStringUtils::urlDecode(pathInfo);
}


string CHttpRequest::getScriptUrl()
{
	if (_scriptUrl.empty()) {
		CWebApplication * app = dynamic_cast<CWebApplication *>(Cws::app());
		_scriptUrl = app->getUrlManager()->getScriptUrl();
	}
	return _scriptUrl;
}

string CHttpRequest::getBaseUrl(bool absolute)
{
	if (_baseUrl.empty()) {
		_baseUrl = CStringUtils::rtrim(CFile::dirName(getScriptUrl()), "\\/");
	}
	return absolute ? getHostInfo() + _baseUrl : _baseUrl;
}

bool CHttpRequest::getIsSecureConnection() const
{
	if (hasEnvVar("HTTPS")) {
		if (hasEnvVar("X-Forwarded-Proto")) {
			string schema = getEnvVar("X-Forwarded-Proto");
			::transform(schema.begin(), schema.end(), schema.begin(), ::tolower);
			return schema == "https";
		} else {
			string schema = getEnvVar("HTTPS");
			::transform(schema.begin(), schema.end(), schema.begin(), ::tolower);
			return schema != "off";
		}
	} else {
		return false;
	}
}

int CHttpRequest::getSecurePort()
{
	if (!_securePort) {
		if (getIsSecureConnection() && hasEnvVar("SERVER_PORT")) {
			_securePort = atoi(getEnvVar("SERVER_PORT").c_str());
		} else {
			_securePort = 443;
		}
	}
	return _securePort;
}

int CHttpRequest::getPort()
{
	if (!_port) {
		if (!getIsSecureConnection() && hasEnvVar("SERVER_PORT")) {
			_port = atoi(getEnvVar("SERVER_PORT").c_str());
		} else {
			_port = 80;
		}
	}
	return _port;
}

CCookieCollection & CHttpRequest::getCookies()
{
	return _cookies;
}

string CHttpRequest::getHostInfo(const string & schema)
{
	bool isSecure = getIsSecureConnection();
	if (_hostInfo.empty()) {
		string protocol = isSecure ? "https" : "http";
		if (hasEnvVar("HTTP_HOST")) {
			_hostInfo = protocol + "://" + getEnvVar("HTTP_HOST");
		} else {
			_hostInfo = protocol + "://" + getEnvVar("SERVER_NAME");
			int port = isSecure ? getSecurePort() : getPort();
			if ((port != 80 && !isSecure) || (port != 443 && isSecure)) {
				_hostInfo += ":" + port;
			}
		}
	}
	if (!schema.empty()) {
		if ((isSecure && schema == "https") || (!isSecure && schema == "http")) {
			return _hostInfo;
		}
		int port = schema == "https" ? getSecurePort() : getPort();
		string portExp;
		if ((port != 80 && schema == "http") || (port != 443 && schema == "https")) {
			portExp = ":" + port;
		}
		std::string::size_type schemaPos = _hostInfo.find(":");
		std::string::size_type portPos = _hostInfo.find_last_of(":");
		if (schemaPos == portPos) {
			return schema + _hostInfo.substr(schemaPos + 1);
		} else {
			return schema + _hostInfo.substr(schemaPos + 1, portPos - schemaPos - 1) + portExp;
		}
	} else {
		return _hostInfo;
	}
}

string CHttpRequest::getPathInfo()
{
	if (_pathInfo.empty()) {
		string pathInfo = getRequestUri();
		std::string::size_type pos = pathInfo.find("?");
		if (pos != ::string::npos) {
			pathInfo = pathInfo.substr(0, pos);
		}
		pathInfo = decodePathInfo(pathInfo);
		string scriptUrl = getScriptUrl();
		string baseUrl = getBaseUrl();
		if (pathInfo.find(scriptUrl) == 0) {
			pathInfo = pathInfo.substr(scriptUrl.length());
		} else if (baseUrl.empty() || pathInfo.find(baseUrl) == 0) {
			pathInfo = pathInfo.substr(baseUrl.length());
		} else {
			throw CException(
				"CHttpRequest is unable to determine the path info of the request."
			);
		}
		_pathInfo = CStringUtils::trim(pathInfo, "/");
	}
	return _pathInfo;
}

void CHttpRequest::setPathInfo(const string & pathInfo)
{
	_pathInfo = pathInfo;
}
