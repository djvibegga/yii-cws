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

#include <base/cpptempl.h>

class IOutputBuffer
{
public:
	virtual ~IOutputBuffer() {};
	virtual _string getContent() const = 0;
	virtual void echo(const _string & content) = 0;
	virtual IOutputBuffer & operator<< (const _string &right) = 0;
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
    virtual _string renderInternal(const string & viewFile, const cpptempl::data_map & data, bool ret = false) const = 0;
    virtual _string renderInternal(IView & viewInstance, bool ret = false) const = 0;
};

class IViewRenderer
{
public:
    string fileExtension;
    IViewRenderer() : fileExtension(".tpl") {}
    virtual ~IViewRenderer() {}
    virtual _string renderFile(const IRenderingContext * context, const string & file, const cpptempl::data_map & data, bool ret) = 0;
};

class IHasLayout
{
public:
	virtual ~IHasLayout() {}
	virtual boost::filesystem::path getLayoutPath() const = 0;
	virtual void setLayoutPath(const boost::filesystem::path & path) = 0;
};

struct FCGX_Request;
class CWebApplication;

class IWebRequestPool
{
public:
	virtual ~IWebRequestPool() {};
	virtual CWebApplication * createAppInstance() const = 0;
	virtual FCGX_Request * popRequest() = 0;
};

class IRunable
{
public:
	virtual ~IRunable() {};
	virtual void init() throw (CException) = 0;
	virtual void run() throw (CException) = 0;
};

#endif /* INTERFACES_H_ */
