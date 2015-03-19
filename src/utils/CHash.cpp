/*
 * CHash.cpp
 *
 *  Created on: Feb 12, 2014
 *      Author: djvibegga
 */

#include "utils/CHash.h"
#include <cryptopp/md5.h>
#include <crypto++/hex.h>
#include <cryptopp/sha.h>
#include <crypto++/filters.h>
#include "boost/crc.hpp"

AutoSeededRandomPool CHash::_prng;

unsigned int CHash::crc32(const string & input)
{
	boost::crc_32_type result;
	result.process_bytes(input.data(), input.length());
	return result.checksum();
}

string CHash::md5(const string & input)
{
	CryptoPP::MD5 hash;
	byte digest[ CryptoPP::MD5::DIGESTSIZE ];
	hash.CalculateDigest(digest, (byte*)input.c_str(), input.length());

	CryptoPP::HexEncoder encoder;
	string output;
	encoder.Attach(new CryptoPP::StringSink(output));
	encoder.Put(digest, sizeof(digest));
	encoder.MessageEnd();

	return output;
}

string CHash::sha1(const string & input)
{
	CryptoPP::SHA1 hash;
	byte digest[ CryptoPP::SHA1::DIGESTSIZE ];
	hash.CalculateDigest(digest, (byte*)input.c_str(), input.length());

	CryptoPP::HexEncoder encoder;
	string output;
	encoder.Attach(new CryptoPP::StringSink(output));
	encoder.Put(digest, sizeof(digest));
	encoder.MessageEnd();

	return output;
}

string CHash::createInitializationVector(unsigned int length)
{
	byte iv[length];
	_prng.GenerateBlock((byte*)iv, length);
	return string(reinterpret_cast<const char*>(iv), length);
}
