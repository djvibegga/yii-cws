/*
 * CProfiler.h
 *
 *  Created on: Jan 7, 2014
 *      Author: djvibegga
 */

#ifndef CPROFILER_H_
#define CPROFILER_H_

#include <list>
#include <sys/time.h>
#include <string>
#include "config.h"

using namespace std;

struct TProfileLogItem
{
	string message;
	timeval timeBegin;
	timeval timeEnd;
};

typedef list<TProfileLogItem> TProfileLogList;

class CProfiler
{
public:
	static TProfileLogList items;
	static string formatItem(const TProfileLogItem & item);
	static void logItems();
};

#ifdef JV_DEBUG
#define PROFILE_BEGIN(msg) \
	timeval timeBegin; \
	gettimeofday(&timeBegin, 0); \
	TProfileLogItem item; \
	item.message = msg; \
	item.timeBegin = timeBegin; \
	CProfiler::items.push_back(item)

#define PROFILE_END() \
	timeval timeEnd;\
	gettimeofday(&timeEnd, 0);\
	CProfiler::items.back().timeEnd = timeEnd

#else

#define PROFILE_BEGIN(message)
#define PROFILE_END()

#endif

#endif /* CPROFILER_H_ */
