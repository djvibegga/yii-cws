/*
 * CWebModule.cpp
 *
 *  Created on: Oct 19, 2013
 *      Author: djvibegga
 */

#include "web/CWebModule.h"

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
