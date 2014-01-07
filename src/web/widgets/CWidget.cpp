/*
 * CWidget.cpp
 *
 *  Created on: Jan 5, 2014
 *      Author: djvibegga
 */
#include "web/widgets/CWidget.h"

int CWidget::_counter = 0;

CWidget::CWidget(CBaseController * owner)
: CView(owner),
  _id(""),
  skin("default")
{

}

CWidget::~CWidget()
{
}

string CWidget::getId(bool autoGenerate)
{
	if (!_id.empty()) {
		return _id;
	} else if (autoGenerate) {
		stringstream ss;
		ss << "jv" << CWidget::_counter++;
		return _id = ss.str();
	}
	return "";
}

void CWidget::setId(const string & id)
{
	_id = id;
}
