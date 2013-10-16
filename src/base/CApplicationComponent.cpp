/*
 * CApplicationComponent.cpp
 *
 *  Created on: Oct 16, 2013
 *      Author: djvibegga
 */

#include "base/CApplicationComponent.h"

CApplicationComponent::CApplicationComponent(const string &id, const CModule * module)
: CComponent()
{
	_module = module;
	_id = id;
}

const CModule * CApplicationComponent::getModule() const
{
	return _module;
}

string CApplicationComponent::getId() const
{
	return _id;
}
