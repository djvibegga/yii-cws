/*
 * CDbSchema.h
 *
 *  Created on: Feb 5, 2014
 *      Author: djvibegga
 */

#ifndef CDBSCHEMA_H_
#define CDBSCHEMA_H_

#include "base/CComponent.h"
#include "db/CDbException.h"
#include "db/schema/CDbTableSchema.h"

class CDbSchema: public CComponent
{
private:
	map<string, CDbTableSchema> _tables;

public:
	CDbSchema();
	virtual string getClassName() const;
	virtual ~CDbSchema();
	virtual string quoteTableName(const string & tableName) const;
	virtual string quoteColumnName(const string & columnName) const;
	void addTableSchema(const CDbTableSchema & tableSchema);
	CDbTableSchema getTableSchema(const string & tableName) const throw (CDbException);
};

#endif /* CDBSCHEMA_H_ */
