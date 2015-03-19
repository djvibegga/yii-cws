/*
 * CHash.h
 *
 *  Created on: Feb 12, 2014
 *      Author: djvibegga
 */

#ifndef CHASH_H_
#define CHASH_H_

#include <string>
#include <cryptopp/osrng.h>

using namespace std;
using namespace CryptoPP;

class CHash
{
private:
	static AutoSeededRandomPool _prng;

public:
	static unsigned int crc32(const string & input);
	static string md5(const string & input);
	static string sha1(const string & input);
	static string createInitializationVector(unsigned int length);
};

#endif /* CHASH_H_ */
