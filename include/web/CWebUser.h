/*
 * CWebUser.h
 *
 *  Created on: Feb 27, 2014
 *      Author: djvibegga
 */

#ifndef CWEBUSER_H_
#define CWEBUSER_H_

#include "base/CApplicationComponent.h"
#include "web/CHttpCookie.h"
#include "base/CHttpException.h"
#include "web/CHttpSession.h"
#include <boost/serialization/access.hpp>
#include <boost/serialization/map.hpp>

typedef map<string, bool> TWebUserAccessMap;

class CWebUserState
{
public:
	string id;
	_string name;
	time_t duration;
	TPersistentStateMap states;

	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & id;
		ar & name;
		ar & duration;
		ar & states;
	}
};

class CWebUser: public CApplicationComponent
{
private:
	string _keyPrefix;
	TWebUserAccessMap _access;

public:
	static const time_t DEFAULT_AUTH_TIMEOUT;
	static const string STATE_ID;
	static const string STATE_NAME;
	static const string STATE_AUTH_TIMEOUT;
	static const string STATE_RETURN_URL;
	static const string STATES_VAR;
	static const string FLASH_KEY_PREFIX;

	bool allowAutoLogin;
	bool autoRenewCookie;
	_string guestName;
	TRouteStruct loginUrl;
	string identityCookie;
	time_t authTimeout;
	time_t absoluteAuthTimeout;
	_string loginRequiredAjaxResponse;
	CWebUser(CModule * module);
	CWebUser(const string &id, CModule * module);
	virtual void init();
	bool getIsGuest() const;
	bool hasState(const string & key) const;
	_string getState(const string & key, const _string defaultValue = _("")) const;
	TSessionDataMap findStates(const string & key) const;
	void setState(const string & key, const _string & value);
	void setState(const string & key, int value);
	void setStateKeyPrefix(const string & value);
	string getStateKeyPrefix() const;
	string getUserId() const;
	void setUserId(const string & value);
	_string getName() const;
	void setName(const _string & value);
	void clearStates();
	bool login(const IUserIdentity & identity, time_t duration = 0) throw (CException);
	void logout(bool destroySession = true);
	string getReturnUrl(const string & defaultUrl = "") const;
	void setReturnUrl(const string & value);
	void loginRequired() throw (CHttpException);
	_string getFlash(const string & key, const _string & defaultValue = _(""), bool remove = true);
	void setFlash(const string & key, const _string & value);
	bool hasFlash(const string & key);

protected:
	string resolveStateKeyPrefix();
	bool beforeLogin(const string & id, const TPersistentStateMap & states, bool fromCookie);
	void afterLogin(bool fromCookie);
	bool beforeLogout();
	void afterLogout();
	void restoreFromCookie();
	void renewCookie();
	void saveToCookie(time_t duration);
	CHttpCookie createIdentityCookie(const string & name) const;
	void changeIdentity(const string & id, const _string & name, TPersistentStateMap & states);
	TPersistentStateMap saveIdentityStates();
	void loadIdentityStates(const TPersistentStateMap & states);
	void updateAuthStatus();
};


#endif /* CWEBUSER_H_ */
