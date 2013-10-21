/*
 * ProductController.cpp
 *
 *  Created on: Oct 21, 2013
 *      Author: djvibegga
 */

#include "ProductController.h"
#include <web/CWebApplication.h>

using namespace std;

ProductController::ProductController()
{
}

ProductController::~ProductController()
{
}

void ProductController::init()
{
	registerAction("test", reinterpret_cast<TAction>(&ProductController::actionTest));
}

void ProductController::actionTest(CHttpRequest * const request, CWebApplication * const app)
{
	app->echo("Content-type: text/html\r\n\r\n<TITLE>fastcgi</TITLE>\n<H1>Fastcgi: 10050.</H1>\n");
	TRequestParams params = request->getParams();
	*app << "I am product controller. Action test." << "<br/>";
	for (TRequestParams::iterator iter = params.begin(); iter != params.end(); ++iter) {
		*app << "Param. Name: " << iter->first << ". Value: " << iter->second << "<br/>";
	}
}
