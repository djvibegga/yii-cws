/*
 * MyUserIdentity.h
 *
 *  Created on: Nov 6, 2014
 *      Author: djvibegga
 */

#ifndef MYUSERIDENTITY_H_
#define MYUSERIDENTITY_H_

#include "interfaces.h"

class MyUserIdentity: public IUserIdentity
{
private:
	string _login;
	string _password;
	bool _isAuthenticated;

public:
	MyUserIdentity(string login, string password);
	bool authenticate();
	bool getIsAuthenticated() const;
	string getId() const;
	_string getName() const;
	TPersistentStateMap getPersistentStates() const;
};

#endif /* MYUSERIDENTITY_H_ */
