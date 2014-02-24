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

class CProfiler: public CComponent
{
private:
	TProfileLogList _items;
	static string _formatItem(const TProfileLogItem & item);

public:
	void add(const TProfileLogItem & item);
	void logItems();
};

#ifdef JV_DEBUG
#define PROFILE_BEGIN(msg) \
	TProfileLogItem item; \
	gettimeofday(&item.timeBegin, 0); \
	item.message = msg; \

#define PROFILE_END() \
	gettimeofday(&item.timeEnd, 0);\
	CApplication::getInstance()->getProfiler().add(item);

#else

#define PROFILE_BEGIN(message)
#define PROFILE_END()

#endif

#endif /* CPROFILER_H_ */
