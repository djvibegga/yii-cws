/*
 * CClientScript.cpp
 *
 *  Created on: Feb 17, 2014
 *      Author: djvibegga
 */

#include "web/CClientScript.h"
#include "web/CAssetManager.h"
#include "base/Jvibetto.h"

CClientScript::CClientScript(CModule * module)
: CApplicationComponent("clientScript", module)
{

}

CClientScript::CClientScript(const string &id, CModule * module)
: CApplicationComponent(id, module)
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
//	if(!isset($this->coreScripts[$name]))
//		return false;
//	$package=$this->coreScripts[$name];
//	if(isset($package['baseUrl']))
//	{
//		$baseUrl=$package['baseUrl'];
//		if($baseUrl==='' || $baseUrl[0]!=='/' && strpos($baseUrl,'://')===false)
//			$baseUrl=Yii::app()->getRequest()->getBaseUrl().'/'.$baseUrl;
//		$baseUrl=rtrim($baseUrl,'/');
//	}
//	elseif(isset($package['basePath']))
//		$baseUrl=Yii::app()->getAssetManager()->publish(Yii::getPathOfAlias($package['basePath']));
//	else
//		$baseUrl=$this->getCoreScriptUrl();
//
//	return $this->coreScripts[$name]['baseUrl']=$baseUrl;
}

CClientScript & CClientScript::addPackage(const string & name, const TScriptPackage & definition)
{
	packages[name] = definition;
	return *this;
}
