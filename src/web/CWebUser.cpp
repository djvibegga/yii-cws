/*
 * CWebUser.cpp
 *
 *  Created on: Feb 27, 2014
 *      Author: djvibegga
 */

#include "web/CWebUser.h"
const time_t CWebUser::DEFAULT_AUTH_TIMEOUT = 840;

CWebUser::CWebUser(CModule * module)
: CApplicationComponent("user", module),
  allowAutoLogin(false),
  autoRenewCookie(false),
  autoUpdateFlash(true),
  guestName("guest"),
  identityCookie("identity"),
  authTimeout(DEFAULT_AUTH_TIMEOUT),
  absoluteAuthTimeout(DEFAULT_AUTH_TIMEOUT),
  loginRequiredAjaxResponse("session has expired")
{

}

CWebUser::CWebUser(const string &id, CModule * module)
: CApplicationComponent(id, module),
  allowAutoLogin(false),
  autoRenewCookie(false),
  autoUpdateFlash(true),
  guestName("guest"),
  identityCookie("identity"),
  authTimeout(DEFAULT_AUTH_TIMEOUT),
  absoluteAuthTimeout(DEFAULT_AUTH_TIMEOUT),
  loginRequiredAjaxResponse("session has expired")
{

}

