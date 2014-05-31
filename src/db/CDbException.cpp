/*
 * CDbException.cpp
 *
 *  Created on: Oct 26, 2013
 *      Author: djvibegga
 */

#include "db/CDbException.h"

CDbException::CDbException(const string &message)
: CException(message)
{
}

CDbException::CDbException(const string &message, int code, const string &file, int line)
: CException(message, (int)code, file, line)
{
}
