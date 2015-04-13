/*
 * CModule.cpp
 *
 *  Created on: Oct 14, 2013
 *      Author: djvibegga
 */

#include "base/CModule.h"
#include "base/CApplication.h"
#include "base/CApplicationComponent.h"
#include "base/Jvibetto.h"

using namespace std;

CModule::CModule(const string &id)
: _components(map<string, CApplicationComponent*>()),
  params(map<string, void*>())
{
	_id = id;
	_parent = 0;
}

CModule::CModule(const string &id, CModule * parent)
: _components(map<string, CApplicationComponent*>()),
  params(map<string, void*>())
{
	_id = id;
	_parent = parent;
	if (parent != 0) {
		parent->setModule(id, this);
	}
}

CModule::~CModule()
{
	unregisterComponents();
}

string CModule::getClassName() const
{
	return "CModule";
}

void CModule::init()
{
    if (_basePath.empty()) {
        setBasePath(resolveBasePath());
    }
    if (_viewPath.empty()) {
        setViewPath(resolveViewPath());
    }
	xml_node config = Jvibetto::app()->getConfigByNamePath(resolveNamePath());
	applyConfig(config);
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

void CModule::setComponent(const string &name, CApplicationComponent * component)
{
	_components[name] = component;
}

void CModule::setComponent(CApplicationComponent * component)
{
	if (component->getId().empty()) {
		throw CException("Unknown component ID", 1, __FILE__, __LINE__);
	}
	setComponent(component->getId(), component);
}

CApplicationComponent * CModule::getComponent(const string &name) const
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

void CModule::setModule(const string & name, IModule * subModule)
{
	_modules[name] = subModule;
}

void CModule::setModule(IModule * subModule)
{
	setModule(subModule->getId(), subModule);
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

TNamesPath CModule::resolveNamePath() const
{
	const IModule * current = this;
	CApplication * app = Jvibetto::app();
	TNamesPath ret;
	while (current != app) {
		ret.push_back(current->getId());
		if (current->getParent() == app) {
			ret.push_back("modules");
		}
		current = current->getParent();
	}
	return ret;
}

void CModule::applyConfig(const xml_node & config)
{
}

boost::filesystem::path CModule::resolveViewPath() const
{
    return getBasePath().string() + "/views";
}

void CModule::setViewPath(const boost::filesystem::path & path)
{
    _viewPath = path;
}

void CModule::setViewPath(const string & path)
{
    setViewPath(boost::filesystem::path(path));
}

boost::filesystem::path CModule::getViewPath() const
{
    return _viewPath;
}

void CModule::setBasePath(const string & path)
{
    setBasePath(boost::filesystem::path(path));
}

void CModule::setBasePath(const boost::filesystem::path & path)
{
    _basePath = path;
}

boost::filesystem::path CModule::getBasePath() const
{
    return _basePath;
}
