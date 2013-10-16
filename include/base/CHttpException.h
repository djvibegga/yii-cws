/*
 * CHttpException.h
 *
 *  Created on: Oct 16, 2013
 *      Author: djvibegga
 */

#ifndef CHTTPEXCEPTION_H_
#define CHTTPEXCEPTION_H_

#include "base/CException.h"

class CHttpException: public CException
{
private:
	int _statusCode;

public:
	CHttpException(int status, const string &message, int code);
};

#endif /* CHTTPEXCEPTION_H_ */
