/*
 * MyApplication.cpp
 *
 *  Created on: Oct 15, 2013
 *      Author: djvibegga
 */

#include "MyApplication.h"
#include "base/Jvibetto.h"
#include <web/CController.h>
#include <logging/CFileLogRoute.h>
#include <logging/CConsoleLogRoute.h>
#include <db/CDbConnection.h>
#include <base/CEvent.h>
#include <web/renderers/CBaseViewRenderer.h>

#include "SiteController.h"
#include "ProductController.h"
#include <web/CUrlManager.h>
#include <web/CAssetManager.h>
#include <web/CClientScript.h>
#include "MyUrlRule.h"
#include "TestBehavior.h"
#include "utils/CFile.h"

MyApplication::MyApplication(const string &configPath, int argc, char * const argv[])
: CWebApplication(configPath, argc, argv)
{
}

MyApplication::MyApplication(const xml_document & configDocument, int argc, char * const argv[])
: CWebApplication(configDocument, argc, argv)
{
}

MyApplication::~MyApplication()
{
}

CUrlManager * MyApplication::createUrlManager()
{
	CUrlManager * manager = CWebApplication::createUrlManager();
	//manager->addRule(new MyUrlRule());
	//manager->addRule(new CUrlRule("site/index", "main/<id:\\d+>/<name:\\w+>*"));
	return manager;
}

CLogRouter * MyApplication::createLogRouter()
{
	CLogRouter * log = CWebApplication::createLogRouter();
	CFileLogRoute * fileRoute = new CFileLogRoute("application.log");
	fileRoute->setLevels("info,error,warning,trace,profile");
	fileRoute->init();
	log->addRoute(fileRoute);
	//getLogger().attachEventHandler("onLog", this, EVENT_HANDLER(&MyApplication::logStdout));
	return log;
}

void MyApplication::registerComponents()
{
	CDbConnection * connection = new CDbConnection(this);
	connection->setId("db");
	connection->init();
	if (!connection->open()) {
		Jvibetto::log("Can't open database connection.", CLogger::LEVEL_ERROR);
	}
	setComponent(connection);

	TestBehavior * behavior = new TestBehavior();
	attachBehavior(behavior);

	CAssetManager * am = new CAssetManager(this);
	am->init();

	CClientScript * cs = new CClientScript(this);
	cs->init();

	SiteController * siteController = new SiteController(this);
	siteController->init();

	CWebModule * catalog = new CWebModule("catalog", this);
	catalog->init();

	CController * productController = new ProductController(catalog);
	productController->init();
}


void MyApplication::logStdout(CEvent & event)
{
	SLogElement logItem = *(SLogElement*)(event.params["logItem"]);
	cout << "[" << logItem.time << "] [" << logItem.category << "] " << logItem.message << endl;
}
