/*
 * CHash.h
 *
 *  Created on: Feb 12, 2014
 *      Author: djvibegga
 */

#ifndef CHASH_H_
#define CHASH_H_

#include <string>

using namespace std;

class CHash
{
public:
	static unsigned int crc32(const string & input);
	static string md5(const string & input);
};

#endif /* CHASH_H_ */
