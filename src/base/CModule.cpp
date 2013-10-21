/*
 * CModule.cpp
 *
 *  Created on: Oct 14, 2013
 *      Author: djvibegga
 */

#include "base/CModule.h"

using namespace std;

CModule::CModule(const string &id)
: _components(map<string, CComponent*>()),
  params(map<string, void*>())
{
	_id = id;
	_parent = 0;
}

CModule::CModule(const string &id, CModule * parent)
: _components(map<string, CComponent*>()),
  params(map<string, void*>())
{
	_id = id;
	_parent = parent;
}

CModule::~CModule()
{
	unregisterComponents();
}

void CModule::init()
{
	registerComponents();
}

void CModule::setId(const string &id)
{
	_id = id;
}

string CModule::getId() const
{
	return _id;
}

void CModule::setComponent(const string &name, CComponent * component)
{
	_components[name] = component;
}

CComponent * CModule::getComponent(const string &name) const
{
	TComponentsMap::const_iterator found = _components.find(name);
	if (found == _components.end()) {
		return 0;
	} else {
		return found->second;
	}
}

IModule * CModule::getParent() const
{
	return _parent;
}

IModule * CModule::getModule(const string &name) const
{
	TModulesMap::const_iterator iter = _modules.find(name);
	if (iter == _modules.end()) {
		return 0;
	} else {
		return iter->second;
	}
}

void CModule::registerComponents()
{

}

void CModule::unregisterComponents()
{
	for (TComponentsMap::iterator iter = _components.begin(); iter != _components.end(); ++iter) {
		delete iter->second;
	}
}
