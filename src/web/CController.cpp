/*
 * CController.cpp
 *
 *  Created on: Oct 19, 2013
 *      Author: djvibegga
 */

#include <web/CController.h>
#include <web/CWebApplication.h>
#include <base/CException.h>

CController::CController(const string & id, CModule * module)
: CApplicationComponent(module)
{
	if (module != 0) {
		module->setController(id, this);
	}
}

CController::~CController()
{

}

void CController::init()
{
}

void CController::registerAction(const string &name, TAction action)
{
	_actions[name] = action;
}

void CController::run(const string &actionId, CHttpRequest * const request, CHttpResponse * const response) throw (CHttpException)
{
	TActionsMap::const_iterator iter = _actions.find(actionId);
	if (iter == _actions.end()) {
		throw CHttpException(404, "Route not found");
	}
	TAction action = iter->second;
	(this->*action)(request, response);
}
