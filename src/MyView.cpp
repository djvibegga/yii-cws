/*
 * MyView.cpp
 *
 *  Created on: Feb 9, 2014
 *      Author: djvibegga
 */

#include "MyView.h"

MyView::MyView(const CBaseController * owner)
: CView(owner)
{
}

void MyView::init()
{
}

void MyView::run() throw (CException)
{
	*this << _("<table>"
					"<tr><td>ID</td><td>Name</td></tr>");
		for (TActiveRecordList::const_iterator iter = records.begin(); iter != records.end(); ++iter) {
			Goal * goal = dynamic_cast<Goal*>(iter->get());
			wstringstream ss;
			ss << goal->id;
			*this << _("<tr><td>") << ss.str() << _("</td>")
					  << _("<td>") << goal->name << _("</td></tr>");
		}
		*this << _("<table>"
			    "</div>"
			"</body>");
}
