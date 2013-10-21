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

using namespace std;

class CComponent;
class CController;
class CEvent;
class CModule;

typedef map<string, CController*> TControllerMap;
typedef map<string, string> TRequestParams;

struct TRouteStruct
{
	string path;
	TRequestParams params;
};

class IEventHandler
{
public:
	virtual ~IEventHandler() {}
	virtual void handleEvent(const string &name, CEvent &event) = 0;
};

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
};

class IModule
{
public:
	virtual ~IModule() {}
	virtual bool hasController(const string & name) const = 0;
	virtual CController * getController(const string & name) const = 0;
	virtual void setController(const string & name, CController * instance) = 0;
	virtual IModule * getParent() const = 0;
	virtual IModule * getModule(const string &name) const = 0;
};

#endif /* INTERFACES_H_ */
