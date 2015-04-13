/*
 * CLogRouter.h
 *
 *  Created on: Oct 23, 2013
 *      Author: djvibegga
 */

#ifndef CLOGROUTER_H_
#define CLOGROUTER_H_

#include "base/CApplicationComponent.h"
#include "logging/CLogRoute.h"

typedef vector<CLogRoute*> TLogRouteList;

class CLogRouter: public CApplicationComponent
{
private:
	TLogRouteList _routes;

public:
	CLogRouter(CModule * module);
	CLogRouter(const string & id, CModule * module);
	virtual string getClassName() const;
	virtual ~CLogRouter();
	virtual void init();
	void addRoute(CLogRoute * route);
	void collectLogs(CEvent & event);
	void processLogs(CEvent & event);
};

#endif /* CLOGROUTER_H_ */
