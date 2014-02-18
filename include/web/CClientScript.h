/*
 * CClientScript.h
 *
 *  Created on: Feb 17, 2014
 *      Author: djvibegga
 */

#ifndef CCLIENTSCRIPT_H_
#define CCLIENTSCRIPT_H_

#include "base/CApplicationComponent.h"

typedef list<string> TScriptPackageDependencyList;
typedef list<string> TScriptPackageJavascriptList;
typedef list<string> TScriptPackageCssList;

struct ScriptPackage
{
	string basePath;
	string baseUrl;
	TScriptPackageCssList css;
	TScriptPackageJavascriptList js;
	TScriptPackageDependencyList depends;
};
typedef ScriptPackage TScriptPackage;
typedef map<string, TScriptPackage> TScriptPackageMap;

class CClientScript: public CApplicationComponent
{
private:
	string _baseUrl;

public:

	CClientScript(CModule * module);
	CClientScript(const string &id, CModule * module);
	virtual void init();

	void setCoreScriptUrl(const string & url);
	string getCoreScriptUrl() const;

	string getPackageBaseUrl(const string & name);
	CClientScript & addPackage(const string & name, const TScriptPackage & definition);

protected:
	TScriptPackageMap packages;
	virtual string resolveCoreScriptUrl() const;
};

#endif /* CCLIENTSCRIPT_H_ */
