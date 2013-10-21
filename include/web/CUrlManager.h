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

class CUrlRule: public CComponent
{
public:
	bool hasHostInfo;

	CUrlRule();
	virtual ~CUrlRule();
	virtual string createUrl(
		const CUrlManager * const manager,
		TRouteStruct &route, const string & ampersand = "&") const;
};

typedef vector<CUrlRule*> TUrlRulesList;

class CUrlManager: public CApplicationComponent
{
private:
	UrlManagerFormat _urlFormat;
	string _baseUrl;
	TUrlRulesList _rules;
	string _scriptName;

public:
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
	virtual void addRule(CUrlRule * rule);
	virtual string createUrl(TRouteStruct &route, const string & ampersand = "&") const;
	virtual string createPathInfo(
		const TRequestParams & $params,
		const string &equal, const string &ampersand,
		const string & key = "") const;
	virtual string parseUrl(CHttpRequest * const request) const;
	string getScriptName() const;

protected:
	virtual void initRules();
	virtual string createUrlDefault(TRouteStruct & route, const string & ampersand) const;
	virtual string trimScriptName(const string & path);
	virtual string resolveScriptName();
};

#endif /* CURLMANAGER_H_ */
