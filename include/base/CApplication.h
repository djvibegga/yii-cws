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
#include <string>
#include "pugiconfig.hpp"
#include "pugixml.hpp"

using namespace std;
using namespace pugi;

typedef vector<string> TArgumentsList;

class CEvent;

class CApplication: public CModule
{
private:
	string _configPath;
	xml_document * _xmlConfig;
	TArgumentsList _arguments;
	static CApplication * _instance;

public:
	CApplication(const string &configPath, int argc, char * const argv[]);
	virtual ~CApplication();
	virtual void init() throw(CException);
	virtual void run() throw(CException);
	void onBeginRequest(CEvent & event);
	void onEndRequest(CEvent & event);
	xml_node getConfigRoot();
	TArgumentsList getArguments() const;
	static CApplication * getInstance();

protected:
	virtual void beginRequest();
	virtual void handleRequest();
	virtual void endRequest();
	virtual void processRequest() = 0;
};

#endif /* APPLICATION_H_ */
