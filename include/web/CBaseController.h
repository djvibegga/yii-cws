/*
 * CBaseController.h
 *
 *  Created on: 26 םמ‗ב. 2013 ד.
 *      Author: djvibegga
 */

#ifndef CBASECONTROLLER_H_
#define CBASECONTROLLER_H_

#include <base/CComponent.h>
#include <base/CException.h>
#include <stack>

using namespace std;

typedef stack<CComponent*> TWidgetStack;

class CBaseController: public CComponent, public IRenderingContext
{
private:
    TWidgetStack _widgetStack;

public:
    virtual string getViewFile(const string & viewName) const throw (CException) = 0;
    virtual string renderFile(const string & viewFile, cpptempl::data_map * data = 0, bool ret = false);
    virtual string renderInternal(const string & viewFile, cpptempl::data_map * data = 0, bool ret = false) const;
};

#endif /* CBASECONTROLLER_H_ */
