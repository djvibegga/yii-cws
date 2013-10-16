/*
 * Yii.cpp
 *
 *  Created on: Oct 16, 2013
 *      Author: djvibegga
 */

#include "base/YiiBase.h"

CApplication * YiiBase::app()
{
	return CApplication::getInstance();
}
