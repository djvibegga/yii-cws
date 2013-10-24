/*
 * Jvibetto.h
 *
 *  Created on: Oct 16, 2013
 *      Author: djvibegga
 */

#ifndef YIIBASE_H_
#define YIIBASE_H_

#include "base/CApplication.h"

class Jvibetto
{
public:
	static CApplication * app();
	static CLogger & getLogger();
	static void log(
		const string & message,
		const string & level = "info",
		const string & category = "application");
};

#endif /* YIIBASE_H_ */
