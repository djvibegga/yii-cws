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
#include "Goal.h"
#include "MyView.h"
#include "MyLayoutView.h"

SiteController::SiteController(CModule * parent)
: CController("site", parent)
{
	setLayout(TViewPtr(new MyLayoutView(this)));
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
	//CWebApplication * app = dynamic_cast<CWebApplication*>(Jvibetto::app());
	/*CUrlManager * urlManager = app->getUrlManager();
	TRouteStruct route("site/index");
	route.params = boost::assign::map_list_of("id", "100500") ("name", "maks") ("hui", "pizda");*/

	CDbCriteria criteria;
	//unsigned long id = 1;
	//criteria.compare("id", id, ">=");

    TActiveRecordList records = Goal::model()->findAll(criteria);

	//cpptempl::data_map viewData;
	//viewData["adminURL"] = urlManager->createUrl(route);

//	for (TActiveRecordList::const_iterator iter = records.begin(); iter != records.end(); ++iter) {
//		Goal * goal = dynamic_cast<Goal*>(iter->get());
//		cpptempl::data_map item;
//		item["id"] = goal->id;
//		item["name"] = goal->name;
//		viewData["goals"].push_back(item);
//	}

    MyView view(this);
    view.records = records;
    render(view);

	//render("index", viewData);
}
