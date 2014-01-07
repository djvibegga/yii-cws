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
#include "MyUrlRule.h"
#include "TestBehavior.h"

MyApplication::MyApplication(const string &configPath, int argc, char * const argv[])
: CWebApplication(configPath, argc, argv)
{

}

MyApplication::~MyApplication()
{

}

void MyApplication::registerComponents()
{
	CLogRouter * log = getLog();
	getLogger().attachEventHandler("onLog", this, EVENT_HANDLER(&MyApplication::logStdout));
	CFileLogRoute * fileRoute = new CFileLogRoute("application.log");
	fileRoute->setLevels("info,error,warning,trace");
	fileRoute->init();
	log->addRoute(fileRoute);

	CDbConnection * connection = new CDbConnection(this);
	connection->setId("db");
	connection->init();
	if (!connection->open()) {
		Jvibetto::log("Can't open database connection.", CLogger::LEVEL_ERROR);
	}
	setComponent(connection);

	CViewRenderer * viewRenderer = new CBaseViewRenderer(this);
	viewRenderer->init();

	TestBehavior * behavior = new TestBehavior();
	attachBehavior(behavior);

	CUrlManager * urlManager = new CUrlManager(this);
	urlManager->init();
	urlManager->addRule(new MyUrlRule());
	urlManager->addRule(new CUrlRule("site/index", "main/<id:\\d+>/<name:\\w+>*"));

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
