/*
 * MyApplication.cpp
 *
 *  Created on: Oct 15, 2013
 *      Author: djvibegga
 */

#include "MyApplication.h"
#include "base/YiiBase.h"

MyApplication::MyApplication(const string &configPath)
: CWebApplication(configPath)
{

}

void MyApplication::processRequest() throw(CException)
{
	echo("Content-type: text/html\r\n\r\n<TITLE>fastcgi</TITLE>\n<H1>Fastcgi: 10050.</H1>\n");
}
