/*
 * Yii.cpp
 *
 *  Created on: Oct 16, 2013
 *      Author: djvibegga
 */

#include "base/Jvibetto.h"

CApplication * Jvibetto::app()
{
	return CApplication::getInstance();
}

CLogger & Jvibetto::getLogger()
{
	return CApplication::getInstance()->getLogger();
}

void Jvibetto::log(
	const string & message,
	const string & level,
	const string & category)
{
	getLogger().log(message, level, category);
}
