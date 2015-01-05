/*
 * CLayoutView.h
 *
 *  Created on: Feb 9, 2014
 *      Author: djvibegga
 */

#ifndef CLAYOUTVIEW_H_
#define CLAYOUTVIEW_H_

#include "web/CView.h"

class CLayoutView: public CView
{
public:
	string content;
	CLayoutView(const CBaseController * owner = 0);
	virtual void init();
};


#endif /* CLAYOUTVIEW_H_ */
