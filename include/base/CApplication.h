/*
 * CModule.h
 *
 *  Created on: Oct 14, 2013
 *      Author: djvibegga
 */

#ifndef APPLICATION_H_
#define APPLICATION_H_

#include "base/CModule.h"
#include "base/CException.h"
#include "logging/CLogger.h"
#include "logging/CLogRouter.h"
#include <string>
#include "pugiconfig.hpp"
#include "pugixml.hpp"
#include <boost/filesystem.hpp>
#include <boost/filesystem/operations.hpp>

using namespace std;
using namespace pugi;

typedef vector<string> TArgumentsList;

class CEvent;

class CApplication: public CModule
{
private:
	string _configPath;
	string _id;
	xml_document * _xmlConfig;
	TArgumentsList _arguments;
	CLogger _logger;
	static CApplication * _instance;
	boost::filesystem::path _basePath;
	boost::filesystem::path _runtimePath;
	boost::filesystem::path _executablePath;
	CLogRouter * _log;

public:
	CApplication(const string &configPath, int argc, char * const argv[]);
	virtual ~CApplication();
	virtual string getId() const;
	virtual void setId(const string &id);
	virtual void init() throw(CException);
	virtual void run() throw(CException);
	void onBeginRequest(CEvent & event);
	void onEndRequest(CEvent & event);
	xml_node getConfigRoot() const;
	xml_node getConfigByNamePath(const TNamesPath & path) const;
	TArgumentsList getArguments() const;
	CLogger & getLogger();
	void setRuntimePath(const string & path);
	void setRuntimePath(const boost::filesystem::path & path);
	boost::filesystem::path getRuntimePath() const;
	CLogRouter * getLog();
	void setBasePath(const string & path);
	void setBasePath(const boost::filesystem::path & path);
	boost::filesystem::path getExecutablePath() const;
	boost::filesystem::path getBasePath() const;
	static CApplication * getInstance();

protected:
	virtual void beginRequest();
	virtual void handleRequest();
	virtual void endRequest();
	virtual void processRequest() = 0;
};

#endif /* APPLICATION_H_ */
