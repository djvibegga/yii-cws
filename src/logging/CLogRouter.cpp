/*
 * CLogRouter.cpp
 *
 *  Created on: Oct 23, 2013
 *      Author: djvibegga
 */

#include "logging/CLogRouter.h"
#include "base/Jvibetto.h"
#include "base/CEvent.h"

CLogRouter::CLogRouter(CModule * module)
: CApplicationComponent("log", module)
{
}

CLogRouter::CLogRouter(const string & id, CModule * module)
: CApplicationComponent(id, module)
{
}

CLogRouter::~CLogRouter()
{
	for (TLogRouteList::iterator iter = _routes.begin(); iter != _routes.end(); ++iter) {
		delete *iter;
	}
}

void CLogRouter::init()
{
	CApplicationComponent::init();
	Jvibetto::getLogger().attachEventHandler("onFlush", this);
	Jvibetto::app()->attachEventHandler("onEndRequest", this);
}

void CLogRouter::addRoute(CLogRoute * route)
{
	_routes.push_back(route);
}

void CLogRouter::handleEvent(const string &name, CEvent &event)
{
	if (name == "onFlush") {
		collectLogs(event);
	} else if (name == "onEndRequest") {
		processLogs(event);
	}
}

void CLogRouter::collectLogs(CEvent & event)
{
	CLogger & logger = Jvibetto::getLogger();
	bool dumpLogs = event.params.find("dumpLogs") != event.params.end() &&
		*((bool*)event.params["dumpLogs"]);
	for (TLogRouteList::iterator iter = _routes.begin(); iter != _routes.end(); ++iter) {
		if ((*iter)->enabled) {
			(*iter)->collectLogs(logger, dumpLogs);
		}
	}
}

void CLogRouter::processLogs(CEvent & event)
{
	CLogger & logger = Jvibetto::getLogger();
	for (TLogRouteList::iterator iter = _routes.begin(); iter != _routes.end(); ++iter) {
		if ((*iter)->enabled) {
			(*iter)->collectLogs(logger, true);
		}
	}
	logger.clearLogs();
}
