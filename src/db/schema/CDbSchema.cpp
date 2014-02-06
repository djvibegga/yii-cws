/*
 * CDbSchema.cpp
 *
 *  Created on: Feb 5, 2014
 *      Author: djvibegga
 */

#include "db/schema/CDbSchema.h"
#include "base/CStringUtils.h"

CDbSchema::CDbSchema()
{
}

CDbSchema::~CDbSchema()
{
}

string CDbSchema::quoteTableName(const string & tableName)
{
	string quoted = tableName;
	return "`" + CStringUtils::trim(quoted, "`") + "`";
}
