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
typedef boost::shared_ptr<IView> TViewPtr;
#define ACTION(pointer) CALLEE(TAction, pointer)

class CController: public CBaseController, public CApplicationComponent
{
private:
	TActionsMap _actions;
	boost::filesystem::path _viewPath;
	boost::shared_ptr<IView> _layout;

protected:
	virtual bool beforeRender(const string & view);
	void afterRender(const string & view, _string &output);

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
	virtual _string renderPartial(const string & view, const cpptempl::data_map & data, bool ret = false, bool processOutput = false) throw (CException);
	virtual _string renderPartial(IView & viewInstance, bool ret = false, bool processOutput = false) throw (CException);
	virtual string getLayoutFile(const string & layoutName) throw (CException);
	virtual TViewPtr getLayout() const;
	virtual void setLayout(const string & layout);
	virtual void setLayout(TViewPtr layout);
	virtual _string render(const string & view, const cpptempl::data_map & data, bool ret = false) throw (CException);
	virtual _string render(IView & viewInstance, bool ret = false) throw (CException);
	virtual _string processOutput(const _string & output);
};

#endif /* CCONTROLLER_H_ */
