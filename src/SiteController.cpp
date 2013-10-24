/*
 * SiteController.cpp
 *
 *  Created on: Oct 21, 2013
 *      Author: djvibegga
 */

#include "SiteController.h"
#include <base/Jvibetto.h>
#include <boost/assign.hpp>

SiteController::SiteController(CModule * parent)
: CController("site", parent)
{
}

SiteController::~SiteController()
{
}

void SiteController::init()
{
	registerAction("index", reinterpret_cast<TAction>(&SiteController::actionIndex));
}

void SiteController::actionIndex(CHttpRequest * const request, CWebApplication * const app)
{
	app->echo("Content-type: text/html\r\n\r\n<TITLE>fastcgi</TITLE>\n<H1>Fastcgi: 10050.</H1>\n");
	*app << "I am site controller. Action index.";

	CUrlManager * urlManager = app->getUrlManager();
	TRouteStruct route("site/index");
	route.params = boost::assign::map_list_of("id", "100500") ("name", "maks") ("hui", "pizda");
	*app << "Admin panel URL: " << urlManager->createUrl(route) << "<br/>";

	TRequestParams params = request->getParams();
	for (TRequestParams::iterator iter = params.begin(); iter != params.end(); ++iter) {
		*app << "Param. Name: " << iter->first << ". Value: " << iter->second << "<br/>";
	}

	Jvibetto::log("site controller call", CLogger::LEVEL_ERROR);
}
