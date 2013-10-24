
#include "logging/CLogRoute.h"
#include "base/CStringUtils.h"

CLogRoute::CLogRoute()
: enabled(true)
{
}

CLogRoute::~CLogRoute()
{
}

void CLogRoute::setLevels(const string & levels)
{
	this->levels = CStringUtils::explode(",", levels);
}

void CLogRoute::setCategories(const string & categories)
{
	this->categories = CStringUtils::explode(",", categories);
}

void CLogRoute::setExcept(const string & except)
{
	this->except = CStringUtils::explode(",", except);
}

string CLogRoute::formatLogMessage(const SLogElement & element) const
{
	stringstream ss;
	ss << element.time
	   << " [" << element.level << "] ["
	   << element.category << "] "
	   << element.message;
	return ss.str();
}

void CLogRoute::collectLogs(CLogger & logger, bool processLogs)
{
	TLogElementList logs = logger.getLogs(levels, categories, except);
	if (this->logs.empty()) {
		this->logs = logs;
	} else {
		this->logs.insert(this->logs.end(), logs.begin(), logs.end());
	}
	if (processLogs && !this->logs.empty()) {
		this->processLogs(this->logs);
		this->logs.clear();
	}
}
