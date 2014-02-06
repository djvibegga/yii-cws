/*
 * CDbCommandBuilder.h
 *
 *  Created on: Oct 25, 2013
 *      Author: djvibegga
 */

#ifndef CDBCOMMANDBUILDER_H_
#define CDBCOMMANDBUILDER_H_

#include "db/CDbConnection.h"
#include "db/CDbCommand.h"
#include "db/CDbCriteria.h"
#include "db/schema/CDbSchema.h"
#include "db/schema/CDbCommandBuilder.h"

class CDbCommandBuilder: public CComponent
{
private:
	CDbSchema _schema;
	CDbConnection * _connection;

public:
	CDbCommandBuilder(CDbConnection * connection, const CDbSchema & schema);
	virtual ~CDbCommandBuilder();
	CDbCommand createFindCommand(const string & tableName, const CDbCriteria & criteria, const string & alias = "t");
	CDbSchema & getSchema();
	const CDbConnection * getConnection() const;
	string applyJoin(const string & sql, const string & join) const;
	string applyCondition(const string & sql, const string & condition) const;
	string applyOrder(const string & sql, const string & orderBy) const;
	string applyLimit(const string & sql, int limit, int offset);
	string applyGroup(const string & sql, const string & group) const;
	string applyHaving(const string & sql, const string & having) const;
	void bindValues(CDbCommand & command, const TCommandParameterMap & values) const;
};

#endif /* CDBCOMMANDBUILDER_H_ */
