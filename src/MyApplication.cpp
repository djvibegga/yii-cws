/*
 * MyApplication.cpp
 *
 *  Created on: Oct 15, 2013
 *      Author: djvibegga
 */

#include "MyApplication.h"
#include "base/YiiBase.h"
#include <web/CController.h>
#include "SiteController.h"
#include "ProductController.h"
#include <web/CUrlManager.h>

MyApplication::MyApplication(const string &configPath, int argc, char * const argv[])
: CWebApplication(configPath, argc, argv)
{

}

MyApplication::~MyApplication()
{

}

void MyApplication::registerComponents()
{
	CUrlManager * urlManager = new CUrlManager();
	urlManager->init();
	setComponent("urlManager", urlManager);

	CController * siteController = new SiteController();
	siteController->init();
	setController("site", siteController);

	CController * productController = new ProductController();
	productController->init();
	setController("product", productController);
}
