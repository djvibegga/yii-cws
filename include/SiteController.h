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
	void actionIndex(CHttpRequest * const request, CHttpResponse * response) throw (CException);
	void actionAssetManager(CHttpRequest * const request, CHttpResponse * response) throw (CException);
	void actionSession(CHttpRequest * const request, CHttpResponse * response) throw (CException);
	void actionCookies(CHttpRequest * const request, CHttpResponse * response) throw (CException);
	void actionLogin(CHttpRequest * const request, CHttpResponse * response) throw (CException);
	void actionLogout(CHttpRequest * const request, CHttpResponse * response) throw (CException);
};

#endif /* SITECONTROLLER_H_ */
