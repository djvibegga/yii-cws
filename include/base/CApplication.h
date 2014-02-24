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
#include "base/CProfiler.h"
#include "logging/CLogger.h"
#include "logging/CLogRouter.h"
#include <string>
#include <stack>
#include "pugiconfig.hpp"
#include "pugixml.hpp"
#include <boost/filesystem.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/thread/mutex.hpp>

using namespace std;
using namespace pugi;

class CApplication;

typedef vector<string> TArgumentsList;
typedef stack<IOutputBuffer*> TOutputStack;
typedef map<long, CApplication*> TAppInstanceMap;

class CEvent;

class CApplication: public CModule, public IRunable
{
private:
	static bool _failHandlerCalled;
	string _configPath;
	string _id;
	xml_document * _xmlConfig;
	TArgumentsList _arguments;
	CLogger _logger;
	CProfiler _profiler;
	static boost::mutex _instanceLocker;
	static TAppInstanceMap _instances;
	boost::filesystem::path _runtimePath;
	boost::filesystem::path _executablePath;
	CLogRouter * _log;
	TOutputStack _outputStack;

public:
	CApplication(const string & configPath, int argc, char * const argv[]);
	CApplication(const xml_document & configDocument, int argc, char * const argv[]);
	virtual ~CApplication();
	virtual string getId() const;
	virtual void setId(const string &id);
	virtual void init() throw (CException);
	virtual void run() throw (CException);
	void onBeginRequest(CEvent & event);
	void onEndRequest(CEvent & event);
	xml_node getConfigRoot() const;
	xml_node getConfigByNamePath(const TNamesPath & path) const;
	TArgumentsList getArguments() const;
	CLogger & getLogger();
	CProfiler & getProfiler();
	IViewRenderer * getViewRenderer() const;
	void setRuntimePath(const string & path);
	void setRuntimePath(const boost::filesystem::path & path);
	boost::filesystem::path getRuntimePath() const;
	CLogRouter * getLog();
	boost::filesystem::path getExecutablePath() const;
	virtual boost::filesystem::path resolveBasePath() const;
	static CApplication * getInstance();
	static long getThreadId();
	TOutputStack & getOutputStack();

protected:
	time_t startTime;
	virtual void beginRequest();
	virtual void handleRequest();
	virtual void endRequest();
	virtual void onProgramError();
	virtual void processRequest() = 0;
	virtual IViewRenderer * createViewRenderer();
	virtual CLogRouter * createLogRouter();

private:
	void logProfileItems(CEvent & event);
	static void _programFailCallback(int signum);
};

#endif /* APPLICATION_H_ */
