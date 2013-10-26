/*
 * CConsoleLogRoute.cpp
 *
 *  Created on: Oct 26, 2013
 *      Author: djvibegga
 */

#include "logging/CConsoleLogRoute.h"
#include <iostream>

CConsoleLogRoute::CConsoleLogRoute()
{

}

CConsoleLogRoute::~CConsoleLogRoute()
{
}

void CConsoleLogRoute::init() throw (CException)
{
}


void CConsoleLogRoute::processLogs(TLogElementList & logs)
{
	for (TLogElementList::const_iterator iter = logs.begin(); iter != logs.end(); ++iter) {
		cout << formatLogMessage(*iter).c_str() << endl;
	}
}
