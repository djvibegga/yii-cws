/*
 * CClientScript.h
 *
 *  Created on: Feb 17, 2014
 *      Author: djvibegga
 */

#ifndef CCLIENTSCRIPT_H_
#define CCLIENTSCRIPT_H_

#include "base/CApplicationComponent.h"
#include "web/helpers/CHtml.h"

typedef map<string, string> TScriptMap;
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

struct CssBlock
{
	_string media;
	_string code;
};
typedef CssBlock TCssBlock;

typedef map<string, TScriptPackage> TScriptPackageMap;
typedef map<string, _string> TInlineJavascriptCodeMap;
typedef map<int, TInlineJavascriptCodeMap> TInlineJavascriptMap;
typedef map<string, TCssBlock> TInlineCssMap;
typedef map<string, _string> TClientFileMap;
typedef map<int, TClientFileMap> TJavascriptFileMap;
typedef map<string, _string> TCssFileMap;
typedef list<TTagAttributesMap> TMetaTagsList;
typedef list<TTagAttributesMap> TLinkTagsList;

class CClientScript: public CApplicationComponent
{
private:
	string _baseUrl;

public:
	static const int POS_HEAD;
	static const int POS_BEGIN;
	static const int POS_END;
	static const int POS_LOAD;
	static const int POS_READY;

	bool enableJavaScript;
	TScriptMap scriptMap;
	TInlineJavascriptMap scripts;

	CClientScript(CModule * module);
	CClientScript(const string &id, CModule * module);
	virtual void init();

	void reset();
	void render(_string & output);
	void setCoreScriptUrl(const string & url);
	string getCoreScriptUrl() const;
	CClientScript & registerCoreScript(const string & name);
	virtual void renderCoreScripts();
	string getPackageBaseUrl(const string & name);
	CClientScript & registerPackage(const string & name);
	CClientScript & registerScriptFile(const string & url, int position = POS_HEAD);
	CClientScript & registerScript(const string & id, const _string & script, int position = POS_READY);
	CClientScript & registerCssFile(const string & url, const _string & media = _(""));
	CClientScript & registerCss(const string & id, const _string & css, const _string & media = _(""));
	CClientScript & addPackage(const string & name, const TScriptPackage & definition);
	bool isCssFileRegistered(const string & url) const;
	bool isCssRegistered(const string & id) const;
	bool isScriptRegistered(const string & id, int position = POS_READY) const;
	bool isScriptFileRegistered(const string & url, int position = POS_HEAD) const;
	CClientScript & registerMetaTag(
		const _string & content,
		const _string & name = _(""),
		const _string & httpEquiv = _(""),
		const TTagAttributesMap & options = TTagAttributesMap());
	CClientScript & registerLinkTag(
		const _string & relation = _(""),
		const _string & type = _(""),
		const _string & href = _(""),
		const _string & media = _(""),
		const TTagAttributesMap & options = TTagAttributesMap());
	virtual void renderHead(_string & output);
	virtual void renderBodyBegin(_string & output);
	virtual void renderBodyEnd(_string & output);

protected:
	bool hasScripts;
	TScriptPackageMap packages;
	TScriptPackageMap coreScripts;
	TInlineCssMap css;
	TJavascriptFileMap scriptFiles;
	TCssFileMap cssFiles;
	TMetaTagsList metaTags;
	TLinkTagsList linkTags;

	virtual string resolveCoreScriptUrl() const;
	virtual void unifyScripts();
	virtual void remapScripts();
	virtual _string renderScriptBatch(const TInlineJavascriptCodeMap & scripts);
};

#endif /* CCLIENTSCRIPT_H_ */
