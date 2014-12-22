/*
 * defines.h
 *
 *  Created on: Feb 8, 2014
 *      Author: djvibegga
 */

#ifndef DEFINES_H_
#define DEFINES_H_

#include "config.h"
#include <string>

using namespace std;

#ifdef _UNICODE
#define _string wstring
#define _f(message) boost::locale::wformat(message.c_str())
#define _stringstream wstringstream
#define _(data) L##data
#define _to_utf8(data) cpptempl::wide_to_utf8(data)
#define utf8_to_(data) cpptempl::utf8_to_wide(data)
#else
#define _string string
#define _f(message) boost::locale::format(message.c_str())
#define _stringstream stringstream
#define _(data) data
#define _to_utf8(data) data
#define utf8_to_(data) data
#endif

#endif /* DEFINES_H_ */
