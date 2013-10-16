/*
 * CException.cpp
 *
 *  Created on: Oct 14, 2013
 *      Author: djvibegga
 */

#include "base/CException.h"
#include <sstream>
#include <stdlib.h>

using namespace std;

CException::CException(const string &message)
: _file("undefined"),
  _line(-1)
{
	_message = message;
	_code = 0;
}

CException::CException(const string &message, int code, const string &file, int line)
{
	_message = message;
	_code = code,
	_file = file;
	_line = line;
}

string CException::getMessage() const
{
	return _message;
}

int CException::getCode() const
{
	return _code;
}

string CException::getFile() const
{
	return _file;
}

int CException::getLine() const
{
	return _line;
}

string CException::getFullMessage() const
{
	stringstream ss;
	ss << "Exception has been thown: "
	   << _message
	   << ". File: " << (_file == "undefined" ? _file.c_str() : realpath(_file.c_str(), 0))
	   << ". Line: " << _line;
	return ss.str();
}
