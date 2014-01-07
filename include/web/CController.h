/*
 * CController.h
 *
 *  Created on: Oct 19, 2013
 *      Author: djvibegga
 */

#ifndef CCONTROLLER_H_
#define CCONTROLLER_H_

#include <base/CHttpException.h>
#include <web/CBaseController.h>
#include <web/CHttpRequest.h>
#include <web/CHttpResponse.h>
#include <string>
#include <map>

using namespace std;

typedef void(CController::*TAction)(CHttpRequest * const, CHttpResponse * );
typedef map<string, TAction> TActionsMap;
#define ACTION(pointer) CALLEE(TAction, pointer)

class CController: public CBaseController, public CApplicationComponent
{
private:
	TActionsMap _actions;
	boost::filesystem::path _viewPath;

public:
	CController(const string & id, CModule * module = 0);
	virtual ~CController();
	virtual void init();
	void registerAction(const string &name, TAction action);
	virtual void run(const string &actionId, CHttpRequest * const request, CHttpResponse * response) throw (CHttpException);
	virtual string getViewFile(const string & viewName) const throw (CException);
	boost::filesystem::path resolveViewPath() const;
	boost::filesystem::path getViewPath() const;
	void setViewPath(const string & path);
	void setViewPath(const boost::filesystem::path & path);
	virtual string resolveViewFile(
	    const string & viewName,
	    const boost::filesystem::path & viewPath,
	    const boost::filesystem::path & basePath,
	    const boost::filesystem::path & moduleViewPath) const throw (CException);
	virtual string renderPartial(const string & view, cpptempl::data_map & data, bool ret = false, bool processOutput = false) throw (CException);
};

#endif /* CCONTROLLER_H_ */
