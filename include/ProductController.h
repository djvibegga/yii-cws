/*
 * ProductController.h
 *
 *  Created on: Oct 21, 2013
 *      Author: djvibegga
 */

#ifndef PRODUCTCONTROLLER_H_
#define PRODUCTCONTROLLER_H_

#include <web/CController.h>

class ProductController: public CController
{
public:
	ProductController();
	virtual ~ProductController();
	void init();
	void actionTest(CHttpRequest * const request, CWebApplication * const app);
};

#endif /* PRODUCTCONTROLLER_H_ */
