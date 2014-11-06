/*
 * MyUserIdentity.cpp
 *
 *  Created on: Nov 6, 2014
 *      Author: djvibegga
 */

#include "MyUserIdentity.h"

MyUserIdentity::MyUserIdentity(string login, string password)
: _login(login),
  _password(password),
  _isAuthenticated(false)
{

}

bool MyUserIdentity::authenticate()
{
	return _isAuthenticated = _login == "admin" && _password == "admin";
}

bool MyUserIdentity::getIsAuthenticated() const
{
	return _isAuthenticated;
}

string MyUserIdentity::getId() const
{
	return _isAuthenticated ? "admin" : "guest";
}

_string MyUserIdentity::getName() const
{
	return _isAuthenticated ? _("admin") : _("guest");
}

TPersistentStateMap MyUserIdentity::getPersistentStates() const
{
	return TPersistentStateMap();
}
