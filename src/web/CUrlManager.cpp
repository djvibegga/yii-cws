/*
 * CUrlManager.cpp
 *
 *  Created on: Oct 19, 2013
 *      Author: djvibegga
 */

#include "web/CUrlManager.h"
#include <base/CStringUtils.h>
#include <base/YiiBase.h>

CUrlRule::CUrlRule()
: hasHostInfo(false)
{

}

CUrlRule::~CUrlRule()
{
}

string CUrlRule::createUrl(
		const CUrlManager * const manager,
		TRouteStruct &route, const string & ampersand) const
{
	return "";
}


CUrlManager::CUrlManager(const CModule * module)
: CApplicationComponent("", module),
  _urlFormat(FORMAT_GET),
  _baseUrl("/"),
  showScriptName(true),
  appendParams(false),
  routeVar("r"),
  matchValue(false),
  useStrictParsing(false),
  caseSensitive(true)
{
}

CUrlManager::~CUrlManager()
{
	for (TUrlRulesList::iterator iter = _rules.begin(); iter != _rules.end(); ++iter) {
		delete *iter;
	}
}

void CUrlManager::init()
{
	_scriptName = resolveScriptName();
	initRules();
}

void CUrlManager::addRule(CUrlRule * rule)
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
	CUrlRule * rule = 0;
	for (TUrlRulesList::const_iterator iter = _rules.begin(); iter != _rules.end(); ++iter) {
		rule = *iter;
		url = rule->createUrl(this, route, ampersand);
		if (!url.empty()) {
			if (rule->hasHostInfo) {
				return url.empty() ? "/" : url;
			} else {
				return _baseUrl + "/" + url;
			}
		}
	}
	return createUrlDefault(route, ampersand);
}

string CUrlManager::createUrlDefault(TRouteStruct & route, const string & ampersand) const
{
	string ret;
	if (_urlFormat == FORMAT_PATH) {
		ret = CStringUtils::rtrim(_baseUrl + "/" + route.path, "/");
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
		ret = _baseUrl;
		if (showScriptName) {
			ret += getScriptName();
		}
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
	unsigned int pos = path.find("/" + scriptName);
	if (pos == ::string::npos) {
		return path;
	}
	return path.substr(pos + scriptName.length() + 1);
}

string CUrlManager::getScriptName() const
{
	return _scriptName;
}

string CUrlManager::resolveScriptName()
{
	string ret = YiiBase::app()->getArguments()[0];
	unsigned int pos = ret.find_last_of("/");
	return pos == ::string::npos ? "" : ret.substr(pos + 1);
}

string CUrlManager::createPathInfo(const TRequestParams & params, const string &equal, const string &ampersand, const string & key) const
{
	vector<string> pairs;
	string k;
	for (TRequestParams::const_iterator iter = params.begin(); iter != params.end(); ++iter) {
		if (iter->second.empty()) {
			continue;
		}
		if (!key.empty()) {
			k = key + "[" + k + "]";
		}

		/*if (is_array($v))
			$pairs[]=$this->createPathInfo($v,$equal,$ampersand, $k);
		else*/
		pairs.push_back(
			CStringUtils::urlEncode(iter->first) + equal + CStringUtils::urlEncode(iter->second)
		);
	}
	return CStringUtils::implode(ampersand, pairs);
}

string CUrlManager::parseUrl(CHttpRequest * const request) const
{
	return "";
	/*if($this->getUrlFormat()===self::PATH_FORMAT)
	{
		$rawPathInfo=$request->getPathInfo();
		$pathInfo=$this->removeUrlSuffix($rawPathInfo,$this->urlSuffix);
		foreach($this->_rules as $i=>$rule)
		{
			if(is_array($rule))
				$this->_rules[$i]=$rule=Yii::createComponent($rule);
			if(($r=$rule->parseUrl($this,$request,$pathInfo,$rawPathInfo))!==false)
				return isset($_GET[$this->routeVar]) ? $_GET[$this->routeVar] : $r;
		}
		if($this->useStrictParsing)
			throw new CHttpException(404,Yii::t('yii','Unable to resolve the request "{route}".',
				array('{route}'=>$pathInfo)));
		else
			return $pathInfo;
	}
	elseif(isset($_GET[$this->routeVar]))
		return $_GET[$this->routeVar];
	elseif(isset($_POST[$this->routeVar]))
		return $_POST[$this->routeVar];
	else
		return '';*/
}
