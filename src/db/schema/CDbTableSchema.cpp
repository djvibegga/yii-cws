/*
 * CDbTableSchema.cpp
 *
 *  Created on: Apr 12, 2015
 *      Author: djvibegga
 */

#include "db/schema/CDbTableSchema.h"

CDbTableSchema::~CDbTableSchema()
{
}

void CDbTableSchema::setTableName(const string & tableName)
{
	_tableName = tableName;
}

string CDbTableSchema::getTableName() const
{
	return _tableName;
}

void CDbTableSchema::setPrimaryKey(const vector<string> & key)
{
	_pk = key;
}

vector<string> CDbTableSchema::getPrimaryKey() const
{
	return _pk;
}
