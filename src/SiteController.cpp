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
#include <db/CDbDataReader.h>
#include <db/CDbCriteria.h>
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
	//CUrlManager * urlManager = app->getUrlManager();
	//TRouteStruct route("site/index");
	//route.params = boost::assign::map_list_of("id", "100500") ("name", "maks") ("hui", "pizda");

	CDbCriteria criteria;
	unsigned long id = 5;
	criteria.compare("id", id);

	CDbConnection * db = dynamic_cast<CDbConnection*>(app->getComponent("db"));
	string query = "SELECT COUNT(*) as goalsCount FROM goals WHERE " + criteria.condition;
	CDbCommand cmd(db, query);
	CDbDataReader reader = cmd.queryAll(criteria.params);

	cpptempl::data_map viewData;
	//viewData["adminURL"] = urlManager->createUrl(route);

	reader.nextResult();
	long goalsCount = reader.readColumn("goalsCount").asULong();
	viewData["goalsCount"] = goalsCount;

	stringstream ss;
	ss << "Fetched " << goalsCount << " goals from database";
	Jvibetto::trace(ss.str());

	render("index", viewData);
}
