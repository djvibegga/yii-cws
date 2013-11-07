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

using namespace std;

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

void ProductController::actionTest(CHttpRequest * const request, CHttpResponse * response)
{
	Jvibetto::log("test message");

	response->echo("<TITLE>fastcgi</TITLE>\n<H1>Fastcgi: 10050.</H1>\n");
	TRequestParams params = request->getParams();
	*response << "I am product controller. Action test." << "<br/>";
	for (TRequestParams::iterator iter = params.begin(); iter != params.end(); ++iter) {
		*response << "Param. Name: " << iter->first << ". Value: " << iter->second << "<br/>";
	}

	CDbConnection * connection = dynamic_cast<CDbConnection*>(Jvibetto::app()->getComponent("db"));
	CDbCommand cmd(connection, "select id, email from user where id >= :id");

	unsigned long id = 1;
	cmd.bindParam("id", id);

	stringstream ss;

	CDbDataReader reader = cmd.queryAll();
	TDbRow row;
	int i = 0;
	while (reader.nextResult()) {
		row = reader.readRow();
		ss << "Row " << i << " fetched: ID => " << row["id"]->asULong() << "<br/>";
		ss << "Row " << i << " fetched: email => " << row["email"]->asString().GetMultiByteChars() << "<br/>";
		++i;
	}

	*response << ss.str();
}
