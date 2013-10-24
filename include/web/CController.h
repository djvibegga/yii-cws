/*
 * CController.h
 *
 *  Created on: Oct 19, 2013
 *      Author: djvibegga
 */

#ifndef CCONTROLLER_H_
#define CCONTROLLER_H_

#include "base/CApplicationComponent.h"
#include <base/CHttpException.h>
#include "web/CHttpRequest.h"
#include <string>
#include <map>

using namespace std;

class CWebApplication;

typedef void(CController::*TAction)(CHttpRequest * const, CWebApplication * const);
typedef map<string, TAction> TActionsMap;

class CController: public CApplicationComponent
{
private:
	TActionsMap _actions;

public:
	CController(const string & id, CModule * module = 0);
	virtual ~CController();
	virtual void init();
	void registerAction(const string &name, TAction action);
	virtual void run(const string &actionId, CHttpRequest * const request, CWebApplication * const app) throw (CHttpException);
};

#endif /* CCONTROLLER_H_ */
