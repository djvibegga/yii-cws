/*
 * CWebUser.h
 *
 *  Created on: Feb 27, 2014
 *      Author: djvibegga
 */

#ifndef CWEBUSER_H_
#define CWEBUSER_H_

#include "base/CApplicationComponent.h"

class CWebUser: public CApplicationComponent
{
public:
	static const time_t DEFAULT_AUTH_TIMEOUT;

	bool allowAutoLogin;
	bool autoRenewCookie;
	bool autoUpdateFlash;
	string guestName;
	TRouteStruct loginUrl;
	string identityCookie;
	time_t authTimeout;
	time_t absoluteAuthTimeout;
	string loginRequiredAjaxResponse;
	CWebUser(CModule * module);
	CWebUser(const string &id, CModule * module);
};


#endif /* CWEBUSER_H_ */
