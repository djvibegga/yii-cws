/*
 * defines.h
 *
 *  Created on: Feb 8, 2014
 *      Author: djvibegga
 */

#ifndef DEFINES_H_
#define DEFINES_H_

#include "config.h"

using namespace std;

#ifdef _UNICODE
#define _string wstring
#define _(data) L##data
#else
#define _string string
#define _(data) data
#endif

#endif /* DEFINES_H_ */
