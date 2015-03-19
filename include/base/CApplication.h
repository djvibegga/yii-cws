/*
 * CModule.h
 *
 *  Created on: Oct 14, 2013
 *      Author: djvibegga
 */

#ifndef CAPPLICATION_H_
#define CAPPLICATION_H_

#include "base/CModule.h"
#include "base/CException.h"
#include "base/CProfiler.h"
#include "base/CSecurityManager.h"
#include "logging/CLogger.h"
#include "logging/CLogRouter.h"
#include "base/CTemplateEngine.h"
#include "base/CStatePersister.h"

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
class CApplicationPool;

class CApplication: public CModule, public IRunable
{

	friend class CApplicationPool;

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
	CApplicationPool * _pool;
	string _language;
	vector<string> _languages;
	static long _mainThreadId;
	static map<string, std::locale> _locales;
	CStatesMap _globalState;
	bool _stateChanged;
	CStatePersister _persister;

public:
	string name;

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
	static long getMainThreadId();
	static bool getIsWorkerInstance();
	static bool getIsWebWorkerInstance();
	TOutputStack & getOutputStack();
	void setLanguage(const string & language);
	string getLanguage() const;
	void end(unsigned int status = 0, bool needExit = true);
	static long convertThreadIdToLong(boost::thread::id threadId);
	CTemplateEngine * getTemplateEngine() const;

	void setPool(CApplicationPool * pool);
	CApplicationPool * getPool() const;
	vector<string> getLanguages() const;
	std::locale getLocaleByLanguage(const string & language) const;
	string createUrl(TRouteStruct & route, const string & ampersand = "&") const;

	CStatePersister & getStatePersister();
	virtual _string getGlobalState(const string & key, const _string & defaultValue = _(""));
	virtual void setGlobalState(const string & key, const _string & value, const _string & defaultValue = _(""));
	virtual CStatesMap loadGlobalStates();
	virtual void saveGlobalState();

protected:
	time_t startTime;
	virtual void beginRequest();
	virtual void handleRequest();
	virtual void endRequest();
	virtual void onProgramError();
	virtual void processRequest() = 0;
	virtual CTemplateEngine * createTemplateEngine();
	virtual IViewRenderer * createViewRenderer();
	virtual CLogRouter * createLogRouter();
	virtual CSecurityManager * createSecurityManager();
	virtual void applyConfig(const xml_node & config);
	virtual void initLocales();

private:
	void logProfileItems(CEvent & event);
	static void _programFailCallback(int signum);
};

#endif /* CAPPLICATION_H_ */
