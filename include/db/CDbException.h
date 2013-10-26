/*
 * CDbException.h
 *
 *  Created on: Oct 26, 2013
 *      Author: djvibegga
 */

#ifndef CDBEXCEPTION_H_
#define CDBEXCEPTION_H_

#include "base/CException.h"

class CDbException: public CException
{
public:
	CDbException(const string &message);
	CDbException(const string &message, int code, const string &file = "undefined", int line = -1);
};

#endif /* CDBEXCEPTION_H_ */
