/*
 * CController.h
 *
 *  Created on: Oct 19, 2013
 *      Author: djvibegga
 */

#ifndef CCONTROLLER_H_
#define CCONTROLLER_H_

#include "base/CApplicationComponent.h"
#include "base/CHttpException.h"
#include "web/CHttpRequest.h"
#include "web/CHttpResponse.h"
#include <string>
#include <map>

using namespace std;

class CWebApplication;

typedef void(CController::*TAction)(CHttpRequest * const, CHttpResponse * );
typedef map<string, TAction> TActionsMap;
#define ACTION(pointer) CALLEE(TAction, pointer)

class CController: public CApplicationComponent
{
private:
	TActionsMap _actions;

public:
	CController(const string & id, CModule * module = 0);
	virtual ~CController();
	virtual void init();
	void registerAction(const string &name, TAction action);
	virtual void run(const string &actionId, CHttpRequest * const request, CHttpResponse * response) throw (CHttpException);
};

#endif /* CCONTROLLER_H_ */
