#include "base/CHttpException.h"

CHttpException::CHttpException(int status, const string &message, int code = 0)
: CException(message, code, "undefined", -1)
{
	_statusCode = status;
}
