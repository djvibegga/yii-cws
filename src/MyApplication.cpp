/*
 * MyApplication.cpp
 *
 *  Created on: Oct 15, 2013
 *      Author: djvibegga
 */

#include "MyApplication.h"

MyApplication::MyApplication(const string &configPath)
: CApplication(configPath)
{

}

void MyApplication::processRequest(const FCGX_Request &request)
{
	echo("Content-type: text/html\r\n\r\n<TITLE>fastcgi</TITLE>\n<H1>Fastcgi: 10050.</H1>\n");
}
