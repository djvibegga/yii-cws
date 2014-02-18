/*
 * CClientScriptSession.h
 *
 *  Created on: Feb 18, 2014
 *      Author: djvibegga
 */

#ifndef CCLIENTSCRIPTBUFFER_H_
#define CCLIENTSCRIPTBUFFER_H_

#include "web/CClientScript.h"
#include "web/helpers/CHtml.h"

typedef map<string, string> TScriptMap;

struct CssBlock
{
	_string media;
	_string code;
};
typedef CssBlock TCssBlock;

typedef map<string, _string> TInlineJavascriptCodeMap;
typedef map<int, TInlineJavascriptCodeMap> TInlineJavascriptMap;
typedef map<string, TCssBlock> TInlineCssMap;
typedef map<string, _string> TClientFileMap;
typedef map<int, TClientFileMap> TJavascriptFileMap;
typedef map<string, _string> TCssFileMap;
typedef list<TTagAttributesMap> TMetaTagsList;
typedef list<TTagAttributesMap> TLinkTagsList;

class CClientScriptBuffer: public CComponent
{
private:
	CClientScript * _cs;

public:
	static const int POS_HEAD;
	static const int POS_BEGIN;
	static const int POS_END;
	static const int POS_LOAD;
	static const int POS_READY;

	bool enableJavaScript;
	TScriptMap scriptMap;
	TInlineJavascriptMap scripts;

	CClientScriptBuffer(CClientScript * clientScript);
	virtual ~CClientScriptBuffer();

	void reset();
	CClientScriptBuffer & registerCoreScript(const string & name);
	virtual void renderCoreScripts();
	CClientScriptBuffer & registerPackage(const string & name);
	CClientScriptBuffer & registerScriptFile(const string & url, int position = POS_HEAD);
	CClientScriptBuffer & registerScript(const string & id, const _string & script, int position = POS_READY);
	CClientScriptBuffer & registerCssFile(const string & url, const _string & media = _(""));
	CClientScriptBuffer & registerCss(const string & id, const _string & css, const _string & media = _(""));
	CClientScriptBuffer & registerMetaTag(
		const _string & content,
		const _string & name = _(""),
		const _string & httpEquiv = _(""),
		const TTagAttributesMap & options = TTagAttributesMap());
	CClientScriptBuffer & registerLinkTag(
		const _string & relation = _(""),
		const _string & type = _(""),
		const _string & href = _(""),
		const _string & media = _(""),
		const TTagAttributesMap & options = TTagAttributesMap());

	void render(_string & output);
	virtual void renderHead(_string & output);
	virtual void renderBodyBegin(_string & output);
	virtual void renderBodyEnd(_string & output);

	bool isCssFileRegistered(const string & url) const;
	bool isCssRegistered(const string & id) const;
	bool isScriptRegistered(const string & id, int position = POS_READY) const;
	bool isScriptFileRegistered(const string & url, int position = POS_HEAD) const;

protected:
	bool hasScripts;
	TScriptPackageMap coreScripts;
	TInlineCssMap css;
	TJavascriptFileMap scriptFiles;
	TCssFileMap cssFiles;
	TMetaTagsList metaTags;
	TLinkTagsList linkTags;

	virtual void unifyScripts();
	virtual void remapScripts();
	virtual _string renderScriptBatch(const TInlineJavascriptCodeMap & scripts);
};

#endif /* CCLIENTSCRIPTBUFFER_H_ */
