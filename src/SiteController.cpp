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
#include <web/CAssetManager.h>
#include <web/CClientScript.h>
#include <web/CWebUser.h>
#include <utils/CMap.h>
#include "TestWidget.h"
#include "Goal.h"
#include "MyView.h"
#include "MyLayoutView.h"
#include "MyUserIdentity.h"

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
	registerAction("am", ACTION(&SiteController::actionAssetManager));
	registerAction("session", ACTION(&SiteController::actionSession));
	registerAction("cookies", ACTION(&SiteController::actionCookies));
	registerAction("login", ACTION(&SiteController::actionLogin));
	registerAction("logout", ACTION(&SiteController::actionLogout));
}

void SiteController::actionIndex(CHttpRequest * const request, CHttpResponse * response) throw (CException)
{
	//CWebApplication * app = dynamic_cast<CWebApplication*>(Jvibetto::app());
	/*CUrlManager * urlManager = app->getUrlManager();
	TRouteStruct route("site/index");
	route.params = boost::assign::map_list_of("id", "100500") ("name", "maks") ("hui", "pizda");*/

	CDbCriteria criteria;
//	unsigned long id = 950;
//	criteria.compare("id", id, ">=");

    TActiveRecordList records = Goal::model()->findAll(criteria);

//	cpptempl::data_map viewData;
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
    view.setLayout(getViewFile("//layouts/main"));
    render(view);

//	render("index", viewData);
}

void SiteController::actionAssetManager(CHttpRequest * const request, CHttpResponse * response) throw (CException)
{
	CAssetManager * am = dynamic_cast<CAssetManager*>(Jvibetto::app()->getComponent("assetManager"));
	CClientScript * cs = dynamic_cast<CClientScript*>(Jvibetto::app()->getComponent("clientScript"));

	string url = am->publish(
		Jvibetto::getPathOfAlias("application.assets")
	);

	cs->registerMetaTag(_("7ca4fb0526bc4815"), _("yandex-verification"));
	cs->registerCss("initial", _("body #content {padding: 5px; }"), _("screen"));

	cs->registerPackage("test");
	cs->registerPackage("subtest");
	cs->registerPackage("bbq");
	cs->registerPackage("cookie");

	cs->registerScript("test", _("alert('yes');"), CClientScript::POS_READY);

	cpptempl::data_map viewData;
	render("am", viewData);
}

void SiteController::actionSession(CHttpRequest * const request, CHttpResponse * response) throw (CException)
{
	cpptempl::data_map viewData;
	CHttpSession * session = dynamic_cast<CHttpSession*>(Jvibetto::app()->getComponent("session"));
	TSessionDataMap & sessionData = session->getData();
	if (sessionData.find("key") == sessionData.end()) {
		(*session)["key"] = _("привет");
		viewData["sessionKey"] = _("unknown");
	} else {
		viewData["sessionKey"] = sessionData["key"];
	}

	viewData["sessionId"] = session->getSessionId();
	render("session", viewData);
}

void SiteController::actionCookies(CHttpRequest * const request, CHttpResponse * response) throw (CException)
{
	CHttpSession * session = dynamic_cast<CHttpSession*>(Jvibetto::app()->getComponent("session"));
	CCookieCollection & cookies = request->getCookies();

	cpptempl::data_map viewData;
	if (cookies.find("sessid") != cookies.end()) {
		viewData["sessid"] = cookies["sessid"].value;
	}

	cookies.add(CHttpCookie("lang", "ru", "", "/"));
	cookies.add(CHttpCookie("sessid", session->getSessionId(), "", "/", time(0) + 100));
	cookies.remove("lang");

	render("cookies", viewData);
}

void SiteController::actionLogin(CHttpRequest * const request, CHttpResponse * response) throw (CException)
{
	CWebUser * user = dynamic_cast<CWebUser*>(Jvibetto::app()->getComponent("user"));

	if (!user->getIsGuest()) {
		*response << _("user is already logged");
		return;
	}

	MyUserIdentity identity("admin", "admin");
	if (!identity.authenticate()) {
		*response << _("failure auth");
		return;
	}
	if (!user->login(identity)) {
		*response << _("failure login");
		return;
	}

	*response << _("Welcome, ") << user->getName() << _("!");
}

void SiteController::actionLogout(CHttpRequest * const request, CHttpResponse * response) throw (CException)
{
	CWebUser * user = dynamic_cast<CWebUser*>(Jvibetto::app()->getComponent("user"));
	user->logout();
	*response << _("Bye bye...");
}
