/*
 * CTemplateView.h
 *
 *  Created on: Jan 7, 2014
 *      Author: djvibegga
 */

#ifndef CTEMPLATEVIEW_H_
#define CTEMPLATEVIEW_H_

#include "web/CView.h"

class CTemplateView: public CView
{
private:
	string _viewFile;
	CDT _data;

public:
	CTemplateView(const string & viewFile, CDT & data, const CBaseController * owner = 0);
	CTemplateView(const string & viewFile, const CBaseController * owner = 0);
	virtual string getClassName() const;
	virtual ~CTemplateView();
	void setData(CDT & data);
	CDT & getData();
	virtual void init();
	virtual void run() throw (CException);
};

#endif /* CTEMPLATEVIEW_H_ */
