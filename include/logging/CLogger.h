/*
 * CLogger.h
 *
 *  Created on: Oct 23, 2013
 *      Author: djvibegga
 */

#ifndef CLOGGER_H_
#define CLOGGER_H_

#include "base/CComponent.h"
#include <boost/date_time/posix_time/posix_time.hpp>

using namespace boost::posix_time;

struct SLogElement
{
	string message;
	string level;
	string category;
	ptime time;
};

typedef vector<SLogElement> TLogElementList;
typedef vector<string> TLogLevelList;
typedef vector<string> TLogCategoryList;

class CLogger: public CComponent
{
private:
	TLogElementList _logs;
	unsigned int _logCount;
	TLogLevelList _levels;
	TLogCategoryList _categories;
	TLogCategoryList _except;
	//private $_timings;
	bool _processing;

	static TLogCategoryList _lowerList(const vector<string> & list);
	bool _filterByLevel(const SLogElement & element) const;
	bool _filterByCategory(const SLogElement & element) const;

public:
	static const string LEVEL_TRACE;
	static const string LEVEL_WARNING;
	static const string LEVEL_ERROR;
	static const string LEVEL_INFO;
	static const string LEVEL_PROFILE;

	unsigned int autoFlush;
	bool autoDump;

	CLogger();
	virtual string getClassName() const;
	void log(const string & message, const string & level = "info", const string & category = "application");
	TLogElementList getLogs(const TLogLevelList & levels);
	TLogElementList getLogs(const TLogLevelList & levels, const TLogCategoryList & categories);
	TLogElementList getLogs(const TLogLevelList & levels, const TLogCategoryList & categories, const TLogCategoryList & except);
	void clearLogs();
	void flush(bool dumpLogs = false);
	void onFlush(CEvent & event);
	void onLog(CEvent & event);
};

#endif /* CLOGGER_H_ */
