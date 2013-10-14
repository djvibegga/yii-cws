/*
 * CModule.h
 *
 *  Created on: Oct 14, 2013
 *      Author: djvibegga
 */

#ifndef CMODULE_H_
#define CMODULE_H_

#include "base/CComponent.h"

typedef map<string, CComponent*> TComponentsMap;
typedef map<string, void*> TParametersMap;

class CModule: public CComponent
{
private:
	CModule * _parent;
	string _id;
	TComponentsMap _components;

public:
	TParametersMap params;

	CModule(const string &id);
	CModule(const string &id, CModule * parent);
	virtual ~CModule();
	virtual void init();
	virtual void setId(const string &id);
	virtual string getId() const;
	void setComponent(const string &name, CComponent * component);
	CComponent * getComponent(const string &name) const;
	CModule * getParent() const;
};


#endif /* CMODULE_H_ */
