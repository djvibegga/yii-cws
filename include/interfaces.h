/*
 * interfaces.h
 *
 *  Created on: Oct 14, 2013
 *      Author: djvibegga
 */

#ifndef INTERFACES_H_
#define INTERFACES_H_

#include <string>
#include <map>
#include <vector>

using namespace std;

class CComponent;
class CController;
class CEvent;
class CModule;

typedef map<string, CController*> TControllerMap;
typedef map<string, string> TRequestParams;
typedef vector<string> TNamesPath;

#define CALLEE(calltype, pointer) reinterpret_cast<calltype>(pointer)

struct TRouteStruct
{
	TRouteStruct() {}
	TRouteStruct(const string & path) { this->path = path; }
	string path;
	TRequestParams params;
};

class IEventHandler
{
public:
	virtual ~IEventHandler() {}
};

typedef void(IEventHandler::* TEventHandler)(CEvent & event);
#define EVENT_HANDLER(pointer) CALLEE(TEventHandler, pointer)

class IBehavior
{
public:
	virtual ~IBehavior() {};
	virtual void attach(CComponent * const component) = 0;
	virtual void detach(CComponent * const component) = 0;
	virtual bool getEnabled() = 0;
	virtual void setEnabled(bool value) = 0;
};

#include "pugiconfig.hpp"
#include "pugixml.hpp"

using namespace pugi;

class IConfigureable
{
public:
	virtual ~IConfigureable() {}
	virtual void applyConfig(const xml_node & config) = 0;
	virtual TNamesPath resolveNamePath() const = 0;
	virtual string getId() const = 0;
};

class IModule
{
public:
	virtual ~IModule() {}
	virtual bool hasController(const string & name) const = 0;
	virtual string getId() const = 0;
	virtual void setId(const string &id) = 0;
	virtual CController * getController(const string & name) const = 0;
	virtual void setController(const string & name, CController * instance) = 0;
	virtual void setModule(const string & name, IModule * subModule) = 0;
	virtual IModule * getParent() const = 0;
	virtual IModule * getModule(const string &name) const = 0;
};

#include <base/cpptempl.h>

class IRenderingContext
{
public:
    virtual ~IRenderingContext() {}
    virtual string renderInternal(const string & viewFile, cpptempl::data_map * data, bool ret) const = 0;
};

class IViewRenderer
{
public:
    string fileExtension;
    IViewRenderer() : fileExtension(".tpl") {}
    virtual ~IViewRenderer() {}
    virtual string renderFile(const IRenderingContext * context, const string & file, cpptempl::data_map * data, bool ret) = 0;
};


#endif /* INTERFACES_H_ */
