/*
 * CWebModule.h
 *
 *  Created on: Oct 19, 2013
 *      Author: djvibegga
 */

#ifndef CWEBMODULE_H_
#define CWEBMODULE_H_

#include <base/CModule.h>
#include <web/CController.h>

class CWebModule: public CModule, public IHasLayout
{
private:
	TControllerMap _controllerMap;
	boost::filesystem::path _layoutPath;

protected:
	virtual boost::filesystem::path resolveLayoutPath();
	virtual boost::filesystem::path resolveBasePath() const;

public:
	string layout;

	CWebModule(const string &id);
	CWebModule(const string &id, CModule * parent);
	virtual string getClassName() const;
	virtual ~CWebModule();
	virtual void init();

	virtual bool hasController(const string & name) const;
	virtual CController * getController(const string & name) const;
	virtual void setController(const string & name, CController * instance);
	virtual boost::filesystem::path getLayoutPath() const;
	virtual void setLayoutPath(const boost::filesystem::path & layoutPath);
};

#endif /* CWEBMODULE_H_ */
