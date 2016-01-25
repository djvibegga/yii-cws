/*
 * CClientScript.cpp
 *
 *  Created on: Feb 17, 2014
 *      Author: djvibegga
 */

#include "web/CClientScript.h"
#include "web/CAssetManager.h"
#include "web/CWebApplication.h"
#include "base/Cws.h"
#include "base/CStringUtils.h"
#include "base/CProfiler.h"
#include <boost/regex.hpp>

using namespace std;

const int CClientScript::POS_HEAD = 0;
const int CClientScript::POS_BEGIN = 1;
const int CClientScript::POS_END = 2;
const int CClientScript::POS_LOAD = 3;
const int CClientScript::POS_READY = 4;

CClientScript::CClientScript(CModule * module)
: CApplicationComponent("clientScript", module),
  enableJavaScript(true),
  hasScripts(false)
{
}

CClientScript::CClientScript(const string &id, CModule * module)
: CApplicationComponent(id, module),
  enableJavaScript(true),
    hasScripts(false)
{
}

string CClientScript::getClassName() const
{
	return "CClientScript";
}

void CClientScript::init()
{
	CApplicationComponent::init();
	_baseUrl = resolveCoreScriptUrl();
	loadPackages(
		boost::filesystem::path(CWS_PATH + string("/web/js/assets.xml")),
		corePackages
	);
}

void CClientScript::applyConfig(const xml_node & config)
{
	if (!config.child("packages").empty()) {
		loadPackages(config.child("packages"), packages);
	}
}

void CClientScript::setCoreScriptUrl(const string & url)
{
	_baseUrl = url;
}

string CClientScript::getCoreScriptUrl() const
{
	return _baseUrl;
}

string CClientScript::resolveCoreScriptUrl() const
{
	CAssetManager * am = dynamic_cast<CAssetManager*>(Cws::app()->getComponent("assetManager"));
	boost::filesystem::path srcPath(
		boost::filesystem::canonical(
			boost::filesystem::path(boost::filesystem::path(CWS_PATH))
		)
	);
	return am->publish(srcPath.string() + "/web/js/source");
}

string CClientScript::getPackageBaseUrl(const string & name, const TScriptPackage & package) const
{
	PROFILE_BEGIN("begin CClientScript::getPackageBaseUrl(" + name + ")");
	string baseUrl;
	CWebApplication * app = dynamic_cast<CWebApplication*>(Cws::app());
	if (!package.baseUrl.empty()) {
		baseUrl = package.baseUrl;
		if (baseUrl.empty() || (baseUrl.find("/") != 0 && baseUrl.find("://") == ::string::npos)) {
			baseUrl = app->getRequest()->getBaseUrl() + "/" + baseUrl;
		}
		baseUrl = CStringUtils::rtrim(baseUrl, "/");
	} else if (!package.basePath.empty()) {
		baseUrl = app->getAssetManager()->publish(
			Cws::getPathOfAlias(package.basePath)
		);
	} else {
		baseUrl = getCoreScriptUrl();
	}
	PROFILE_END();
	return baseUrl;
}

CClientScript & CClientScript::addPackage(const string & name, TScriptPackage & definition)
{
	definition.baseUrl = getPackageBaseUrl(name, definition);
	packages[name] = definition;
	return *this;
}

void CClientScript::reset()
{
	PROFILE_BEGIN("CClientScript::reset()");
	hasScripts = false;
	cssFiles = TCssFileMap();
	css = TInlineCssMap();
	scriptFiles = TJavascriptFileMap();
	scripts = TInlineJavascriptMap();
	metaTags = TMetaTagsList();
	linkTags = TLinkTagsList();
	coreScripts = TScriptPackageUnorderedMap();
	//recordCachingAction("clientScript", "reset", array());
	PROFILE_END();
}

void CClientScript::loadPackages(const boost::filesystem::path & from, TScriptPackageMap & dest) throw (CException)
{
	xml_document document;
	xml_parse_result result;
	result = document.load_file(from.string().c_str());
	if (result.status != status_ok) {
		stringstream ss;
		ss << "Can't parse clientScript packages config: '" << from << "'.";
		throw CException(ss.str());
	}
	loadPackages(document.root().child("packages"), dest);
}

void CClientScript::loadPackages(const xml_node & root, TScriptPackageMap & dest) throw (CException)
{
	xml_object_range<xml_named_node_iterator> xmlPackages = root.children("package");
	for (xml_object_range<xml_named_node_iterator>::const_iterator iter = xmlPackages.begin(); iter != xmlPackages.end(); ++iter) {
		TScriptPackage package;
		if (!iter->child("basePath").empty()) {
			package.basePath = iter->child("basePath").attribute("value").as_string();
		}
		if (!iter->child("baseUrl").empty()) {
			package.basePath = iter->child("baseUrl").attribute("value").as_string();
		}
		if (!iter->child("js").empty()) {
			xml_object_range<xml_named_node_iterator> xmlPackageFiles = iter->child("js").children("file");
			for (xml_object_range<xml_named_node_iterator>::const_iterator jsIter = xmlPackageFiles.begin(); jsIter != xmlPackageFiles.end(); ++jsIter) {
				package.js.push_back(jsIter->attribute("path").as_string());
			}
		}
		if (!iter->child("css").empty()) {
			xml_object_range<xml_named_node_iterator> xmlPackageFiles = iter->child("css").children("file");
			for (xml_object_range<xml_named_node_iterator>::const_iterator cssIter = xmlPackageFiles.begin(); cssIter != xmlPackageFiles.end(); ++cssIter) {
				package.css.push_back(cssIter->attribute("path").as_string());
			}
		}
		if (!iter->child("depends").empty()) {
			xml_object_range<xml_named_node_iterator> xmlPackageDependencies = iter->child("depends").children("package");
			for (xml_object_range<xml_named_node_iterator>::const_iterator depIter = xmlPackageDependencies.begin(); depIter != xmlPackageDependencies.end(); ++depIter) {
				package.depends.push_back(depIter->attribute("name").as_string());
			}
		}
		string name = iter->attribute("name").as_string();
		package.baseUrl = getPackageBaseUrl(name, package);
		dest[name] = package;
	}
}

void CClientScript::render(string & output)
{
	PROFILE_BEGIN("CClientScript::render()");
	if (!hasScripts) {
		return;
	}
	renderCoreScripts();
	if (!scriptMap.empty()) {
		remapScripts();
	}
	unifyScripts();
	renderHead(output);
	if (enableJavaScript) {
		renderBodyBegin(output);
		renderBodyEnd(output);
	}
	PROFILE_END();
	reset();
}

void CClientScript::renderCoreScripts()
{
	PROFILE_BEGIN("CClientScript::renderCoreScripts()");
	if (coreScripts.empty()) {
		return;
	}
	TCssFileMap cssFiles;
	TClientFileMap jsFiles;
	TScriptPackage package;

	for (TScriptPackageUnorderedMap::iterator iter = coreScripts.begin(); iter != coreScripts.end(); ++iter) {
		package = iter.second();
		if (!package.js.empty()) {
			for (TScriptPackageJavascriptList::const_iterator jsIter = package.js.begin(); jsIter != package.js.end(); ++jsIter) {
				jsFiles.push(package.baseUrl + "/" + *jsIter, "text/javascript");
			}
		}
		if (!package.css.empty()) {
			for (TScriptPackageCssList::const_iterator cssIter = package.css.begin(); cssIter != package.css.end(); ++cssIter) {
				cssFiles.push(package.baseUrl + "/" + *cssIter, "");
			}
		}
	}

	if (!jsFiles.empty()) {
		TJavascriptFileMap::const_iterator scriptsFound = this->scriptFiles.find(POS_HEAD);
		if (scriptsFound != this->scriptFiles.end()) {
			for (TClientFileMap::iterator iter = scriptsFound->second.begin(); iter != scriptsFound->second.end(); ++iter) {
				jsFiles.push(iter.first(), iter.second());
			}
		}
		this->scriptFiles[POS_HEAD] = jsFiles;
	}

	if (!cssFiles.empty()) {
		for (TCssFileMap::iterator iter = this->cssFiles.begin(); iter != this->cssFiles.end(); ++iter) {
			cssFiles.push(iter.first(), iter.second());
		}
		this->cssFiles = cssFiles;
	}

	PROFILE_END();
}

CClientScript & CClientScript::registerCoreScript(const string & name)
{
	PROFILE_BEGIN("CClientScript::registerCoreScript(" + name + ")");

	if (coreScripts.find(name) != coreScripts.end()) {
		return *this;
	}
	TScriptPackage package;
	bool isPackagePresented = false;
	TScriptPackageMap::const_iterator packageFound = packages.find(name);
	if (packageFound != packages.end()) {
		package = packageFound->second;
		isPackagePresented = true;
	} else {
		TScriptPackageMap::const_iterator corePackageFound = corePackages.find(name);
		if (corePackageFound != corePackages.end()) {
			package = corePackageFound->second;
			isPackagePresented = true;
		}
	}
	if (isPackagePresented) {
		if (!package.depends.empty()) {
			for (TScriptPackageDependencyList::const_iterator iter = package.depends.begin(); iter != package.depends.end(); ++iter) {
				registerCoreScript(*iter);
			}
		}
		coreScripts.push(name, package);
		hasScripts = true;
//		$params=func_get_args();
//		$this->recordCachingAction('clientScript','registerCoreScript',$params);
	}

	PROFILE_END();
	return *this;
}

CClientScript & CClientScript::registerPackage(const string & name)
{
	return registerCoreScript(name);
}

CClientScript & CClientScript::registerScriptFile(const string & url, int position)
{
	hasScripts = true;
	TJavascriptFileMap::const_iterator found = scriptFiles.find(position);
	if (found == scriptFiles.end()) {
		scriptFiles[position] = TClientFileMap();
	}
	scriptFiles[position].push(url, "text/javascript");
//	$params=func_get_args();
//	$this->recordCachingAction('clientScript','registerScriptFile',$params);
	return *this;
}

CClientScript & CClientScript::registerScript(const string & id, const string & script, int position)
{
	hasScripts = true;
	TInlineJavascriptMap::const_iterator found = scripts.find(position);
	if (found == scripts.end()) {
		scripts[position] = TInlineJavascriptCodeMap();
	}
	scripts[position].push(id, script);
	if (position == POS_READY || position == POS_LOAD) {
		registerCoreScript("jquery");
	}
//	$params=func_get_args();
//	$this->recordCachingAction('clientScript','registerScript',$params);
	return *this;
}

CClientScript & CClientScript::registerCssFile(const string & url, const string & media)
{
	hasScripts = true;
	cssFiles.push(url, media);
	//$params=func_get_args();
	//$this->recordCachingAction('clientScript','registerCssFile',$params);
	return *this;
}

CClientScript & CClientScript::registerCss(const string & id, const string & css, const string & media)
{
	hasScripts = true;
	TCssBlock block;
	block.media = media;
	block.code = css;
	this->css.push(id, block);
	//$params=func_get_args();
	//$this->recordCachingAction('clientScript','registerCss',$params);
	return *this;
}

bool CClientScript::isCssFileRegistered(const string & url) const
{
	return cssFiles.find(url) != cssFiles.end();
}

bool CClientScript::isCssRegistered(const string & id) const
{
	return css.find(id) != css.end();
}

bool CClientScript::isScriptFileRegistered(const string & url, int position) const
{
	TJavascriptFileMap::const_iterator found = scriptFiles.find(position);
	if (found == scriptFiles.end()) {
		return false;
	}
	return found->second.find(url) != found->second.end();
}

bool CClientScript::isScriptRegistered(const string & id, int position) const
{
	TInlineJavascriptMap::const_iterator found = scripts.find(position);
	if (found == scripts.end()) {
		return false;
	}
	return found->second.find(id) != found->second.end();
}

void CClientScript::unifyScripts()
{
//	if(!$this->enableJavaScript)
//		return;
//	$map=array();
//	if(isset($this->scriptFiles[self::POS_HEAD]))
//		$map=$this->scriptFiles[self::POS_HEAD];
//
//	if(isset($this->scriptFiles[self::POS_BEGIN]))
//	{
//		foreach($this->scriptFiles[self::POS_BEGIN] as $scriptFile=>$scriptFileValue)
//		{
//			if(isset($map[$scriptFile]))
//				unset($this->scriptFiles[self::POS_BEGIN][$scriptFile]);
//			else
//				$map[$scriptFile]=true;
//		}
//	}
//
//	if(isset($this->scriptFiles[self::POS_END]))
//	{
//		foreach($this->scriptFiles[self::POS_END] as $key=>$scriptFile)
//		{
//			if(isset($map[$key]))
//				unset($this->scriptFiles[self::POS_END][$key]);
//		}
//	}
}

void CClientScript::remapScripts()
{
//	$cssFiles=array();
//	foreach($this->cssFiles as $url=>$media)
//	{
//		$name=basename($url);
//		if(isset($this->scriptMap[$name]))
//		{
//			if($this->scriptMap[$name]!==false)
//				$cssFiles[$this->scriptMap[$name]]=$media;
//		}
//		elseif(isset($this->scriptMap['*.css']))
//		{
//			if($this->scriptMap['*.css']!==false)
//				$cssFiles[$this->scriptMap['*.css']]=$media;
//		}
//		else
//			$cssFiles[$url]=$media;
//	}
//	$this->cssFiles=$cssFiles;
//
//	$jsFiles=array();
//	foreach($this->scriptFiles as $position=>$scriptFiles)
//	{
//		$jsFiles[$position]=array();
//		foreach($scriptFiles as $scriptFile=>$scriptFileValue)
//		{
//			$name=basename($scriptFile);
//			if(isset($this->scriptMap[$name]))
//			{
//				if($this->scriptMap[$name]!==false)
//					$jsFiles[$position][$this->scriptMap[$name]]=$this->scriptMap[$name];
//			}
//			elseif(isset($this->scriptMap['*.js']))
//			{
//				if($this->scriptMap['*.js']!==false)
//					$jsFiles[$position][$this->scriptMap['*.js']]=$this->scriptMap['*.js'];
//			}
//			else
//				$jsFiles[$position][$scriptFile]=$scriptFileValue;
//		}
//	}
//	$this->scriptFiles=$jsFiles;
}

void CClientScript::renderHead(string & output)
{
	PROFILE_BEGIN("CClientScript::renderHead()")
	string html;
	for (TMetaTagsList::const_iterator iter = metaTags.begin(); iter != metaTags.end(); ++iter) {
		html += CHtml::metaTag(iter->at("content"), "", "", *iter) + "\n";
	}
	for (TLinkTagsList::const_iterator iter = linkTags.begin(); iter != linkTags.end(); ++iter) {
		html += CHtml::linkTag("", "", "", "", *iter) + "\n";
	}
	for (TCssFileMap::iterator iter = cssFiles.begin(); iter != cssFiles.end(); ++iter) {
		html += CHtml::cssFile(iter.first(), iter.second()) + "\n";
	}
	TCssBlock block;
	for (TInlineCssMap::iterator iter = css.begin(); iter != css.end(); ++iter) {
		block = iter.second();
		html += CHtml::css(block.code, block.media) + "\n";
	}
	if (enableJavaScript) {
		TJavascriptFileMap::iterator filesFound = scriptFiles.find(POS_HEAD);
		if (filesFound != scriptFiles.end()) {
			for (TClientFileMap::iterator iter = filesFound->second.begin(); iter != filesFound->second.end(); ++iter) {
				html += CHtml::scriptFile(iter.first(), iter.second()) + "\n";
			}
		}
		TInlineJavascriptMap::const_iterator scriptsFound = scripts.find(POS_HEAD);
		if (scriptsFound != scripts.end()) {
			html += renderScriptBatch(scriptsFound->second);
		}
	}

	if (!html.empty()) {
		if (output.find("</head") == ::string::npos) {
			output = html + output;
		} else {
			output = boost::regex_replace(
				output,
				boost::regex("(</head\\s*>)"),
				"<#head#>$1"
			);
			string replacement = "<#head#>";
			::string::size_type pos = output.find(replacement);
			if (pos != ::string::npos) {
				output.replace(pos, replacement.length(), html);
			}
		}
	}

	PROFILE_END();
}

void CClientScript::renderBodyBegin(string & output)
{
	string html;

	TJavascriptFileMap::const_iterator filesFound = scriptFiles.find(POS_BEGIN);
	if (filesFound != scriptFiles.end()) {
		for (TClientFileMap::iterator iter = filesFound->second.begin(); iter != filesFound->second.end(); ++iter) {
			html += CHtml::scriptFile(iter.first(), iter.second()) + "\n";
		}
	}
	TInlineJavascriptMap::const_iterator scriptsFound = scripts.find(POS_BEGIN);
	if (scriptsFound != scripts.end()) {
		html += renderScriptBatch(scriptsFound->second);
	}

	if (!html.empty()) {
		if (output.find("<body") == ::string::npos) {
			output = html + output;
		} else {
			output = boost::regex_replace(
				output,
				boost::regex("(<body\\b[^>]*>)"),
				"$1<#begin#>"
			);
			string replacement = "<#begin#>";
			::string::size_type pos = output.find(replacement);
			if (pos != ::_string::npos) {
				output.replace(pos, replacement.length(), html);
			}
		}
	}
}

void CClientScript::renderBodyEnd(string & output)
{
	bool isFullPage = output.find("</body") != ::string::npos;
	output = boost::regex_replace(
		output,
		boost::regex("(</body\\s*>)"),
		"<#end#>$1"
	);
	string html;

	TJavascriptFileMap::const_iterator filesFound = scriptFiles.find(POS_END);
	if (filesFound != scriptFiles.end()) {
		for (TClientFileMap::iterator iter = filesFound->second.begin(); iter != filesFound->second.end(); ++iter) {
			html += CHtml::scriptFile(iter.first(), iter.second()) + "\n";
		}
	}

	TInlineJavascriptCodeMap batch;

	TInlineJavascriptMap::const_iterator endFound = scripts.find(POS_END);
	if (endFound != scripts.end()) {
		batch = endFound->second;
	}

	TInlineJavascriptMap::const_iterator readyFound = scripts.find(POS_READY);
	if (readyFound != scripts.end()) {
		string js = readyFound->second.begin().second();
		for (TInlineJavascriptCodeMap::iterator iter = ++readyFound->second.begin(); iter != readyFound->second.end(); ++iter) {
			js += "\n" + iter.second();
		}
		if (isFullPage) {
			js = "jQuery(function($) {\n" + js + "\n});";
		}
		batch.push("#CClientScript#ready#", js);
	}

	TInlineJavascriptMap::const_iterator loadFound = scripts.find(POS_LOAD);
	if (loadFound != scripts.end()) {
		string js = loadFound->second.begin().second();
		for (TInlineJavascriptCodeMap::iterator iter = ++loadFound->second.begin(); iter != loadFound->second.end(); ++iter) {
			js += "\n" + iter.second();
		}
		if (isFullPage) {
			js = "jQuery(window).on('load',function() {\n" + js + "\n});";
		}
		batch.push("#CClientScript#load#", js);
	}

	if (!batch.empty()) {
		html += renderScriptBatch(batch);
	}

	if (isFullPage) {
		string replacement = "<#end#>";
		string::size_type pos = output.find(replacement);
		if (pos != ::string::npos) {
			output.replace(pos, replacement.length(), html);
		}
	} else {
		output += html;
	}
}

string CClientScript::renderScriptBatch(const TInlineJavascriptCodeMap & scripts)
{
	string html;
	for (TInlineJavascriptCodeMap::iterator iter = scripts.begin(); iter != scripts.end(); ++iter) {
		html += CHtml::script(iter.second());
	}
	return html;
}

CClientScript & CClientScript::registerMetaTag(
	const string & content,
	const string & name,
	const string & httpEquiv,
	const TTagAttributesMap & options)
{
	hasScripts = true;
	TTagAttributesMap opts = options;
	if (!name.empty()) {
		opts["name"] = name;
	}
	if (!httpEquiv.empty()) {
		opts["http-equiv"] = httpEquiv;
	}
	opts["content"] = content;
	metaTags.push_back(opts);
//	$params=func_get_args();
//	$this->recordCachingAction('clientScript','registerMetaTag',$params);
	return *this;
}

CClientScript & CClientScript::registerLinkTag(
	const string & relation,
	const string & type,
	const string & href,
	const string & media,
	const TTagAttributesMap & options)
{
	hasScripts = true;
	TTagAttributesMap opts = options;
	if (!relation.empty()) {
		opts["rel"] = relation;
	}
	if (!type.empty()) {
		opts["type"] = type;
	}
	if (!href.empty()) {
		opts["href"] = href;
	}
	if (!media.empty()) {
		opts["media"] = media;
	}
	linkTags.push_back(opts);
//	$params=func_get_args();
//	$this->recordCachingAction('clientScript','registerLinkTag',$params);
	return *this;
}
