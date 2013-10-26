/*
 * CHttpResponse.h
 *
 *  Created on: Oct 26, 2013
 *      Author: djvibegga
 */

#ifndef CHTTPRESPONSE_H_
#define CHTTPRESPONSE_H_

#include "base/CApplicationComponent.h"

class CWebApplication;

class CHttpResponse: public CApplicationComponent
{
private:
	bool _startedOutput;
	const CWebApplication * _app;

public:
    CHttpResponse(const CWebApplication * app);
    ~CHttpResponse();
    void echo(const string & content);
    CHttpResponse & operator<< (const string &right);
};

#endif /* CHTTPRESPONSE_H_ */
