/*
 * TestWidget.h
 *
 *  Created on: Jan 7, 2014
 *      Author: djvibegga
 */

#ifndef TESTWIDGET_H_
#define TESTWIDGET_H_

#include "web/widgets/CWidget.h"

class TestWidget: public CWidget
{
public:
	TestWidget(const CBaseController * owner = 0);
	virtual ~TestWidget();
	virtual boost::filesystem::path getLocalViewPath() const;
	virtual void init();
	virtual void run() throw (CException);
};


#endif /* TESTWIDGET_H_ */
