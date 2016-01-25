/*
 * CLogRouter.cpp
 *
 *  Created on: Oct 23, 2013
 *      Author: djvibegga
 */

#include "logging/CLogRouter.h"
#include "base/Cws.h"
#include "base/CEvent.h"

CLogRouter::CLogRouter(CModule * module)
: CApplicationComponent("log", module)
{
}

CLogRouter::CLogRouter(const string & id, CModule * module)
: CApplicationComponent(id, module)
{
}

string CLogRouter::getClassName() const
{
	return "CLogRouter";
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
	Cws::getLogger()
		.attachEventHandler("onFlush", this, reinterpret_cast<TEventHandler>(&CLogRouter::collectLogs));
	Cws::app()
		->attachEventHandler("onEndRequest", this, reinterpret_cast<TEventHandler>(&CLogRouter::processLogs));
}

void CLogRouter::addRoute(CLogRoute * route)
{
	_routes.push_back(route);
}

void CLogRouter::collectLogs(CEvent & event)
{
	CLogger & logger = Cws::getLogger();
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
	CLogger & logger = Cws::getLogger();
	for (TLogRouteList::iterator iter = _routes.begin(); iter != _routes.end(); ++iter) {
		if ((*iter)->enabled) {
			(*iter)->collectLogs(logger, true);
		}
	}
	logger.clearLogs();
}
