#include "base/CHttpException.h"

CHttpException::CHttpException(int status, const string &message, int code)
: CException(message, code, "undefined", -1)
{
	_statusCode = status;
}

int CHttpException::getStatus() const
{
	return _statusCode;
}
