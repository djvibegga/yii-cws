/*
 * CModule.h
 *
 *  Created on: Oct 14, 2013
 *      Author: djvibegga
 */

#ifndef APPLICATION_H_
#define APPLICATION_H_

#include "base/CModule.h"
#include "fcgi_stdio.h"
#include <string>
#include "pugiconfig.hpp"
#include "pugixml.hpp"

using namespace std;
using namespace pugi;

class CApplication: public CModule
{
private:
	string _configPath;
	xml_node _xmlConfig;
	FCGX_Request _request;

public:
	CApplication(const string &configPath);
	virtual ~CApplication();
	virtual void run();
	void echo(const string & content);

protected:
	void mainLoop();
	virtual void init();
	virtual void processRequest(const FCGX_Request &request) = 0;
};

#endif /* APPLICATION_H_ */
