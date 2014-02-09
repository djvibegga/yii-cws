/*
 * MyView.h
 *
 *  Created on: Feb 9, 2014
 *      Author: djvibegga
 */

#ifndef MYVIEW_H_
#define MYVIEW_H_

#include "web/CView.h"
#include "Goal.h"

class MyView: public CView
{
public:
	TActiveRecordList records;

	MyView(const CBaseController * owner = 0);
	void init();
	void run() throw (CException);
};


#endif /* MYVIEW_H_ */
