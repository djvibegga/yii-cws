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
	TProfileLogItem item; \
	gettimeofday(&item.timeBegin, 0); \
	item.message = msg; \

#define PROFILE_END() \
	gettimeofday(&item.timeEnd, 0);\
	CProfiler::items.push_back(item);

#else

#define PROFILE_BEGIN(message)
#define PROFILE_END()

#endif

#endif /* CPROFILER_H_ */
