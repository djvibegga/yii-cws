/*
 * CWebModule.cpp
 *
 *  Created on: Oct 19, 2013
 *      Author: djvibegga
 */

#include <web/CWebModule.h>
#include <base/Cws.h>

CWebModule::CWebModule(const string &id)
: CModule(id)
{
}

CWebModule::CWebModule(const string &id, CModule * parent)
: CModule(id, parent)
{
}

CWebModule::~CWebModule()
{
}

string CWebModule::getClassName() const
{
	return "CWebModule";
}

void CWebModule::init()
{
	_layoutPath = resolveLayoutPath();
}

bool CWebModule::hasController(const string & name) const
{
	return _controllerMap.find(name) != _controllerMap.end();
}

CController * CWebModule::getController(const string & name) const
{
	TControllerMap::const_iterator iter = _controllerMap.find(name);
	if (iter == _controllerMap.end()) {
		return 0;
	} else {
		return iter->second;
	}
}

void CWebModule::setController(const string & name, CController * instance)
{
	_controllerMap[name] = instance;
}

boost::filesystem::path CWebModule::resolveBasePath() const
{
    string resolved = Cws::app()->getBasePath().string() + "/modules/" + getId();
    return boost::filesystem::path(resolved);
}

boost::filesystem::path CWebModule::resolveLayoutPath()
{
	return boost::filesystem::path(getViewPath().normalize().string() + "/layouts");
}

boost::filesystem::path CWebModule::getLayoutPath() const
{
	return _layoutPath;
}

void CWebModule::setLayoutPath(const boost::filesystem::path & layoutPath)
{
	_layoutPath = layoutPath;
}
