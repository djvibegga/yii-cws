/*
 * CProfiler.h
 *
 *  Created on: Jan 7, 2014
 *      Author: djvibegga
 */

#ifndef CPROFILER_H_
#define CPROFILER_H_

#include "base/CComponent.h"
#include <sys/time.h>

struct TProfileLogItem
{
	string message;
	timeval timeBegin;
	timeval timeEnd;
};

typedef vector<TProfileLogItem> TProfileLogList;
typedef stack<TProfileLogItem> TProfileLogBuffer;

class CProfiler: public CComponent
{
private:
	TProfileLogList _items;
	TProfileLogBuffer _stack;
	static string _formatItem(const TProfileLogItem & item);

public:
	virtual string getClassName() const;
	void add(const TProfileLogItem & item);
	void begin(const string & msg);
	void end();
	void logItems();
};

#ifdef JV_DEBUG
#define PROFILE_BEGIN(msg) \
	CApplication::getInstance()->getProfiler().begin(msg);

#define PROFILE_END() \
	CApplication::getInstance()->getProfiler().end();
#else

#define PROFILE_BEGIN(message)
#define PROFILE_END()

#endif

#endif /* CPROFILER_H_ */
