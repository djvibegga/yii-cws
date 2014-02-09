/*
 * MyLayoutView.cpp
 *
 *  Created on: Feb 9, 2014
 *      Author: djvibegga
 */

#include "MyLayoutView.h"

MyLayoutView::MyLayoutView(const CBaseController * owner)
: CLayoutView(owner)
{
}

void MyLayoutView::run() throw (CException)
{
	*this << _("<!DOCTYPE html>"
		"<html>"
			"<head>"
				"<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">"
				"<title></title>"
			"</head>"
			"<body>"
				"<div id=\"content\">")
					<< content
			<< _("</div>"
			"</body>"
		"</html>");
}
