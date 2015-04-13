/*
 * CSecurityManager.cpp
 *
 *  Created on: Mar 19, 2015
 *      Author: djvibegga
 */

#include "base/CSecurityManager.h"
#include "base/Jvibetto.h"
#include "base/CStringUtils.h"
#include "utils/CHash.h"

#include <cryptopp/aes.h>
#include <cryptopp/modes.h>
#include <cryptopp/hex.h>
#include <cryptopp/hmac.h>

#include <iostream>
#include <boost/assign.hpp>

using namespace std;
using namespace CryptoPP;

const string CSecurityManager::STATE_VALIDATION_KEY = "CSecurityManager.validationKey";
const string CSecurityManager::STATE_ENCRYPTION_KEY = "CSecurityManager.encryptionKey";

CSecurityManager::CSecurityManager(CModule * module)
: CApplicationComponent("securityManager", module)
{
}

CSecurityManager::CSecurityManager(const string &id, CModule * module)
: CApplicationComponent(id, module)
{
}

string CSecurityManager::getClassName() const
{
	return "CSecurityManager";
}

void CSecurityManager::init()
{
	CApplicationComponent::init();
	if (validationKey.empty()) {
		validationKey = resolveValidationKey();
	}
	if (encryptionKey.empty()) {
		encryptionKey = resolveEncryptionKey();
	}
}

string CSecurityManager::resolveValidationKey()
{
	CApplication * app = Jvibetto::app();
	string key = _to_utf8(app->getGlobalState(STATE_VALIDATION_KEY));
	if (!key.empty()) {
		return key;
	} else {
		key = generateRandomString(32, true);
		if (key.empty()) {
			key = generateRandomString(32, false);
		}
		if (key.empty()) {
			throw CException("CSecurityManager::generateRandomString() cannot generate random string in the current environment.");
		}
		app->setGlobalState(STATE_VALIDATION_KEY, utf8_to_(key));
		return key;
	}
}

string CSecurityManager::resolveEncryptionKey()
{
	CApplication * app = Jvibetto::app();
	string key = _to_utf8(app->getGlobalState(STATE_ENCRYPTION_KEY));
	if (!key.empty()) {
		return key;
	} else {
		key = generateRandomString(32, true);
		if (key.empty()) {
			key = generateRandomString(32, false);
		}
		if (key.empty()) {
			throw CException("CSecurityManager::generateRandomString() cannot generate random string in the current environment.");
		}
		app->setGlobalState(STATE_ENCRYPTION_KEY, utf8_to_(key));
		return key;
	}
}

string CSecurityManager::encrypt(const string & data, const string & key)
{
	string ret;
	string ivString = CHash::createInitializationVector(AES::BLOCKSIZE);

	string encriptionKey = key.empty() ? CHash::md5(this->encryptionKey) : key;
	CBC_Mode<AES>::Encryption encryption((byte*)encriptionKey.c_str(), encriptionKey.size(), (byte*)ivString.c_str());

	StringSource ss( data, true,
		new StreamTransformationFilter(encryption,
			new StringSink( ret )
		)
	);

	return ivString + ret;
}

string CSecurityManager::decrypt(const string & data, const string & key)
{
	string ret;
	string encriptionKey = key.empty() ? CHash::md5(this->encryptionKey) : key;

	string ivString(data.substr(0, AES::BLOCKSIZE));
	CBC_Mode<AES>::Decryption decryption((byte*)encriptionKey.c_str(), encriptionKey.size(), (byte*)ivString.c_str());

	StringSource decryptor( data.substr(ivString.length()), true,
		new StreamTransformationFilter(decryption,
			new StringSink(ret)
		)
	);

	return ret;
}

void CSecurityManager::applyConfig(const xml_node & config)
{
	PARSE_XML_CONF_STRING_PROPERTY(config, validationKey, "validationKey");
	PARSE_XML_CONF_STRING_PROPERTY(config, encryptionKey, "encryptionKey");
}

string CSecurityManager::generateRandomString(unsigned int length, bool cryptographicallyStrong)
{
	string randomBytes = generateRandomBytes(length + 2, cryptographicallyStrong);
	if (randomBytes.empty()) {
		return "";
	} else {
		TReplacementMap tr = boost::assign::map_list_of ("+", "_") ("/", "~");
		return CStringUtils::strtr(CStringUtils::base64Encode(randomBytes).substr(0, length), tr);
	}
}

string CSecurityManager::generateRandomBytes(unsigned int length, bool cryptographicallyStrong)
{
	return CHash::createInitializationVector(length);
}

string CSecurityManager::hashData(const string & data, const string & key)
{
	return computeHMAC(data, key) + data;
}

string CSecurityManager::validateData(const string & data, const string & key)
{
	unsigned int len = computeHMAC("test").length();
	if ( data.length() >= len) {
		string hmac = data.substr(0, len);
		string data2 = data.substr(len, data.length());
		return hmac == computeHMAC(data2, key) ? data2 : "";
	}
	return "";
}

string CSecurityManager::computeHMAC(const string & data, const string key)
{
	string validationKey = key.empty() ? this->validationKey : key;
	HMAC<SHA256> hmac((const byte*)validationKey.c_str(), validationKey.size());
	string ret;
	StringSource ss(data, true,
		new HashFilter(hmac,
			new HexEncoder(
				new StringSink(ret)
			)
		)
	);
	return ret;
}
