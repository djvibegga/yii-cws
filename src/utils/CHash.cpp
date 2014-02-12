/*
 * CHash.cpp
 *
 *  Created on: Feb 12, 2014
 *      Author: djvibegga
 */

#include "utils/CHash.h"
#include "boost/crc.hpp"

unsigned int CHash::crc32(const string & input)
{
	boost::crc_32_type result;
	result.process_bytes(input.data(), input.length());
	return result.checksum();
}

