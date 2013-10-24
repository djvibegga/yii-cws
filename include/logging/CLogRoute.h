/*
 * CLogRoute.h
 *
 *  Created on: Oct 23, 2013
 *      Author: djvibegga
 */

#ifndef CLOGROUTE_H_
#define CLOGROUTE_H_

#include "base/CComponent.h"
#include "logging/CLogger.h"

class CLogRoute: public CComponent
{
public:

	bool enabled;
	TLogLevelList levels;
	TLogCategoryList categories;
	TLogCategoryList except;
	TLogElementList logs;

	CLogRoute();
	virtual ~CLogRoute();
	virtual void init() = 0;
	void setLevels(const string & levels);
	void setCategories(const string & categories);
	void setExcept(const string & except);
	virtual void collectLogs(CLogger & logger, bool processLogs = false);

protected:

	virtual string formatLogMessage(const SLogElement & element) const;
	virtual void processLogs(TLogElementList & logs) = 0;
};

#endif /* CLOGROUTE_H_ */
