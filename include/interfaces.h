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
#include <boost/filesystem.hpp>
#include <boost/thread.hpp>
#include <boost/locale.hpp>
#include "defines.h"
#include "base/CException.h"

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
	TRouteStruct(const TRouteStruct & other)
	{
		this->path = other.path;
		for (TRequestParams::const_iterator iter = other.params.begin(); iter != other.params.end(); ++iter) {
			this->params[iter->first] = iter->second;
		}
	}
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

#define PARSE_XML_CONF_INT_PROPERTY(xmlNode, attributeName, childName) \
if (!xmlNode.child(childName).empty()) {\
	attributeName = xmlNode\
		.child(childName)\
		.attribute("value")\
		.as_int();\
}

#define PARSE_XML_CONF_UINT_PROPERTY(xmlNode, attributeName, childName) \
if (!xmlNode.child(childName).empty()) {\
	attributeName = xmlNode\
		.child(childName)\
		.attribute("value")\
		.as_uint();\
}

#define PARSE_XML_CONF_STRING_PROPERTY(xmlNode, attributeName, childName) \
if (!xmlNode.child(childName).empty()) {\
	attributeName = xmlNode\
		.child(childName)\
		.attribute("value")\
		.as_string();\
}

#define PARSE_XML_CONF_BOOL_PROPERTY(xmlNode, attributeName, childName) \
if (!xmlNode.child(childName).empty()) {\
	attributeName = xmlNode\
		.child(childName)\
		.attribute("value")\
		.as_bool();\
}

#define PARSE_XML_CONF_FLOAT_PROPERTY(xmlNode, attributeName, childName) \
if (!xmlNode.child(childName).empty()) {\
	attributeName = xmlNode\
		.child(childName)\
		.attribute("value")\
		.as_float();\
}

#define PARSE_XML_CONF_DOUBLE_PROPERTY(xmlNode, attributeName, childName) \
if (!xmlNode.child(childName).empty()) {\
	attributeName = xmlNode\
		.child(childName)\
		.attribute("value")\
		.as_double();\
}

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

#include <ctpp2/CDT.hpp>

using namespace CTPP;

class IOutputBuffer
{
public:
	virtual ~IOutputBuffer() {};
	virtual string getContent() const = 0;
	virtual void echo(const wstring & content) = 0;
	virtual void echo(const string & content) = 0;
	virtual IOutputBuffer & operator<< (const wstring &right) = 0;
	virtual IOutputBuffer & operator<< (const string &right) = 0;
};

class IView;
typedef boost::shared_ptr<IView> TViewPtr;

class IView
{
public:
	virtual ~IView() {}
	virtual void setOutputBuffer(IOutputBuffer * buffer) = 0;
	virtual IOutputBuffer * getOutputBuffer() const = 0;
	virtual TViewPtr getLayout() const = 0;
	virtual void init() = 0;
	virtual void run() throw (CException) = 0;
};

class IRenderingContext
{
public:
    virtual ~IRenderingContext() {}
    virtual string renderInternal(const string & viewFile, CDT & data, bool ret = false) const = 0;
    virtual string renderInternal(IView & viewInstance, bool ret = false) const = 0;
};

class IViewRenderer
{
public:
    string fileExtension;
    IViewRenderer() : fileExtension(".html") {}
    virtual ~IViewRenderer() {}
    virtual string getViewFile(const string & sourceFile) = 0;
    virtual string renderFile(const IRenderingContext * context, const string & file, CDT & data, bool ret) = 0;
};

class IHasLayout
{
public:
	virtual ~IHasLayout() {}
	virtual boost::filesystem::path getLayoutPath() const = 0;
	virtual void setLayoutPath(const boost::filesystem::path & path) = 0;
};

class CWebApplication;
class FCGX_Request;

class IRunable
{
public:
	virtual ~IRunable() {};
	virtual void init() throw (CException) = 0;
	virtual void run() throw (CException) = 0;
};


typedef map<string, _string> TPersistentStateMap;

class IUserIdentity
{
public:
	virtual ~IUserIdentity() {};
	virtual bool authenticate() = 0;
	virtual bool getIsAuthenticated() const = 0;
	virtual string getId() const = 0;
	virtual _string getName() const = 0;
	virtual TPersistentStateMap getPersistentStates() const = 0;
};

#endif /* INTERFACES_H_ */
