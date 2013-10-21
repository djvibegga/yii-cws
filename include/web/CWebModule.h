/*
 * CWebModule.h
 *
 *  Created on: Oct 19, 2013
 *      Author: djvibegga
 */

#ifndef CWEBMODULE_H_
#define CWEBMODULE_H_

#include "base/CModule.h"
#include "web/CController.h"

class CWebModule: public CModule
{
private:
	TControllerMap _controllerMap;

public:
	CWebModule(const string &id);
	CWebModule(const string &id, CWebModule * parent);
	virtual ~CWebModule();

	virtual bool hasController(const string & name) const;
	virtual CController * getController(const string & name) const;
	virtual void setController(const string & name, CController * instance);
};

#endif /* CWEBMODULE_H_ */
