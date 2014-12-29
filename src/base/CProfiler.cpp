/*
 * CProfiler.cpp
 *
 *  Created on: Jan 7, 2014
 *      Author: djvibegga
 */

#include "base/CProfiler.h"
#include "base/Jvibetto.h"
#include <sstream>

string CProfiler::_formatItem(const TProfileLogItem & item)
{
	unsigned long microsecBegin = ((unsigned long long)item.timeBegin.tv_sec * 1000000) + item.timeBegin.tv_usec;
	unsigned long microsecEnd = ((unsigned long long)item.timeEnd.tv_sec * 1000000) + item.timeEnd.tv_usec;
	stringstream ss;
	ss << item.message << ". Microtime duration: " << microsecEnd - microsecBegin;
	return ss.str();
}

void CProfiler::logItems()
{
	for (TProfileLogList::const_iterator iter = _items.begin(); iter != _items.end(); ++iter) {
		Jvibetto::log(_formatItem(*iter), CLogger::LEVEL_PROFILE);
	}
	_items.clear();
}

void CProfiler::add(const TProfileLogItem & item)
{
	_items.push_back(item);
}

void CProfiler::begin(const string & msg)
{
	TProfileLogItem item;
	gettimeofday(&item.timeBegin, 0);
	item.message = msg;
	_stack.push(item);
}

void CProfiler::end()
{
	TProfileLogItem item = _stack.top();
	gettimeofday(&item.timeEnd, 0);
	add(item);
	_stack.pop();
}
