/*
 * ProductController.cpp
 *
 *  Created on: Oct 21, 2013
 *      Author: djvibegga
 */

#include "ProductController.h"
#include <web/CWebApplication.h>
#include <base/Jvibetto.h>
#include <db/CDbCommand.h>
#include <db/CDbDataReader.h>
#include <base/cpptempl.h>

using namespace std;
using namespace cpptempl;

ProductController::ProductController(CModule * parent)
: CController("product", parent)
{
}

ProductController::~ProductController()
{
}

void ProductController::init()
{
	registerAction("test", reinterpret_cast<TAction>(&ProductController::actionTest));
}

void ProductController::actionTest(CHttpRequest * const request, CHttpResponse * response) throw (CException)
{
	Jvibetto::log("test message");

	response->echo(_("<TITLE>fastcgi</TITLE>\n<H1>Fastcgi: 10050.</H1>\n"));
	TRequestParams params = request->getParams();
	*response << _("I am product controller. Action test.") << _("<br/>");

	CDbConnection * connection = dynamic_cast<CDbConnection*>(Jvibetto::app()->getComponent("db"));
	CDbCommand cmd(connection, "select id, email from user where id >= :id");

	unsigned long id = 1;
	cmd.bindParam("id", id);

	CDbDataReader reader = cmd.queryAll();
	TDbRow row;
	cpptempl::data_map viewData;
	cpptempl::data_map user;
	while (reader.nextResult()) {
		row = reader.readRow();
		user["id"] = string(row["id"]->asString().GetMultiByteChars());
		user["email"] = string(row["email"]->asString().GetMultiByteChars());
		viewData["users"].push_back(user);
	}

	_string text = _("{% for user in users %}ID: {$user.id}, Email: {$user.email}{% endfor %}");
	_string result = cpptempl::parse(text, viewData);

	*response << result;
}
