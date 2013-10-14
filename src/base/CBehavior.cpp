/*
 * CBehavior.cpp
 *
 *  Created on: Oct 14, 2013
 *      Author: djvibegga
 */
#include "base/CEvent.h"
#include "base/CComponent.h"
#include "base/CBehavior.h"

CBehavior::CBehavior()
: _enabled(true)
{
}

CBehavior::~CBehavior()
{
}

void CBehavior::attach(CComponent * const component)
{

}

void CBehavior::detach(CComponent * const component)
{

}

bool CBehavior::getEnabled()
{
	return _enabled;
}

void CBehavior::setEnabled(bool value)
{
	_enabled = value;
}

void CBehavior::handleEvent(const string &name, CEvent &event)
{

}
