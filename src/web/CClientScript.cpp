/*
 * CClientScript.cpp
 *
 *  Created on: Feb 17, 2014
 *      Author: djvibegga
 */

#include "web/CClientScript.h"
#include "web/CAssetManager.h"
#include "base/Jvibetto.h"
#include "base/CStringUtils.h"
#include <boost/regex.hpp>

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

void CClientScript::init()
{
	CApplicationComponent::init();
	_baseUrl = resolveCoreScriptUrl();
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
	CAssetManager * am = dynamic_cast<CAssetManager*>(Jvibetto::app()->getComponent("assetManager"));
	boost::filesystem::path srcPath(
		boost::filesystem::canonical(
			boost::filesystem::path(Jvibetto::app()->getBasePath().string() + "/" + __FILE__).parent_path()
		)
	);
	return am->publish(srcPath.string() + "/js/source");
}

string CClientScript::getPackageBaseUrl(const string & name)
{
	TScriptPackageMap::const_iterator packageFound = coreScripts.find(name);
	if (packageFound == coreScripts.end()) {
		return "";
	}

	TScriptPackage package = packageFound->second;
	string baseUrl;
	CWebApplication * app = dynamic_cast<CWebApplication*>(Jvibetto::app());
	if (!package.baseUrl.empty()) {
		baseUrl = package.baseUrl;
//		if($baseUrl==='' || $baseUrl[0]!=='/' && strpos($baseUrl,'://')===false)
//			$baseUrl=Yii::app()->getRequest()->getBaseUrl().'/'.$baseUrl;
//		$baseUrl=rtrim($baseUrl,'/');
	} else if (!package.basePath.empty()) {
		baseUrl = app->getAssetManager()->publish()
			//->getAssgetAssetManager()->publish(Yii::getPathOfAlias($package['basePath']));
	} else {
		baseUrl = getCoreScriptUrl();
	}
//	return $this->coreScripts[$name]['baseUrl']=$baseUrl;
	return "";
}

CClientScript & CClientScript::addPackage(const string & name, const TScriptPackage & definition)
{
	packages[name] = definition;
	return *this;
}

void CClientScript::reset()
{
	hasScripts = false;
	cssFiles = TCssFileMap();
	css = TInlineCssMap();
	scriptFiles = TJavascriptFileMap();
	scripts = TInlineJavascriptMap();
	metaTags = TMetaTagsList();
	linkTags = TLinkTagsList();
	//recordCachingAction("clientScript", "reset", array());
}

void CClientScript::render(_string & output)
{
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

	reset();
}

void CClientScript::renderCoreScripts()
{
//	if($this->coreScripts===null)
//		return;
//	$cssFiles=array();
//	$jsFiles=array();
//	foreach($this->coreScripts as $name=>$package)
//	{
//		$baseUrl=$this->getPackageBaseUrl($name);
//		if(!empty($package['js']))
//		{
//			foreach($package['js'] as $js)
//				$jsFiles[$baseUrl.'/'.$js]=$baseUrl.'/'.$js;
//		}
//		if(!empty($package['css']))
//		{
//			foreach($package['css'] as $css)
//				$cssFiles[$baseUrl.'/'.$css]='';
//		}
//	}
//	// merge in place
//	if($cssFiles!==array())
//	{
//		foreach($this->cssFiles as $cssFile=>$media)
//			$cssFiles[$cssFile]=$media;
//		$this->cssFiles=$cssFiles;
//	}
//	if($jsFiles!==array())
//	{
//		if(isset($this->scriptFiles[$this->coreScriptPosition]))
//		{
//			foreach($this->scriptFiles[$this->coreScriptPosition] as $url => $value)
//				$jsFiles[$url]=$value;
//		}
//		$this->scriptFiles[$this->coreScriptPosition]=$jsFiles;
//	}
}

CClientScript & CClientScript::registerCoreScript(const string & name)
{
//	if(isset($this->coreScripts[$name]))
//		return $this;
//	if(isset($this->packages[$name]))
//		$package=$this->packages[$name];
//	else
//	{
//		if($this->corePackages===null)
//			$this->corePackages=require(YII_PATH.'/web/js/packages.php');
//		if(isset($this->corePackages[$name]))
//			$package=$this->corePackages[$name];
//	}
//	if(isset($package))
//	{
//		if(!empty($package['depends']))
//		{
//			foreach($package['depends'] as $p)
//				$this->registerCoreScript($p);
//		}
//		$this->coreScripts[$name]=$package;
//		$this->hasScripts=true;
//		$params=func_get_args();
//		$this->recordCachingAction('clientScript','registerCoreScript',$params);
//	}
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
	scriptFiles[position][url] = _("text/javascript");
//	$params=func_get_args();
//	$this->recordCachingAction('clientScript','registerScriptFile',$params);
	return *this;
}

CClientScript & CClientScript::registerScript(const string & id, const _string & script, int position)
{
	hasScripts = true;
	TInlineJavascriptMap::const_iterator found = scripts.find(position);
	if (found == scripts.end()) {
		scripts[position] = TInlineJavascriptCodeMap();
	}
	scripts[position][id] = script;
	if (position == POS_READY || position == POS_LOAD) {
		registerCoreScript("jquery");
	}
//	$params=func_get_args();
//	$this->recordCachingAction('clientScript','registerScript',$params);
	return *this;
}

CClientScript & CClientScript::registerCssFile(const string & url, const _string & media)
{
	hasScripts = true;
	cssFiles[url] = media;
	//$params=func_get_args();
	//$this->recordCachingAction('clientScript','registerCssFile',$params);
	return *this;
}

CClientScript & CClientScript::registerCss(const string & id, const _string & css, const _string & media)
{
	hasScripts = true;
	TCssBlock block;
	block.media = media;
	block.code = css;
	this->css[id] = block;
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

void CClientScript::renderHead(_string & output)
{
	_string html;
	for (TMetaTagsList::const_iterator iter = metaTags.begin(); iter != metaTags.end(); ++iter) {
		html += CHtml::metaTag(iter->at(_("content")), _(""), _(""), *iter) + _("\n");
	}
	for (TLinkTagsList::const_iterator iter = linkTags.begin(); iter != linkTags.end(); ++iter) {
		html += CHtml::linkTag(_(""), _(""), _(""), _(""), *iter) + _("\n");
	}
	for (TCssFileMap::const_iterator iter = cssFiles.begin(); iter != cssFiles.end(); ++iter) {
		html += CHtml::cssFile(iter->first, iter->second) + _("\n");
	}
	for (TInlineCssMap::const_iterator iter = css.begin(); iter != css.end(); ++iter) {
		html += CHtml::css(iter->second.code, iter->second.media) + _("\n");
	}
	if (enableJavaScript) {
		TJavascriptFileMap::const_iterator filesFound = scriptFiles.find(POS_HEAD);
		if (filesFound != scriptFiles.end()) {
			for (TClientFileMap::const_iterator iter = filesFound->second.begin(); iter != filesFound->second.end(); ++iter) {
				html += CHtml::scriptFile(iter->first, iter->second) + _("\n");
			}
		}
		TInlineJavascriptMap::const_iterator scriptsFound = scripts.find(POS_HEAD);
		if (scriptsFound != scripts.end()) {
			html += renderScriptBatch(scriptsFound->second);
		}
	}

	if (!html.empty()) {
		if (output.find(_("</head")) == ::string::npos) {
			output = html + output;
		} else {
			output = boost::regex_replace(
				output,
				boost::wregex(_("(</head\\s*>)")),
				_("<#head#>$1")
			);
			_string replacement = _("<#head#>");
			::_string::size_type pos = output.find(replacement);
			if (pos != ::_string::npos) {
				output.replace(pos, replacement.length(), html);
			}
		}
	}
}

void CClientScript::renderBodyBegin(_string & output)
{
	_string html;

	TJavascriptFileMap::const_iterator filesFound = scriptFiles.find(POS_BEGIN);
	if (filesFound != scriptFiles.end()) {
		for (TClientFileMap::const_iterator iter = filesFound->second.begin(); iter != filesFound->second.end(); ++iter) {
			html += CHtml::scriptFile(iter->first, iter->second) + _("\n");
		}
	}
	TInlineJavascriptMap::const_iterator scriptsFound = scripts.find(POS_BEGIN);
	if (scriptsFound != scripts.end()) {
		html += renderScriptBatch(scriptsFound->second);
	}

	if (!html.empty()) {
		if (output.find(_("<body")) == ::_string::npos) {
			output = html + output;
		} else {
			output = boost::regex_replace(
				output,
				boost::wregex(_("(<body\\b[^>]*>)")),
				_("$1<#begin#>")
			);
			_string replacement = _("<#begin#>");
			::_string::size_type pos = output.find(replacement);
			if (pos != ::_string::npos) {
				output.replace(pos, replacement.length(), html);
			}
		}
	}
}

void CClientScript::renderBodyEnd(_string & output)
{
	bool isFullPage = output.find(_("</body")) != ::_string::npos;
	output = boost::regex_replace(
		output,
		boost::wregex(_("(</body\\s*>)")),
		_("<#end#>$1")
	);
	_string html;

	TJavascriptFileMap::const_iterator filesFound = scriptFiles.find(POS_END);
	if (filesFound != scriptFiles.end()) {
		for (TClientFileMap::const_iterator iter = filesFound->second.begin(); iter != filesFound->second.end(); ++iter) {
			html += CHtml::scriptFile(iter->first, iter->second) + _("\n");
		}
	}

	TInlineJavascriptCodeMap batch;

	TInlineJavascriptMap::const_iterator endFound = scripts.find(POS_END);
	if (endFound != scripts.end()) {
		batch = endFound->second;
	}

	TInlineJavascriptMap::const_iterator readyFound = scripts.find(POS_READY);
	if (readyFound != scripts.end()) {
		_string js = readyFound->second.begin()->second;
		for (TInlineJavascriptCodeMap::const_iterator iter = ++readyFound->second.begin(); iter != readyFound->second.end(); ++iter) {
			js += _("\n") + iter->second;
		}
		if (isFullPage) {
			js = _("jQuery(function($) {\n") + js + _("\n});");
		}
		batch["#CClientScript#ready#"] = js;
	}

	TInlineJavascriptMap::const_iterator loadFound = scripts.find(POS_LOAD);
	if (loadFound != scripts.end()) {
		_string js = loadFound->second.begin()->second;
		for (TInlineJavascriptCodeMap::const_iterator iter = ++loadFound->second.begin(); iter != loadFound->second.end(); ++iter) {
			js += _("\n") + iter->second;
		}
		if (isFullPage) {
			js = _("jQuery(window).on('load',function() {\n") + js + _("\n});");
		}
		batch["#CClientScript#load#"] = js;
	}

	if (!batch.empty()) {
		html += renderScriptBatch(batch);
	}

	if (isFullPage) {
		_string replacement = _("<#end#>");
		::_string::size_type pos = output.find(replacement);
		if (pos != ::_string::npos) {
			output.replace(pos, replacement.length(), html);
		}
	} else {
		output += html;
	}
}

_string CClientScript::renderScriptBatch(const TInlineJavascriptCodeMap & scripts)
{
	_string html;
	for (TInlineJavascriptCodeMap::const_iterator iter = scripts.begin(); iter != scripts.end(); ++iter) {
		html += CHtml::script(iter->second);
	}
	return html;
}

CClientScript & CClientScript::registerMetaTag(
	const _string & content,
	const _string & name,
	const _string & httpEquiv,
	const TTagAttributesMap & options)
{
	hasScripts = true;
	TTagAttributesMap opts = options;
	if (!name.empty()) {
		opts[_("name")] = name;
	}
	if (!httpEquiv.empty()) {
		opts[_("http-equiv")] = httpEquiv;
	}
	opts[_("content")] = content;
	metaTags.push_back(opts);
//	$params=func_get_args();
//	$this->recordCachingAction('clientScript','registerMetaTag',$params);
	return *this;
}

CClientScript & CClientScript::registerLinkTag(
	const _string & relation,
	const _string & type,
	const _string & href,
	const _string & media,
	const TTagAttributesMap & options)
{
	hasScripts = true;
	TTagAttributesMap opts = options;
	if (!relation.empty()) {
		opts[_("rel")] = relation;
	}
	if (!type.empty()) {
		opts[_("type")] = type;
	}
	if (!href.empty()) {
		opts[_("href")] = href;
	}
	if (!media.empty()) {
		opts[_("media")] = media;
	}
	linkTags.push_back(opts);
//	$params=func_get_args();
//	$this->recordCachingAction('clientScript','registerLinkTag',$params);
	return *this;
}
