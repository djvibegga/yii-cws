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

string CDbSchema::getClassName() const
{
	return "CDbSchema";
}

CDbSchema::~CDbSchema()
{
}

string CDbSchema::quoteTableName(const string & tableName) const
{
	string quoted = tableName;
	return "`" + CStringUtils::trim(quoted, "`") + "`";
}

string CDbSchema::quoteColumnName(const string & columnName) const
{
	string quoted = columnName;
	return "`" + CStringUtils::trim(quoted, "`") + "`";
}

void CDbSchema::addTableSchema(const CDbTableSchema & tableSchema)
{
	_tables[tableSchema.getTableName()] = tableSchema;
}

CDbTableSchema CDbSchema::getTableSchema(const string & tableName) const throw (CDbException)
{
	map<string, CDbTableSchema>::const_iterator found = _tables.find(tableName);
	if (found != _tables.end()) {
		return found->second;
	}
	throw CDbException("Application hasn't defined table schema for the table '" + tableName + "'.");
}
