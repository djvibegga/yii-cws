/*
 * CHttpResponse.cpp
 *
 *  Created on: Oct 26, 2013
 *      Author: djvibegga
 */

#include "web/CHttpResponse.h"
#include "web/CWebApplication.h"
#include <fcgi_stdio.h>


CHttpResponse::CHttpResponse(CWebApplication * app)
: CApplicationComponent("response", app),
  _startedOutput(false),
  _app(app)
{
}

CHttpResponse::~CHttpResponse()
{

}

_string CHttpResponse::getContent() const
{
	return _("");
}

void CHttpResponse::echo(const _string & content)
{
	if (!_startedOutput) {
		_startedOutput = true;
		echo(_("Content-type: text/html\r\n\r\n"));
	}
	FCGX_FPrintF(
		_app->request->out,
#ifdef _UNICODE
		cpptempl::wide_to_utf8(content).c_str()
#else
		content.c_str()
#endif
	);
}

IOutputBuffer & CHttpResponse::operator<< (const _string &right)
{
	echo(right);
	return *this;
}
