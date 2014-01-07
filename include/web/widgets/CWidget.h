/*
 * CWidget.h
 *
 *  Created on: Jan 5, 2014
 *      Author: djvibegga
 */

#ifndef CWIDGET_H_
#define CWIDGET_H_

#include "web/CView.h"

class CWidget: public CView
{
private:
	static int _counter;
	string _id;

public:
	string actionPrefix;
	string skin;

	CWidget(const CBaseController * owner = 0);
	virtual ~CWidget();
	string getId(bool autoGenerate = true);
	void setId(const string & id);
};

#endif /* CWIDGET_H_ */
