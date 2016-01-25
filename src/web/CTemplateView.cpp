/*
 * CTemplateView.cpp
 *
 *  Created on: Jan 7, 2014
 *      Author: djvibegga
 */

#include "web/CTemplateView.h"
#include "base/CProfiler.h"
#include "base/Cws.h"
#include "base/CStringUtils.h"
#include "utils/CFile.h"

CTemplateView::CTemplateView(const string & viewFile, CDT & data, const CBaseController * owner)
: CView(owner),
  _viewFile(viewFile),
  _data(data)
{
}

CTemplateView::CTemplateView(const string & viewFile, const CBaseController * owner)
: CView(owner),
  _viewFile(viewFile)
{
}

string CTemplateView::getClassName() const
{
	return "CTemplateView";
}

CTemplateView::~CTemplateView()
{
}

void CTemplateView::setData(CDT & data)
{
	_data = data;
}

CDT & CTemplateView::getData()
{
	return _data;
}

void CTemplateView::init()
{
	CView::init();
}

void CTemplateView::run() throw (CException)
{
	PROFILE_BEGIN("CTemplateView::run(). View file: " + _viewFile);
	Cws::app()->getViewRenderer()->renderFile(0, _viewFile, _data, false);
	PROFILE_END();
}
