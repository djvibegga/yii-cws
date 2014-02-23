/*
 * CAsyncTask.h
 *
 *  Created on: Feb 23, 2014
 *      Author: djvibegga
 */

#ifndef CASYNCTASK_H_
#define CASYNCTASK_H_

#include "interfaces.h"

class CAsyncTask;

typedef map<long, CAsyncTask*> TAsyncTaskMap;

class CAsyncTask: public IRunable
{
private:
	IRunable * _runable;
	static TAsyncTaskMap _tasks;
	void _runInThread(IRunable & runable);

public:
	CAsyncTask(IRunable * runable);
	virtual ~CAsyncTask();
	virtual void init() throw (CException);
	virtual void run() throw (CException);
};

#endif /* CASYNCTASK_H_ */
