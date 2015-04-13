/*
 * CWebUser.cpp
 *
 *  Created on: Feb 27, 2014
 *      Author: djvibegga
 */

#include "web/CWebUser.h"
#include "web/CHttpResponse.h"
#include "web/CWebApplication.h"
#include "base/Jvibetto.h"
#include "base/CStringUtils.h"
#include "utils/CHash.h"
#include "utils/CArchiver.h"

const time_t CWebUser::DEFAULT_AUTH_TIMEOUT = 840;
const string CWebUser::STATE_ID = "__id";
const string CWebUser::STATE_NAME = "__name";
const string CWebUser::STATE_AUTH_TIMEOUT  = "__authTimeout";
const string CWebUser::STATE_RETURN_URL = "__returnUrl";
const string CWebUser::STATES_VAR = "__states";
const string CWebUser::FLASH_KEY_PREFIX = "__flashes";

CWebUser::CWebUser(CModule * module)
: CApplicationComponent("user", module),
  allowAutoLogin(false),
  autoRenewCookie(false),
  guestName(_("guest")),
  identityCookie("identity"),
  authTimeout(DEFAULT_AUTH_TIMEOUT),
  absoluteAuthTimeout(DEFAULT_AUTH_TIMEOUT),
  loginRequiredAjaxResponse(_("session has expired"))
{
}

CWebUser::CWebUser(const string &id, CModule * module)
: CApplicationComponent(id, module),
  allowAutoLogin(false),
  autoRenewCookie(false),
  guestName(_("guest")),
  identityCookie("identity"),
  authTimeout(DEFAULT_AUTH_TIMEOUT),
  absoluteAuthTimeout(DEFAULT_AUTH_TIMEOUT),
  loginRequiredAjaxResponse(_("session has expired"))
{
}

string CWebUser::getClassName() const
{
	return "CWebUser";
}

void CWebUser::init()
{
	CApplicationComponent::init();
	if (_keyPrefix.empty()) {
		_keyPrefix = resolveStateKeyPrefix();
	}

	if (!CApplication::getIsWebWorkerInstance()) {
		return;
	}
}

void CWebUser::load()
{
	CHttpSession * session = dynamic_cast<CHttpSession*>(Jvibetto::app()->getComponent("session"));
	if (session) {
		session->open();
	}
	if (getIsGuest() && allowAutoLogin) {
		restoreFromCookie();
	} else if (autoRenewCookie && allowAutoLogin) {
		renewCookie();
	}
	updateAuthStatus();
}

void CWebUser::applyConfig(const xml_node & config)
{
	PARSE_XML_CONF_BOOL_PROPERTY(config, allowAutoLogin, "allowAutoLogin");
	PARSE_XML_CONF_BOOL_PROPERTY(config, autoRenewCookie, "autoRenewCookie");
	PARSE_XML_CONF_UINT_PROPERTY(config, authTimeout, "authTimeout");
	PARSE_XML_CONF_UINT_PROPERTY(config, absoluteAuthTimeout, "absoluteAuthTimeout");
}

bool CWebUser::getIsGuest() const
{
	return getState(STATE_ID).empty();
}

_string CWebUser::getState(const string & key, const _string defaultValue) const
{
	string dataKey = getStateKeyPrefix() + key;
	CHttpSession * session = dynamic_cast<CHttpSession*>(Jvibetto::app()->getComponent("session"));
	TSessionDataMap sessionData = session->getData();
	TSessionDataMap::const_iterator found = sessionData.find(dataKey);
	return found == sessionData.end() ? defaultValue : found->second;
}

TSessionDataMap CWebUser::findStates(const string & key) const
{
	string stateKeyPrefix = getStateKeyPrefix();
	string dataKey = stateKeyPrefix + key;
	CHttpSession * session = dynamic_cast<CHttpSession*>(Jvibetto::app()->getComponent("session"));
	TSessionDataMap sessionData = session->getData();
	TSessionDataMap ret;
	string keyBuffer;
	for (TSessionDataMap::const_iterator iter = sessionData.begin(); iter != sessionData.end(); ++iter) {
		if (iter->first.find(dataKey) == ::string::npos) {
			continue;
		}
		keyBuffer = iter->first;
		keyBuffer.erase(0, stateKeyPrefix.length());
		ret[keyBuffer] = iter->second;
	}
	return ret;
}

void CWebUser::setState(const string & key, const _string & value)
{
	string dataKey = getStateKeyPrefix() + key;
	CHttpSession * session = dynamic_cast<CHttpSession*>(Jvibetto::app()->getComponent("session"));
	TSessionDataMap & sessionData = session->getData();
	TSessionDataMap::iterator item = sessionData.find(dataKey);
	if (item == sessionData.end()) {
		if (!value.empty()) {
			sessionData[dataKey] = value;
		}
	} else {
		if (value.empty()) {
			sessionData.erase(item);
		} else {
			item->second = value;
		}
	}
}

void CWebUser::setState(const string & key, int value)
{
	_stringstream buffer;
	buffer << value;
	setState(key, buffer.str());
}

string CWebUser::getStateKeyPrefix() const
{
	return _keyPrefix;
}

void CWebUser::setStateKeyPrefix(const string & value)
{
	_keyPrefix = value;
}

string CWebUser::resolveStateKeyPrefix()
{
	string val = "jv_session_" + Jvibetto::app()->getId();
	return CHash::md5(val);
}

string CWebUser::getUserId() const
{
	return _to_utf8(getState(STATE_ID));
}

void CWebUser::setUserId(const string & value)
{
	setState(STATE_ID, utf8_to_(value));
}

_string CWebUser::getName() const
{
	_string ret = getState(STATE_NAME);
	return ret.empty() ? guestName : ret;
}

void CWebUser::setName(const _string & value)
{
	setState(STATE_NAME, value);
}

bool CWebUser::hasState(const string & key) const
{
	CHttpSession * session = dynamic_cast<CHttpSession*>(Jvibetto::app()->getComponent("session"));
	string sessionKey = getStateKeyPrefix() + key;
	TSessionDataMap & sessionData = session->getData();
	TSessionDataMap::const_iterator found = sessionData.find(sessionKey);
	return found != sessionData.end();
}

void CWebUser::clearStates()
{
	CHttpSession * session = dynamic_cast<CHttpSession*>(Jvibetto::app()->getComponent("session"));
	string keyPrefix = getStateKeyPrefix();
	size_t prefixLen = keyPrefix.length();
	TSessionDataMap & sessionData = session->getData();
	for (TSessionDataMap::iterator iter = sessionData.begin(); iter != sessionData.end(); ++iter) {
		if (iter->first.substr(0, prefixLen) == keyPrefix) {
			sessionData.erase(iter);
		}
	}
}

void CWebUser::updateAuthStatus()
{
	if (authTimeout != 0 && getIsGuest()) {
		string state = _to_utf8(getState(STATE_AUTH_TIMEOUT));
		if (!state.empty() != 0 && CStringUtils::toInt(state) < time(0)) {
			logout(false);
		} else {
			setState(STATE_AUTH_TIMEOUT, time(0) + authTimeout);
		}
	}
}

bool CWebUser::login(const IUserIdentity & identity, time_t duration) throw (CException)
{
	string id = identity.getId();
	TPersistentStateMap states = identity.getPersistentStates();
	if (beforeLogin(id, states, false)) {
		changeIdentity(id, identity.getName(), states);
		if (duration > 0) {
			if (allowAutoLogin) {
				saveToCookie(duration);
			} else {
				throw CException("allowAutoLogin must be set true in order to use cookie-based authentication.");
			}
		}
		afterLogin(false);
	}
	return !getIsGuest();
}

void CWebUser::logout(bool destroySession)
{
	if (beforeLogout()) {
		CWebApplication * app = dynamic_cast<CWebApplication*>(Jvibetto::app());
		if (allowAutoLogin) {
			CCookieCollection cookies = app->getRequest()->getCookies();
			CHttpResponse * response = app->getResponse();
			response->removeCookie(getStateKeyPrefix());
			if (identityCookie.empty()) {
				CHttpCookie cookie = createIdentityCookie(getStateKeyPrefix());
				response->removeCookie(cookie);
			}
		}
		if (destroySession) {
			CHttpSession * session = dynamic_cast<CHttpSession*>(app->getComponent("session"));
			session->destroy();
		} else {
			clearStates();
		}
		_access = TWebUserAccessMap();
		afterLogout();
	}
}


string CWebUser::getReturnUrl(const string & defaultUrl) const
{
	string defaultReturnUrl = "";
    if (defaultUrl.empty()) {
    	CUrlManager * urlManager = dynamic_cast<CUrlManager*>(Jvibetto::app()->getComponent("urlManager"));
    	CHttpRequest * request = dynamic_cast<CHttpRequest*>(Jvibetto::app()->getComponent("request"));
    	string defaultReturnUrl = urlManager->showScriptName
    		? request->getScriptUrl()
    		: request->getBaseUrl() + "/";
    } else {
    	defaultReturnUrl = CHtml::normalizeUrl(defaultUrl);
    }
    return _to_utf8(getState("__returnUrl", utf8_to_(defaultReturnUrl)));
}

void CWebUser::setReturnUrl(const string & value)
{
	setState(STATE_RETURN_URL, utf8_to_(value));
}


void CWebUser::loginRequired() throw (CHttpException)
{
	CWebApplication * app = dynamic_cast<CWebApplication*>(Jvibetto::app());
	CHttpRequest * request = app->getRequest();
	CHttpResponse * response = app->getResponse();

	if (!request->getIsAjaxRequest()) {
		setReturnUrl(request->getRequestUri());
	} else if (!loginRequiredAjaxResponse.empty()) {
		response->echo(loginRequiredAjaxResponse);
		app->end();
	}

	if (!loginUrl.path.empty()) {
		string url = app->getUrlManager()->createUrl(loginUrl);
		response->redirect(url);
	} else {
		throw CHttpException(403, "Login Required");
	}
}

bool CWebUser::beforeLogin(const string & id, const TPersistentStateMap & states, bool fromCookie)
{
	return true;
}

void CWebUser::afterLogin(bool fromCookie)
{
}

bool CWebUser::beforeLogout()
{
	return true;
}

void CWebUser::afterLogout()
{
}

void CWebUser::restoreFromCookie()
{
	CWebApplication * app = dynamic_cast<CWebApplication*>(Jvibetto::app());
	CHttpCookie cookie = app->getRequest()->getCookies()[getStateKeyPrefix()];
	CWebUserState data = parseStateFromCookie(cookie);

	if (data.id.empty()) {
		return;
	}

	if (beforeLogin(data.id, data.states, true)) {
		changeIdentity(data.id, data.name, data.states);
		if (autoRenewCookie) {
			cookie.expire = time(0) + data.duration;
			app->getResponse()->addCookie(cookie);
		}
		afterLogin(true);
	}
}

void CWebUser::renewCookie()
{
	CWebApplication * app = dynamic_cast<CWebApplication*>(Jvibetto::app());
	CHttpCookie cookie = app->getRequest()->getCookies()[getStateKeyPrefix()];
	CWebUserState data = parseStateFromCookie(cookie);

	if (!data.duration) {
		return;
	}

	cookie.expire = time(0) + data.duration;
	app->getResponse()->addCookie(cookie);
}

CWebUserState CWebUser::parseStateFromCookie(const CHttpCookie & cookie)
{
	CWebApplication * app = dynamic_cast<CWebApplication*>(Jvibetto::app());
	CWebUserState state;

	if (cookie.value.empty()) {
		return state;
	}
	string validatedValue = app->getSecurityManager()->validateData(CStringUtils::base64Decode(cookie.value));
	if (validatedValue.empty()) {
		return state;
	}

	CArchiver<CWebUserState>::load(validatedValue, state);
	return state;
}

void CWebUser::saveToCookie(time_t duration)
{
	CWebApplication * app = dynamic_cast<CWebApplication*>(Jvibetto::app());
	CHttpCookie cookie = createIdentityCookie(getStateKeyPrefix());
	cookie.expire = time(0) + duration;

	CWebUserState data;
	data.id = getUserId();
	data.name = getName();
	data.duration = duration;
	data.states = saveIdentityStates();
	cookie.value = CStringUtils::base64Encode(app->getSecurityManager()->hashData(CArchiver<CWebUserState>::save(data)));
	app->getResponse()->addCookie(cookie);
}

CHttpCookie CWebUser::createIdentityCookie(const string & name) const
{
	CHttpCookie cookie;
	cookie.name = name;
	return cookie;
}

_string CWebUser::getFlash(const string & key, const _string & defaultValue, bool remove)
{
	_string value = getState(FLASH_KEY_PREFIX + key, defaultValue);
	if (remove) {
		setFlash(key, _(""));
	}
	return value;
}

void CWebUser::setFlash(const string & key, const _string & value)
{
	setState(FLASH_KEY_PREFIX + key, value);
}

bool CWebUser::hasFlash(const string & key)
{
	return !getFlash(key, _(""), false).empty();
}

void CWebUser::changeIdentity(const string & id, const _string & name, TPersistentStateMap & states)
{
	CWebApplication * app = dynamic_cast<CWebApplication*>(Jvibetto::app());
	app->getSession()->regenerateId(true);
	setUserId(id);
	setName(name);
	loadIdentityStates(states);
}

TPersistentStateMap CWebUser::saveIdentityStates()
{
	return findStates(STATES_VAR);
}

void CWebUser::loadIdentityStates(const TPersistentStateMap & states)
{
	for (TPersistentStateMap::const_iterator iter = states.begin(); iter != states.end(); ++iter) {
		setState(iter->first, iter->second);
	}
}
