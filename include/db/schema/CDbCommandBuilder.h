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

typedef vector<CCommandParameterMap> TMultipleInsertData;
typedef map<string, long> TUpdateCountersData;

class CTablePrimaryKey
{
private:
	map<string, string> _strValues;
	map<string, unsigned long> _ulongValues;
	map<string, long> _longValues;
	vector<string> _keyNames;

public:
	CTablePrimaryKey();

	void setValue(const string & key, const string & value);
	void setValue(const string & key, unsigned long value);
	void setValue(const string & key, long value);

	string getStringValue(const string & key);
	unsigned long getULongValue(const string & key);
	long getLongValue(const string & key);

	vector<string> getKeyNames() const;
	bool getIsComposite() const;

	void bind(const CDbSchema & schema, const string & tableName, CDbCriteria & criteria);
};

class CDbCommandBuilder: public CComponent
{
private:
	CDbSchema _schema;
	CDbConnection * _connection;

public:
	static const string PARAM_PREFIX;

	CDbCommandBuilder(CDbConnection * connection, const CDbSchema & schema);
	virtual string getClassName() const;
	virtual ~CDbCommandBuilder();
	CDbCommand createFindCommand(const string & table, const CDbCriteria & criteria, const string & alias = "t");
	CDbSchema & getSchema();
	const CDbConnection * getConnection() const;
	string applyJoin(const string & sql, const string & join) const;
	string applyCondition(const string & sql, const string & condition) const;
	string applyOrder(const string & sql, const string & orderBy) const;
	string applyLimit(const string & sql, int limit, int offset);
	string applyGroup(const string & sql, const string & group) const;
	string applyHaving(const string & sql, const string & having) const;
	void bindValues(CDbCommand & command, const CCommandParameterMap & values) const;
	unsigned long getLastInsertId(const string & tableName = "");
	CDbCommand createCountCommand(const string & table, const string & alias = "t");
	CDbCommand createCountCommand(const string & table, const CDbCriteria & criteria, const string & alias = "t");
	CDbCommand createDeleteCommand(const string & table);
	CDbCommand createDeleteCommand(const string & table, const CDbCriteria & criteria);
	CDbCommand createInsertCommand(const string & table, const CCommandParameterMap & data) throw (CException);
	CDbCommand createMultipleInsertCommand(const string & table, const TMultipleInsertData & data) throw (CException);
	CDbCommand createUpdateCommand(const string & table, const CCommandParameterMap & data) throw (CException);
	CDbCommand createUpdateCommand(const string & table, const CCommandParameterMap & data, const CDbCriteria & criteria) throw (CException);
	CDbCommand createUpdateCounterCommand(const string & table, const TUpdateCountersData & counters) throw (CException);
	CDbCommand createUpdateCounterCommand(const string & table, const TUpdateCountersData & counters, const CDbCriteria & criteria) throw (CException);
	CDbCriteria createPkCriteria(const string & table, CTablePrimaryKey & pk);
};

#endif /* CDBCOMMANDBUILDER_H_ */
