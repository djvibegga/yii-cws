/*
 * SiteController.cpp
 *
 *  Created on: Oct 21, 2013
 *      Author: djvibegga
 */

#include "SiteController.h"
#include <base/Jvibetto.h>
#include <boost/assign.hpp>
#include <base/Jvibetto.h>
#include <db/CDbCommand.h>
#include <utils/CMap.h>
#include "TestWidget.h"

SiteController::SiteController(CModule * parent)
: CController("site", parent)
{
	layout = "//layouts/main";
}

SiteController::~SiteController()
{
}

void SiteController::init()
{
    CController::init();
	registerAction("index", ACTION(&SiteController::actionIndex));
}

void SiteController::actionIndex(CHttpRequest * const request, CHttpResponse * response)
{
	CWebApplication * app = dynamic_cast<CWebApplication*>(Jvibetto::app());
	CUrlManager * urlManager = app->getUrlManager();
	TRouteStruct route("site/index");
	route.params = boost::assign::map_list_of("id", "100500") ("name", "maks") ("hui", "pizda");

	cpptempl::data_map viewData;
	viewData["adminURL"] = urlManager->createUrl(route);

	render("index", viewData);
}
