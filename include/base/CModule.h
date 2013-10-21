/*
 * CModule.h
 *
 *  Created on: Oct 14, 2013
 *      Author: djvibegga
 */

#ifndef CMODULE_H_
#define CMODULE_H_

#include "base/CComponent.h"
#include "interfaces.h"
#include <memory>

typedef map<string, CComponent*> TComponentsMap;
typedef map<string, IModule*> TModulesMap;
typedef map<string, void*> TParametersMap;

class CModule: public CComponent, public IModule
{
private:
	CModule * _parent;
	string _id;
	TComponentsMap _components;
	TModulesMap _modules;

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
	IModule * getModule(const string &name) const;
	IModule * getParent() const;

protected:
	virtual void registerComponents();
	virtual void unregisterComponents();
};


#endif /* CMODULE_H_ */
