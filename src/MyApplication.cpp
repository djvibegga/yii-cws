/*
 * MyApplication.cpp
 *
 *  Created on: Oct 15, 2013
 *      Author: djvibegga
 */

#include "MyApplication.h"
#include "base/Jvibetto.h"
#include <web/CController.h>
#include "SiteController.h"
#include "logging/CFileLogRoute.h"
#include "ProductController.h"
#include <web/CUrlManager.h>
#include "MyUrlRule.h"

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
	CFileLogRoute * route = new CFileLogRoute("application.log");
	route->setLevels("info");
	route->init();
	log->addRoute(route);

	CUrlManager * urlManager = new CUrlManager(this);
	urlManager->init();
	urlManager->addRule(new MyUrlRule());
	urlManager->addRule(new CUrlRule("site/index", "main/<id:\\d+>/<name:\\w+>*"));

	CController * siteController = new SiteController(this);
	siteController->init();

	CWebModule * catalog = new CWebModule("catalog", this);
	catalog->init();

	CController * productController = new ProductController(catalog);
	productController->init();
}
