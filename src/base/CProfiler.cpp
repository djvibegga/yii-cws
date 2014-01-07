/*
 * CProfiler.cpp
 *
 *  Created on: Jan 7, 2014
 *      Author: djvibegga
 */

#include "base/CProfiler.h"
#include <sstream>

TProfileLogList CProfiler::items;

string CProfiler::formatItem(const TProfileLogItem & item)
{
	long microsecBegin = ((unsigned long long)item.timeBegin.tv_sec * 1000000) + item.timeBegin.tv_usec;
	long microsecEnd = ((unsigned long long)item.timeEnd.tv_sec * 1000000) + item.timeEnd.tv_usec;
	stringstream ss;
	ss << item.message << ". Microtime duration: " << microsecEnd - microsecBegin;
	return ss.str();
}