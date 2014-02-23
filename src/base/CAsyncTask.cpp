/*
 * CAsyncTask.cpp
 *
 *  Created on: Feb 23, 2014
 *      Author: djvibegga
 */

#include "base/CAsyncTask.h"
#include <boost/thread.hpp>

TAsyncTaskMap CAsyncTask::_tasks;

CAsyncTask::CAsyncTask(IRunable * runable)
: _runable(runable)
{
}

CAsyncTask::~CAsyncTask()
{
	if (_runable != 0) {
		delete _runable;
	}
}

void CAsyncTask::init() throw (CException)
{
	if (_runable == 0) {
		throw CException("Bad pointer to runable instance was passed.");
	}
}

void CAsyncTask::run() throw (CException)
{
	boost::thread threadObj(
		boost::bind(&CAsyncTask::_runInThread, this, boost::ref(*_runable))
	);
}

void CAsyncTask::_runInThread(IRunable & runable)
{
	runable.init();
	runable.run();
}
