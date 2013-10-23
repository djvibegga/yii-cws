/*
 * CUrlManager.h
 *
 *  Created on: Oct 19, 2013
 *      Author: djvibegga
 */

#ifndef CURLMANAGER_H_
#define CURLMANAGER_H_

#include "base/CApplicationComponent.h"
#include <web/CHttpRequest.h>
#include <vector>
#include <string>

using namespace std;

class CUrlManager;

enum UrlManagerFormat { FORMAT_GET, FORMAT_PATH };


class CBaseUrlRule: public CComponent
{
public:
	bool hasHostInfo;
	virtual string createUrl(
			const CUrlManager * const manager,
			TRouteStruct &route, const string & ampersand = "&") const = 0;
	virtual string parseUrl(
		const CUrlManager * const manager,
		CHttpRequest * const request,
		const string & pathInfo, const string & rawPathInfo) const = 0;
};


typedef map<string, string> TRouteReferencesMap;
typedef vector<string> TUrlRuleParamList;

class CUrlRule: public CBaseUrlRule
{
private:
	TRouteReferencesMap _references;
	TRouteReferencesMap _params;
	string _urlSuffix;

public:
	bool hasHostInfo;
	bool caseSensitive;
	TRequestParams defaultParams;
	bool matchValue;
	string verb;
	bool parsingOnly;
	string route;
	string routePattern;
	string pattern;
	string templateStr;
	bool append;

	CUrlRule(const string & routeExp, const string & patternExp);
	virtual ~CUrlRule();
	virtual string createUrl(
		const CUrlManager * const manager,
		TRouteStruct &route, const string & ampersand = "&") const;
	virtual string parseUrl(
		const CUrlManager * const manager,
		CHttpRequest * const request,
		const string & pathInfo, const string & rawPathInfo) const;
};

typedef vector<CBaseUrlRule*> TUrlRulesList;

class CUrlManager: public CApplicationComponent
{
private:
	string _baseUrl;
	TUrlRulesList _rules;
	string _scriptName;
	string _scriptUrl;

public:
	UrlManagerFormat urlFormat;
	string urlSuffix;
	bool showScriptName;
	bool appendParams;
	string routeVar;
	bool matchValue;
	bool useStrictParsing;
	bool caseSensitive;

	CUrlManager(const CModule * module = 0);
	virtual ~CUrlManager();
	virtual void init();
	virtual void addRule(CBaseUrlRule * rule);
	virtual string createUrl(TRouteStruct &route, const string & ampersand = "&") const;
	virtual string createPathInfo(
		const TRequestParams & $params,
		const string &equal, const string &ampersand) const;
	virtual string parseUrl(CHttpRequest * const request) const;
	virtual void parsePathInfo(const string & pathInfo) const;
	string getScriptName() const;
	string getScriptUrl();
	virtual string removeUrlSuffix(const string & pathInfo, const string & urlSuffix) const;

protected:
	virtual void initRules();
	virtual string createUrlDefault(TRouteStruct & route, const string & ampersand) const;
	virtual string resolveScriptName();
	virtual string trimScriptName(const string & path);
};

#endif /* CURLMANAGER_H_ */