/*
 * CHttpResponse.cpp
 *
 *  Created on: Oct 26, 2013
 *      Author: djvibegga
 */

#include "web/CHttpResponse.h"
#include "web/CWebApplication.h"
#include <fcgi_stdio.h>


CHttpResponse::CHttpResponse(const CWebApplication * app)
: CApplicationComponent(app),
  _startedOutput(false),
  _app(app)
{
}

CHttpResponse::~CHttpResponse()
{

}

string CHttpResponse::getContent() const
{
	return "";
}

void CHttpResponse::echo(const string & content)
{
	if (!_startedOutput) {
		_startedOutput = true;
		echo("Content-type: text/html\r\n\r\n");
	}
	FCGX_FPrintF(_app->request.out, content.c_str());
}

IOutputBuffer & CHttpResponse::operator<< (const string &right)
{
	echo(right);
	return *this;
}
