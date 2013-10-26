/*
 * CException.h
 *
 *  Created on: Oct 14, 2013
 *      Author: djvibegga
 */

#ifndef CEXCEPTION_H_
#define CEXCEPTION_H_

#include <string>

using namespace std;

class CException
{
private:
	string _message;
	int _code;
	string _file;
	int _line;

public:
	CException(const string &message);
	CException(const string &message, int code, const string &file = "undefined", int line = -1);
	string getMessage() const;
	int getCode() const;
	string getFile() const;
	int getLine() const;
	string getFullMessage() const;
};

#endif /* CEXCEPTION_H_ */
