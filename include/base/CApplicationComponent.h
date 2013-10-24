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

class CApplicationComponent: public CComponent, public IConfigureable
{
private:
	const CModule * _module;
	string _id;

public:
	CApplicationComponent(const CModule * module);
	CApplicationComponent(const string &id, CModule * module);
	const CModule * getModule() const;
	virtual void init();
	virtual void setId(const string & id);
	virtual string getId() const;
	virtual void applyConfig(const xml_node & config);
	virtual TNamesPath resolveNamePath() const;
};

#endif /* CAPPLICATIONCOMPONENT_H_ */
