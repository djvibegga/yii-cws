/*
 * SiteController.h
 *
 *  Created on: Oct 21, 2013
 *      Author: djvibegga
 */

#ifndef SITECONTROLLER_H_
#define SITECONTROLLER_H_

#include "web/CController.h"
#include "web/CWebApplication.h"
#include "web/CHttpRequest.h"

class SiteController: public CController
{
public:
	SiteController(CModule * parent);
	virtual ~SiteController();
	void init();
	void actionIndex(CHttpRequest * const request, CHttpResponse * response);
};

#endif /* SITECONTROLLER_H_ */
