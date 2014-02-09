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

class CHttpResponse: public CApplicationComponent, public IOutputBuffer
{
private:
	bool _startedOutput;
	const CWebApplication * _app;

public:
    CHttpResponse(CWebApplication * app);
    ~CHttpResponse();

    virtual _string getContent() const;
    virtual void echo(const _string & content);
    virtual IOutputBuffer & operator<< (const _string &right);
};

#endif /* CHTTPRESPONSE_H_ */
