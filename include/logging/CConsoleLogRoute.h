/*
 * CConsoleLogRoute.h
 *
 *  Created on: Oct 26, 2013
 *      Author: djvibegga
 */

#ifndef CCONSOLELOGROUTE_H_
#define CCONSOLELOGROUTE_H_

#include "logging/CLogRoute.h"

class CConsoleLogRoute: public CLogRoute
{
public:
	CConsoleLogRoute();
	virtual string getClassName() const;
	virtual ~CConsoleLogRoute();
	virtual void init() throw (CException);

protected:
	virtual void processLogs(TLogElementList & logs);
};

#endif /* CCONSOLELOGROUTE_H_ */
