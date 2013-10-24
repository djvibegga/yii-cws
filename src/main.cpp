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
#include "base/Jvibetto.h"
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

int main(int argc, char* const argv[])
{
	string configPath = getcwd(0, 0);
	configPath += "/main.xml";
	cout << "Config file path: " << configPath << endl;
	try {
		MyApplication app(configPath, argc, argv);
		app.init();
		app.run();
	} catch (const CException & e) {
		cout << e.getFullMessage() << endl;
		return 1;
	}
    return 0;
}
