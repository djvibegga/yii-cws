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

SiteController::SiteController(CModule * parent)
: CController("site", parent)
{
}

SiteController::~SiteController()
{
}

void SiteController::init()
{
	registerAction("index", ACTION(&SiteController::actionIndex));
}

void SiteController::actionIndex(CHttpRequest * const request, CHttpResponse * response)
{
	*response << "I am site controller. Action index.";

	CDbConnection * connection = dynamic_cast<CDbConnection*>(Jvibetto::app()->getComponent("db"));
	CDbCommand cmd(connection, "update user set password = md5('admin') where id = :id");
	cmd.bindParam(":1", 1);
	cmd.execute();


	CWebApplication * app = dynamic_cast<CWebApplication*>(Jvibetto::app());
	CUrlManager * urlManager = app->getUrlManager();
	TRouteStruct route("site/index");
	route.params = boost::assign::map_list_of("id", "100500") ("name", "maks") ("hui", "pizda");
	*response << "Admin panel URL: " << urlManager->createUrl(route) << "<br/>";

	TRequestParams params = request->getParams();
	for (TRequestParams::iterator iter = params.begin(); iter != params.end(); ++iter) {
		*response << "Param. Name: " << iter->first << ". Value: " << iter->second << "<br/>";
	}

	Jvibetto::log("site controller call", CLogger::LEVEL_ERROR);
}
