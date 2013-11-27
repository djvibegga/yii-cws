/*
 * CModule.h
 *
 *  Created on: Oct 14, 2013
 *      Author: djvibegga
 */

#ifndef CMODULE_H_
#define CMODULE_H_

#include "interfaces.h"
#include "base/CComponent.h"
#include <memory>
#include <boost/filesystem.hpp>

class CApplicationComponent;

typedef map<string, CApplicationComponent*> TComponentsMap;
typedef map<string, IModule*> TModulesMap;
typedef map<string, void*> TParametersMap;

class CModule: public CComponent, public IModule, public IConfigureable
{
private:
	CModule * _parent;
	string _id;
	TComponentsMap _components;
	TModulesMap _modules;
	boost::filesystem::path _viewPath;
    boost::filesystem::path _basePath;

public:
	TParametersMap params;

	CModule(const string &id);
	CModule(const string &id, CModule * parent);
	virtual ~CModule();
	virtual void init();
	virtual void setId(const string &id);
	virtual string getId() const;
	void setComponent(const string &name, CApplicationComponent * component);
	void setComponent(CApplicationComponent * component);
	CApplicationComponent * getComponent(const string &name) const;
	void setModule(const string & name, IModule * subModule);
	void setModule(IModule * subModule);
	IModule * getModule(const string &name) const;
	IModule * getParent() const;
	virtual void applyConfig(const xml_node & config);
	virtual TNamesPath resolveNamePath() const;
	virtual boost::filesystem::path resolveViewPath() const;
	void setViewPath(const string & path);
	void setViewPath(const boost::filesystem::path & path);
	boost::filesystem::path getViewPath() const;
	virtual boost::filesystem::path resolveBasePath() const = 0;
	void setBasePath(const string & path);
	void setBasePath(const boost::filesystem::path & path);
	boost::filesystem::path getBasePath() const;

protected:
	virtual void registerComponents();
	virtual void unregisterComponents();
};


#endif /* CMODULE_H_ */
