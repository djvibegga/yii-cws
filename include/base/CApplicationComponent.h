/*
 * CApplicationComponent.h
 *
 *  Created on: Oct 16, 2013
 *      Author: djvibegga
 */

#ifndef CAPPLICATIONCOMPONENT_H_
#define CAPPLICATIONCOMPONENT_H_

#include "base/CComponent.h"
#include "base/CModule.h"

class CApplicationComponent: public CComponent
{
private:
	const CModule * _module;
	string _id;

public:
	CApplicationComponent(const string &id, const CModule * module);
	const CModule * getModule() const;
	string getId() const;
};

#endif /* CAPPLICATIONCOMPONENT_H_ */
