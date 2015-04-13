/*
 * CSecurityManager.h
 *
 *  Created on: Mar 19, 2015
 *      Author: djvibegga
 */

#ifndef CSECURITYMANAGER_H_
#define CSECURITYMANAGER_H_

#include "base/CApplicationComponent.h"

class CSecurityManager: public CApplicationComponent
{
private:
	static const string STATE_VALIDATION_KEY;
	static const string STATE_ENCRYPTION_KEY;

public:

	string validationKey;
	string encryptionKey;

	CSecurityManager(CModule * module);
	CSecurityManager(const string &id, CModule * module);
	virtual string getClassName() const;
	virtual void init();
	string encrypt(const string & data, const string & key = "");
	string decrypt(const string & data, const string & key = "");
	string generateRandomString(unsigned int length, bool cryptographicallyStrong = true);
	string generateRandomBytes(unsigned int length, bool cryptographicallyStrong = true);
	string hashData(const string & data, const string & key = "");
	string validateData(const string & data, const string & key = "");
	string computeHMAC(const string & data, const string key = "");

protected:
	virtual string resolveValidationKey();
	virtual string resolveEncryptionKey();
	virtual void applyConfig(const xml_node & config);
};

#endif /* CSECURITYMANAGER_H_ */
