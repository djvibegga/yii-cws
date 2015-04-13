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
#include "utils/insert_orderer_map.h"

typedef vector<string> TScriptPackageDependencyList;
typedef vector<string> TScriptPackageJavascriptList;
typedef vector<string> TScriptPackageCssList;

struct ScriptPackage
{
	string basePath;
	string baseUrl;
	TScriptPackageCssList css;
	TScriptPackageJavascriptList js;
	TScriptPackageDependencyList depends;
};
typedef ScriptPackage TScriptPackage;
typedef insert_ordered_map<string, TScriptPackage> TScriptPackageUnorderedMap;
typedef map<string, TScriptPackage> TScriptPackageMap;

typedef map<string, string> TScriptMap;

struct CssBlock
{
	string media;
	string code;
};
typedef CssBlock TCssBlock;

typedef insert_ordered_map<string, string> TInlineJavascriptCodeMap;
typedef map<int, TInlineJavascriptCodeMap> TInlineJavascriptMap;
typedef insert_ordered_map<string, TCssBlock> TInlineCssMap;
typedef insert_ordered_map<string, string> TClientFileMap;
typedef map<int, TClientFileMap> TJavascriptFileMap;
typedef insert_ordered_map<string, string> TCssFileMap;
typedef vector<TTagAttributesMap> TMetaTagsList;
typedef vector<TTagAttributesMap> TLinkTagsList;

class CClientScript: public CApplicationComponent
{
private:
	string _baseUrl;

public:

	CClientScript(CModule * module);
	CClientScript(const string &id, CModule * module);
	virtual string getClassName() const;
	virtual void init();

	void setCoreScriptUrl(const string & url);
	string getCoreScriptUrl() const;

	string getPackageBaseUrl(const string & name, const TScriptPackage & package) const;
	CClientScript & addPackage(const string & name, TScriptPackage & definition);

	static const int POS_HEAD;
	static const int POS_BEGIN;
	static const int POS_END;
	static const int POS_LOAD;
	static const int POS_READY;

	bool enableJavaScript;
	TScriptMap scriptMap;
	TInlineJavascriptMap scripts;

	void reset();
	void loadPackages(const boost::filesystem::path & from, TScriptPackageMap & dest) throw (CException);
	void loadPackages(const xml_node & root, TScriptPackageMap & dest) throw (CException);

	CClientScript & registerCoreScript(const string & name);
	virtual void renderCoreScripts();
	CClientScript & registerPackage(const string & name);
	CClientScript & registerScriptFile(const string & url, int position = POS_HEAD);
	CClientScript & registerScript(const string & id, const string & script, int position = POS_READY);
	CClientScript & registerCssFile(const string & url, const string & media = "");
	CClientScript & registerCss(const string & id, const string & css, const string & media = "");
	CClientScript & registerMetaTag(
		const string & content,
		const string & name = "",
		const string & httpEquiv = "",
		const TTagAttributesMap & options = TTagAttributesMap());
	CClientScript & registerLinkTag(
		const string & relation = "",
		const string & type = "",
		const string & href = "",
		const string & media = "",
		const TTagAttributesMap & options = TTagAttributesMap());

	void render(string & output);
	virtual void renderHead(string & output);
	virtual void renderBodyBegin(string & output);
	virtual void renderBodyEnd(string & output);

	bool isCssFileRegistered(const string & url) const;
	bool isCssRegistered(const string & id) const;
	bool isScriptRegistered(const string & id, int position = POS_READY) const;
	bool isScriptFileRegistered(const string & url, int position = POS_HEAD) const;

	virtual void applyConfig(const xml_node & config);

protected:
	TScriptPackageMap packages;
	TScriptPackageMap corePackages;
	bool hasScripts;
	TScriptPackageUnorderedMap coreScripts;
	TInlineCssMap css;
	TJavascriptFileMap scriptFiles;
	TCssFileMap cssFiles;
	TMetaTagsList metaTags;
	TLinkTagsList linkTags;

	virtual string resolveCoreScriptUrl() const;
	virtual void unifyScripts();
	virtual void remapScripts();
	virtual string renderScriptBatch(const TInlineJavascriptCodeMap & scripts);
};

#endif /* CCLIENTSCRIPT_H_ */
