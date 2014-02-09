/*
 * MyLayoutView.h
 *
 *  Created on: Feb 9, 2014
 *      Author: djvibegga
 */

#ifndef MYLAYOUTVIEW_H_
#define MYLAYOUTVIEW_H_

#include "web/CLayoutView.h"

class MyLayoutView: public CLayoutView
{
public:
	MyLayoutView(const CBaseController * owner = 0);
	virtual void run() throw (CException);
};

#endif /* MYLAYOUTVIEW_H_ */
