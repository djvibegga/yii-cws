/*
 * CModule.cpp
 *
 *  Created on: Oct 14, 2013
 *      Author: djvibegga
 */
#include "base/CModule.h"

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

}

void CModule::init()
{

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
	return _components.at(name);
}

CModule * CModule::getParent() const
{
	return _parent;
}
