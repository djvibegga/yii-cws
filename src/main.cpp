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
private:
	string _message;
public:
	void attach(CComponent * const component)
	{
		_message = "hoho";
	}

	virtual void handleEvent(const string &name, CEvent &event)
	{
		if (name == "onBeforeSave") {
			this->beforeSave(event);
		}
	}

	void beforeSave(CEvent &event)
	{
		event.handled = true;
		cout << _message << endl;
	}
};

class FacebookManager: public CComponent
{
private:
	string _message;

public:
	void init()
	{
		_message = "testing";
	}

	string getMessage() const
	{
		return _message;
	}

public:
	virtual ~FacebookManager() {}
};

int main(int argc, char* const argv[])
{
	FacebookManager manager;
	manager.init();
	TestHandler obj;
	manager.attachBehavior(&obj);

	CEvent evt(0);
	manager.raiseEvent("onBeforeSave", evt);

	CModule module("facebook");
	module.init();
	module.setComponent("manager", &manager);

	FacebookManager * component = dynamic_cast<FacebookManager*>(module.getComponent("manager"));
	cout << component->getMessage() << endl;

	ofstream log;
	log.open("server.log", ios_base::out);
	if (!log.good()) {
		log << "can\'t open log file." << endl;
		log.close();
		exit(1);
	}

	string configPath = getcwd(0, 0);
	configPath += "/main.xml";
	cout << "Config file path: " << configPath << endl;

	MyApplication app(configPath);
	app.run();

	log.close();

    return 0;
}
