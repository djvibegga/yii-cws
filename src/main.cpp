/*
 * main.cpp
 *
 *  Created on: Oct 13, 2013
 *      Author: djvibegga
 */

#include <string>
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include "db/CDbConnection.h"
#include "base/CEvent.h"
#include "base/CModule.h"
#include "base/CException.h"
#include "base/CHttpException.h"
#include "base/YiiBase.h"
#include "MyApplication.h"

using namespace std;


void runServer()
{
//	CDbConnection db;
//	db.applyConfig(config.root());
//	try {
//		if (!db.open()) {
//			log << "Can\'t establish mysql connection.";
//			log.close();
//			exit(1);
//		}
//	} catch (const CException &e) {
//		cout << e.getFullMessage() << endl;
//		return;
//	}
//
//	MYSQL * conn = db.getConnection();
//	MYSQL_RES *res;
//	MYSQL_ROW row;
//
//	string content = "";
}

class TestHandler: public CBehavior
{
public:

	TEventNameList events()
	{
		TEventNameList ret;
		ret.insert(ret.begin(), "onEndRequest");
		return ret;
	}

	void handleEvent(const string &name, CEvent &event)
	{
		if (name == "onEndRequest") {
			afterRequest(event);
		}
	}

	void afterRequest(CEvent & event)
	{
		(dynamic_cast<CWebApplication*>(YiiBase::app()))->echo("<h2>I am a behavior</h2>");
	}
};

int main(int argc, char* const argv[])
{
	/*ofstream log;
	log.open("server.log", ios_base::out);
	if (!log.good()) {
		log << "can\'t open log file." << endl;
		log.close();
		exit(1);
	}*/
	//log.close();

	try {
		string configPath = getcwd(0, 0);
		configPath += "/main.xml";
		cout << "Config file path: " << configPath << endl;

		MyApplication app(configPath);
		app.init();
		TestHandler obj;
		app.attachBehavior(&obj);
		app.run();
	} catch (const CException & e) {
		cout << e.getFullMessage() << endl;
	}

    return 0;
}
