/*
 * CUrlManager.cpp
 *
 *  Created on: Oct 19, 2013
 *      Author: djvibegga
 */

#include "web/CUrlManager.h"
#include "web/CWebApplication.h"
#include "web/CHttpRequest.h"
#include "base/CStringUtils.h"
#include "base/Jvibetto.h"
#include "config.h"

#include <boost/regex.hpp>

CUrlRule::CUrlRule(const string & routeExp, const string & patternExp)
: hasHostInfo(false),
  caseSensitive(false),
  matchValue(false),
  parsingOnly(false),
  route(routeExp),
  pattern(patternExp),
  append(false)
{
	route = CStringUtils::trim(routeExp, "/");
	TReplacementMap tr, tr2;
	//tr2["/"] = tr["/"] = "\\/";

	if (route.find('<') != ::string::npos) {
		vector<boost::smatch> matches = CStringUtils::regexMatchAll(route, boost::regex("<(\\w+)>"));
		for (vector<boost::smatch>::const_iterator iter = matches.begin(); iter != matches.end(); ++iter) {
			_references[(*iter)[1]] = "<" + (*iter)[1] + ">";
		}
	}

	hasHostInfo = pattern.substr(0, 7) == "http://" || pattern.substr(0, 8) == "https://";

	TRouteReferencesMap tokens;
	vector<boost::smatch> matchList = CStringUtils::regexMatchAll(
		patternExp,
		boost::regex("<(\\w+):(.*?)?>")
	);
	boost::smatch matches;
	for (vector<boost::smatch>::iterator iter = matchList.begin(); iter != matchList.end(); ++iter) {
		matches = *iter;
		tokens[matches[1]] = matches[2];
	}

	string value;
	for (TRouteReferencesMap::iterator iter = tokens.begin(); iter != tokens.end(); ++iter) {
		value = iter->second;
		if (value.empty()) {
			value = "([^/]+)";
		}
		tr["<" + iter->first + ">"] = "(" + value + ")";
		TRouteReferencesMap::const_iterator ref = _references.find(iter->first);
		if (ref != _references.end()) {
			string key = "<" + iter->first + ">";
			tr2[key] = tr[key];
		} else {
			_params[iter->first] = iter->second;
		}
	}

	string p = CStringUtils::rtrim(pattern, "*");
	append = p != pattern;
	p = CStringUtils::trim(p, "/");
	templateStr = boost::regex_replace(p, boost::regex("<(\\w+):?.*?>"), "<$1>");
	pattern = "^" + CStringUtils::strtr(templateStr, tr) + "/";
	if (!append) {
		pattern += "$";
	} else {
		pattern += "(.*)";
	}

	if (!_references.empty()) {
		routePattern = "^" + CStringUtils::strtr(route, tr2) + "$";
	}

#ifdef JV_DEBUG
	try {
		boost::smatch matches;
		boost::regex_match(string("test"), matches, boost::regex(pattern));
	} catch (const boost::regex_error & e) {
		throw CException(
			"The URL pattern '"
			+ pattern + "' for route '"
			+ route + "' is not a valid regular expression."
		);
	}
#endif
}

string CUrlRule::getClassName() const
{
	return "CUrlRule";
}

CUrlRule::~CUrlRule()
{
}

string CUrlRule::createUrl(
		const CUrlManager * const manager,
		TRouteStruct &route, const string & ampersand) const
{
	if (parsingOnly || templateStr.empty()) {
		return "";
	}
	bool ignoreCase = !((manager->caseSensitive && !caseSensitive) || caseSensitive);
	::boost::regbase::flag_type caseFlag = ignoreCase
		? boost::regex_constants::icase
		: boost::regex_constants::normal;

	TRouteReferencesMap tr;
	if (route.path != this->route) {
		boost::regex regexp(routePattern, caseFlag);
		boost::smatch matches;
		if (!routePattern.empty() && boost::regex_match(route.path, matches, regexp)) {
			unsigned int i = 0;
			for (TRouteReferencesMap::const_iterator iter = _references.begin(); iter != _references.end(); ++iter, ++i) {
				tr[iter->second] = matches[i + 1];
			}
		} else {
			return "";
		}
	}

	TRequestParams routeParams = route.params;
	TRequestParams::iterator rpi;
	for (TRequestParams::const_iterator iter = defaultParams.begin(); iter != defaultParams.end(); ++iter) {
		rpi = routeParams.find(iter->first);
		if (rpi != routeParams.end()) {
			if (routeParams[iter->first] == iter->second) {
				routeParams.erase(rpi);
			} else {
				return "";
			}
		}
	}

	for (TRouteReferencesMap::const_iterator iter = _params.begin(); iter != _params.end(); ++iter) {
		if (iter->first.empty()) {
			return "";
		}
	}

	boost::smatch matches;
	if ((manager->matchValue && !matchValue) || matchValue) {
		for (TRouteReferencesMap::const_iterator iter = _params.begin(); iter != _params.end(); ++iter) {
			boost::regex pattern("\\A" + iter->second + "\\z", caseFlag);
			if (!boost::regex_match(routeParams[iter->first], matches, pattern)) {
				return "";
			}
		}
	}

	TRequestParams::iterator paramsIterator;
	for (TRouteReferencesMap::const_iterator iter = _params.begin(); iter != _params.end(); ++iter) {
		tr["<" + iter->first + ">"] = CStringUtils::urlEncode(routeParams[iter->first]);
		paramsIterator = routeParams.find(iter->first);
		if (paramsIterator != routeParams.end()) {
			routeParams.erase(paramsIterator);
		}
	}

	string suffix = _urlSuffix.empty() ? manager->urlSuffix : _urlSuffix;
	string url = CStringUtils::strtr(templateStr, tr);

	if (hasHostInfo) {
		string hostInfo = dynamic_cast<CWebApplication*>(Jvibetto::app())->getRequest()->getHostInfo();
		if (url.find(hostInfo) == 0) {
			url = url.substr(hostInfo.length());
		}
	}

	if (routeParams.empty()) {
		return !url.empty() ? url + suffix : url;
	}

	url = CStringUtils::rtrim(url, "/");
	if (append) {
		url += "/" + manager->createPathInfo(routeParams, "/", "/") + suffix;
	} else {
		if (!url.empty()) {
			url += suffix;
		}
		url += "?" + manager->createPathInfo(routeParams, "=", ampersand);
	}
	return url;
}

string CUrlRule::parseUrl(
		const CUrlManager * const manager,
		CHttpRequest * const request,
		const string & pathInfo, const string & rawPathInfo) const
{
	if (!verb.empty() && verb != request->getRequestMethod()) {
		return "";
	}
	string path = pathInfo;

	bool ignoreCase = !((manager->caseSensitive && !caseSensitive) || caseSensitive);

	if (!_urlSuffix.empty()) {
		path = manager->removeUrlSuffix(rawPathInfo, _urlSuffix);
	}

	// URL suffix required, but not found in the requested URL
	string urlSuffix;
	if (manager->useStrictParsing && path == rawPathInfo) {
		urlSuffix = _urlSuffix.empty() ? manager->urlSuffix : _urlSuffix;
		if (!urlSuffix.empty() && urlSuffix != "/") {
			return "";
		}
	}

	if (hasHostInfo) {
		string hostInfo = CStringUtils::lowerCase(request->getHostInfo());
		path = hostInfo + CStringUtils::rtrim("/" + path, "/");
	}

	path += "/";
	boost::smatch matches;
	boost::regex regexp(pattern, ignoreCase ? boost::regex_constants::icase : boost::regex_constants::normal);
	if (boost::regex_match(path, matches, regexp)) {
		for (TRequestParams::const_iterator iter = defaultParams.begin(); iter != defaultParams.end(); ++iter) {
			if (request->hasParam(iter->first)) {
				request->setParam(iter->first, iter->second);
			}
		}
		TReplacementMap tr;
		string key;
		unsigned int i = 0;
		for (TRouteReferencesMap::const_iterator iter = _params.begin(); iter != _params.end(); ++iter, ++i) {
			request->setParam(iter->first, matches[i + 1]);
			if (_references.find(iter->first) != _references.end()) {
				tr[_references.find(iter->first)->second] = matches[i + 1];
			}
		}
		string lastMatch;
		unsigned int paramsSize = _params.size();
		if (matches.size() > paramsSize) {// there're additional GET params
			lastMatch = string(matches[paramsSize + 1].first, matches[paramsSize + 1].second);
		}
		if (!lastMatch.empty())
			manager->parsePathInfo(CStringUtils::ltrim(lastMatch, "/"));
		if (!routePattern.empty()) {
			return CStringUtils::strtr(route, tr);
		} else {
			return route;
		}
	}
	return "";
}

const string CUrlManager::FORMAT_GET = "get";
const string CUrlManager::FORMAT_PATH = "path";
const string CUrlManager::DEFAULT_LANGUAGE_PARAM_NAME = "lang";
const string CUrlManager::DEFAULT_LANGUAGE_COOKIE_NAME = "lang";

CUrlManager::CUrlManager(CModule * module)
: CApplicationComponent("urlManager", module),
  _baseUrl(""),
  urlFormat(FORMAT_GET),
  showScriptName(true),
  appendParams(true),
  routeVar("r"),
  matchValue(false),
  useStrictParsing(false),
  caseSensitive(true),
  languageParamName(DEFAULT_LANGUAGE_PARAM_NAME),
  languageCookieName(DEFAULT_LANGUAGE_COOKIE_NAME),
  useLangBasedUrls(true),
  storeLanguageInCookies(false),
  appendLanguageWhenItIsDefault(false)
{
}

string CUrlManager::getClassName() const
{
	return "CUrlManager";
}

CUrlManager::~CUrlManager()
{
	for (TUrlRulesList::iterator iter = _rules.begin(); iter != _rules.end(); ++iter) {
		delete *iter;
	}
}

void CUrlManager::init()
{
	CApplicationComponent::init();
	_scriptName = resolveScriptName();
	initRules();

	_defaultLanguage = Jvibetto::app()->getLanguage();
}

void CUrlManager::applyConfig(const xml_node & config)
{
	PARSE_XML_CONF_STRING_PROPERTY(config, urlFormat, "urlFormat");
	PARSE_XML_CONF_BOOL_PROPERTY(config, showScriptName, "showScriptName");
	PARSE_XML_CONF_STRING_PROPERTY(config, languageParamName, "languageParamName");
	PARSE_XML_CONF_STRING_PROPERTY(config, languageCookieName, "languageCookieName");
	PARSE_XML_CONF_BOOL_PROPERTY(config, useLangBasedUrls, "useLangBasedUrls");
	PARSE_XML_CONF_BOOL_PROPERTY(config, storeLanguageInCookies, "storeLanguageInCookies");
	PARSE_XML_CONF_BOOL_PROPERTY(config, appendLanguageWhenItIsDefault, "appendLanguageWhenItIsDefault");
}

void CUrlManager::addRule(CBaseUrlRule * rule)
{
	_rules.push_back(rule);
}

void CUrlManager::initRules()
{

}

string CUrlManager::createUrl(TRouteStruct &route, const string & ampersand) const
{
	for (TRequestParams::iterator iter = route.params.begin(); iter != route.params.end(); ++iter) {
		if (iter->first == routeVar || iter->second.empty()) {
			route.params.erase(iter);
		}
	}

	route.path = CStringUtils::trim(route.path, "/");

	string url;
	CBaseUrlRule * rule = 0;

	string language = resolveLanguage(route.params);

	for (TUrlRulesList::const_iterator iter = _rules.begin(); iter != _rules.end(); ++iter) {
		rule = *iter;
		url = rule->createUrl(this, route, ampersand);
		if (!url.empty()) {
			url = CStringUtils::ltrim(url, "/");
			if (useLangBasedUrls) {
				url = "/" + language + "/" + url;
			}
			if (showScriptName) {
				url = "/" + getScriptName() + url;
			}
			url = CStringUtils::ltrim(url, "/");
			if (rule->hasHostInfo) {
				return _baseUrl + "/" + (url.empty() ? "/" : url);
			} else {
				return _baseUrl + "/" + url;
			}
		}
	}
	url = _baseUrl;
	if (showScriptName) {
		url += "/" + getScriptName();
	}

	if (!useLangBasedUrls) {
		return url + "/" + CStringUtils::ltrim(createUrlDefault(route, ampersand), "/");
	}

	if (language != Jvibetto::app()->getLanguage() || appendLanguageWhenItIsDefault) {
		url += "/" + language;
	}

	return "/" + (url.empty() ? "" : CStringUtils::trim(url, "/") + "/")
		+ CStringUtils::ltrim(createUrlDefault(route, ampersand), "/");
}

string CUrlManager::createUrlDefault(TRouteStruct & route, const string & ampersand) const
{
	string ret;
	if (urlFormat == FORMAT_PATH) {
		ret = CStringUtils::rtrim(route.path, "/");
		if (appendParams) {
			ret = CStringUtils::rtrim(ret + "/" + createPathInfo(route.params, "/", "/"), "/");
			return route.path.empty() ? ret : ret + urlSuffix;
		} else {
			if (!route.path.empty()) {
				ret += urlSuffix;
			}
			string query = createPathInfo(route.params, "=", ampersand);
			return query.empty() ? ret : ret + "?" + query;
		}
	} else {
		string query = createPathInfo(route.params, "=", ampersand);
		if (!route.path.empty()) {
			ret += "?" + routeVar + '=' + route.path;
			if (!query.empty()) {
				ret += ampersand + query;
			}
		} else if (!query.empty()) {
			ret += "?" + query;
		}
		return ret;
	}
}

string CUrlManager::trimScriptName(const string &path)
{
	string scriptName = resolveScriptName();
	std::string::size_type pos = path.find("/" + scriptName);
	if (pos == ::string::npos) {
		return path;
	}
	return path.substr(pos + scriptName.length() + 1);
}

string CUrlManager::resolveLanguage(TRequestParams & params) const
{
	if (!useLangBasedUrls) {
		return _defaultLanguage;
	}
	string language = "";
	TRequestParams::const_iterator paramFound = params.find(languageParamName);
	if (paramFound != params.end()) {
		language = paramFound->second;
	} else if (storeLanguageInCookies) {
		CCookieCollection & cookies = dynamic_cast<CHttpRequest*>((Jvibetto::app()->getComponent("request")))->getCookies();
		CCookieCollection::const_iterator cookieFound = cookies.find(languageCookieName);
		if (cookieFound != cookies.end()) {
			language = cookieFound->second.value;
		}
	}
	if (language.empty()) {
		language = _defaultLanguage;
	}
	if (params.find(languageParamName) != params.end()) {
		params.erase(languageParamName);
	}
	return language;
}

string CUrlManager::getScriptName() const
{
	return _scriptName;
}

string CUrlManager::resolveScriptName()
{
	string ret = Jvibetto::app()->getArguments()[0];
	std::string::size_type pos = ret.find_last_of("/");
	return pos == ::string::npos ? "" : ret.substr(pos + 1);
}

string CUrlManager::getScriptUrl()
{
	if (_scriptUrl.empty()) {
		CWebApplication * app = dynamic_cast<CWebApplication *>(Jvibetto::app());
		string documentRoot = app->getRequest()->getEnvVar("DOCUMENT_ROOT");
		string executable(app->getExecutablePath().c_str());
		std::string::size_type pos = executable.find(documentRoot);
		if (pos == ::string::npos) {
			throw CHttpException(400, "Bad Request or wrong web server configuration");
		} else {
			return _scriptUrl = executable.substr(documentRoot.length());
		}
	}
	return _scriptUrl;
}

string CUrlManager::createPathInfo(const TRequestParams & params, const string &equal, const string &ampersand) const
{
	vector<string> pairs;
	for (TRequestParams::const_iterator iter = params.begin(); iter != params.end(); ++iter) {
		if (iter->second.empty()) {
			continue;
		}
		pairs.push_back(
			CStringUtils::urlEncode(iter->first) + equal + CStringUtils::urlEncode(iter->second)
		);
	}
	return CStringUtils::implode(ampersand, pairs);
}

string CUrlManager::parseUrl(CHttpRequest * const request) const
{
	if (useLangBasedUrls) {
		CApplication * app = Jvibetto::app();
		string rawPathInfo = request->getPathInfo();
		string language = "";
		boost::smatch matches;
		string regexpStr = "(" + CStringUtils::implode("|", app->getLanguages()) + ")/?(.*)";
#ifdef JV_DEBUG
		Jvibetto::trace("CUrlManager::parseUrl trying to identify language from query. Regexp: " + regexpStr);
#endif
		boost::regex regexp(regexpStr, boost::regex_constants::normal);
		if (boost::regex_match(rawPathInfo, matches, regexp)) {
			app->setLanguage(language = matches[1]);
			if (storeLanguageInCookies) {
				CHttpCookie cookie(languageCookieName, matches[1]);
				CHttpResponse * response = dynamic_cast<CHttpResponse*>(app->getComponent("response"));
				response->addCookie(cookie);
			}
			request->setPathInfo(matches[2]);
		} else if (storeLanguageInCookies) {
			CCookieCollection & cookies = request->getCookies();
			CCookieCollection::const_iterator cookieFound = cookies.find(languageCookieName);
			if (cookieFound != cookies.end()) {
				app->setLanguage(language = cookieFound->second.value);
			}
		}
		if (!language.empty()) {
			request->setParam(languageParamName, language);
		}
	}

	if (urlFormat == FORMAT_PATH) {
		string rawPathInfo = request->getPathInfo();
		string pathInfo = removeUrlSuffix(rawPathInfo, urlSuffix);
		CBaseUrlRule * rule = 0;
		string route;
		for (TUrlRulesList::const_iterator iter = _rules.begin(); iter != _rules.end(); ++iter) {
			rule = *iter;
			route = rule->parseUrl(this, request, pathInfo, rawPathInfo);
			if (!route.empty()) {
				return request->hasParam(routeVar) ? request->getParam(routeVar) : route;
			}
		}
		if (useStrictParsing) {
			throw CHttpException(404, "Unable to resolve the request '" + pathInfo + "'.");
		} else {
			return pathInfo;
		}
	} else if(request->hasParam(routeVar)) {
		return request->getParam(routeVar);
	} else {
		return "";
	}
}

void CUrlManager::parsePathInfo(const string & pathInfo) const
{
	if (pathInfo.empty()) {
		return;
	}
	vector<string> segs = CStringUtils::explode("/", pathInfo + "/");
	int n = segs.size();
	string key;
	CHttpRequest * request = dynamic_cast<CWebApplication*>(Jvibetto::app())->getRequest();
	for (int i = 0; i < n - 1; i += 2) {
		key = segs[i];
		if (key.empty()) {
			continue;
		}
		request->setParam(key, segs[i + 1]);
	}
}

string CUrlManager::removeUrlSuffix(const string & pathInfo, const string & urlSuffix) const
{
	if (pathInfo.substr(pathInfo.length() - urlSuffix.length()) == urlSuffix) {
		return pathInfo.substr(0, pathInfo.length() - urlSuffix.length());
	} else {
		return pathInfo;
	}
}
