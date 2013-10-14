/*
 * main.cpp
 *
 *  Created on: Oct 13, 2013
 *      Author: djvibegga
 */

#include <string>
#include <fstream>
#include <iostream>
#include "fcgi_stdio.h"
#include <stdlib.h>
#include <unistd.h>
#include "db/CDbConnection.h"
#include "base/CEvent.h"
#include "base/CModule.h"
#include "base/CException.h"
#include "pugiconfig.hpp"
#include "pugixml.hpp"

using namespace std;
using namespace pugi;

void runServer()
{
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
	xml_document config;
	xml_parse_result result = config.load_file(configPath.c_str());
	if (result.status != status_ok) {
		log << "Can\'t parse config.";
		log.close();
		exit(1);
	}

	string port = ":";
	port.append(config.child("server").attribute("port").value());
	cout << "Server started on port: " << port << endl;
	int listenQueueBacklog = config.child("server")
		.child("threadsCount")
		.attribute("value").as_int();

	if (FCGX_Init()) {
		log << "can\'t initialize FCGX." << endl;
		log.close();
		exit(1); //Инициализируем библиотеку перед работой.
	} else {
		log << "Initialized FCGX." << endl;
	}

	int  listen_socket = FCGX_OpenSocket(port.c_str(), listenQueueBacklog); //Открываем новый слушающий сокет
	if (listen_socket < 0) {
		log << "Can\'t open socket." << endl;
		exit(1);
	} else {
		log << "Socket opened successfully." << endl;
	}

	FCGX_Request request;

	CDbConnection db;
	db.applyConfig(config.root());
	try {
		if (!db.open()) {
			log << "Can\'t establish mysql connection.";
			log.close();
			exit(1);
		}
	} catch (const CException &e) {
		cout << e.getFullMessage() << endl;
		return;
	}

	MYSQL * conn = db.getConnection();
	MYSQL_RES *res;
	MYSQL_ROW row;

	string content = "";

	if(FCGX_InitRequest(&request,  listen_socket, 0)) exit(1); //Инициализируем структуру запроса

	while(FCGX_Accept_r(&request) == 0)
	{
		content = "";
		FCGX_FPrintF(request.out, "Content-type: text/html\r\n\r\n<TITLE>fastcgi</TITLE>\n<H1>Fastcgi: Hello world.</H1>\n");
		   if (mysql_query(conn, "select id from user")) {
			  log << mysql_error(conn);
			  log.close();
			  exit(1);
		   }
		   res = mysql_use_result(conn);
		   while ((row = mysql_fetch_row(res)) != NULL) {
			   content.append("<div>").append(row[0]).append("</div>");
			   FCGX_FPrintF(request.out, content.c_str());
		   }
		   mysql_free_result(res);

		FCGX_Finish_r(&request);//Завершаем запрос
	}

	db.close();
	log.close();
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

	runServer();

    return 0;
}
