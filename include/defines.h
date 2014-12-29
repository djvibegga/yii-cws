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
#include <boost/locale.hpp>

using namespace std;

#ifdef _UNICODE
#define _string wstring
#define _f(message) boost::locale::wformat(message.c_str())
#define _stringstream wstringstream
#define _regex boost::wregex
#define _(data) L##data
#else
#define _string string
#define _f(message) boost::locale::format(message.c_str())
#define _stringstream stringstream
#define _regex boost::regex
#define _(data) data
#endif

#endif /* DEFINES_H_ */
