/*
 * CApplicationComponent.cpp
 *
 *  Created on: Oct 16, 2013
 *      Author: djvibegga
 */

#include "base/CApplicationComponent.h"
#include "base/Cws.h"
#include <iostream>

CApplicationComponent::CApplicationComponent(CModule * module)
: CComponent()
{
	_module = module;
}

CApplicationComponent::CApplicationComponent(const string &id, CModule * module)
: CComponent()
{
	_module = module;
	_id = id;
	if (module != 0) {
		module->setComponent(id, this);
	}
}

CModule * CApplicationComponent::getModule() const
{
	return _module;
}

string CApplicationComponent::getId() const
{
	return _id;
}

void CApplicationComponent::setId(const string & id)
{
	_id = id;
}

void CApplicationComponent::init()
{
	xml_node config = Cws::app()->getConfigByNamePath(resolveNamePath());
	applyConfig(config);
}

TNamesPath CApplicationComponent::resolveNamePath() const
{
	TNamesPath ret = getModule()->resolveNamePath();
	ret.insert(ret.begin(), "components");
	ret.insert(ret.begin(), getId());
	return ret;
}

void CApplicationComponent::applyConfig(const xml_node & config)
{
}
