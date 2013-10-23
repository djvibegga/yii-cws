/*
 * SiteController.cpp
 *
 *  Created on: Oct 21, 2013
 *      Author: djvibegga
 */

#include "SiteController.h"

SiteController::SiteController()
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
	TRouteStruct route;
	route.path = "site/index";
	route.params["id"] = "100500";
	route.params["name"] = "maks";
	route.params["hui"] = "pizda";
	*app << "Admin panel URL: " << urlManager->createUrl(route) << "<br/>";

	TRequestParams params = request->getParams();
	for (TRequestParams::iterator iter = params.begin(); iter != params.end(); ++iter) {
		*app << "Param. Name: " << iter->first << ". Value: " << iter->second << "<br/>";
	}
}
