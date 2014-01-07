/*
 * CLogger.cpp
 *
 *  Created on: Oct 23, 2013
 *      Author: djvibegga
 */

#include "logging/CLogger.h"
#include <time.h>
#include <boost/assign.hpp>
#include <algorithm>
#include "base/CEvent.h"

const string CLogger::LEVEL_TRACE = "trace";
const string CLogger::LEVEL_WARNING = "warning";
const string CLogger::LEVEL_ERROR = "error";
const string CLogger::LEVEL_INFO = "info";
const string CLogger::LEVEL_PROFILE = "profile";

CLogger::CLogger()
: _logCount(0),
  _processing(false),
  autoFlush(10000),
  autoDump(false)
{
}

TLogCategoryList CLogger::_lowerList(const vector<string> & list)
{
	vector<string> ret;
	string val;
	for (vector<string>::const_iterator iter = list.begin(); iter != list.end(); ++iter) {
		val = *iter;
		transform(val.begin(), val.end(), val.begin(), ::tolower);
		ret.push_back(val);
	}
	return ret;
}

void CLogger::log(const string & message, const string & level, const string & category)
{
	SLogElement element;
	element.message = message;
	element.level = level;
	element.category = category;
	element.time = ptime(from_time_t(time(0)));
	_logs.push_back(element);
	++_logCount;
	if (hasEventHandler("onLog")) {
		CEvent event(this, boost::assign::map_list_of("logItem", (void*)&element));
		onLog(event);
	}

	if (autoFlush > 0 && _logCount >= autoFlush && !_processing) {
		_processing = true;
		flush(autoDump);
		_processing = false;
	}
}

TLogElementList CLogger::getLogs(const TLogLevelList & levels)
{
	return getLogs(levels, TLogCategoryList(), TLogCategoryList());
}

TLogElementList CLogger::getLogs(const TLogLevelList & levels, const TLogCategoryList & categories)
{
	return getLogs(levels, categories, TLogCategoryList());
}

TLogElementList CLogger::getLogs(const TLogLevelList & levels, const TLogCategoryList & categories, const TLogCategoryList & except)
{
	_levels = _lowerList(levels);
	_categories = _lowerList(categories);
	_except = _lowerList(except);
	TLogElementList ret;
	SLogElement element;
	for (TLogElementList::const_iterator iter = _logs.begin(); iter != _logs.end(); ++iter) {
		if (!levels.empty() && _filterByLevel(*iter)) {
			ret.push_back(*iter);
		}
		if (!categories.empty() && _filterByCategory(*iter)) {
			ret.push_back(*iter);
		}
	}
	return ret;
}

bool CLogger::_filterByLevel(const SLogElement & element) const
{
	return find(_levels.begin(), _levels.end(), element.level) != _levels.end();
}

bool CLogger::_filterByCategory(const SLogElement & element) const
{
	return find(_categories.begin(), _categories.end(), element.category) != _categories.end() &&
			find(_except.begin(), _except.end(), element.category) == _except.end();
}

void CLogger::flush(bool dumpLogs)
{
	CEvent event(this, boost::assign::map_list_of("dumpLogs", (void*)&dumpLogs));
	onFlush(event);
	clearLogs();
}

void CLogger::clearLogs()
{
	_logs.clear();
	_logCount = 0;
}

void CLogger::onLog(CEvent & event)
{
	raiseEvent("onLog", event);
}

void CLogger::onFlush(CEvent & event)
{
	raiseEvent("onFlush", event);
}
