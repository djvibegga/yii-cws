/*
 * CDbCommandBuilder.cpp
 *
 *  Created on: Oct 25, 2013
 *      Author: djvibegga
 */

#include "db/schema/CDbCommandBuilder.h"
#include "base/CStringUtils.h"

CDbCommandBuilder::CDbCommandBuilder(CDbConnection * connection, const CDbSchema & schema)
: CComponent(),
  _schema(schema),
  _connection(connection)
{
}

CDbCommandBuilder::~CDbCommandBuilder()
{
}

CDbSchema & CDbCommandBuilder::getSchema()
{
	return _schema;
}

const CDbConnection * CDbCommandBuilder::getConnection() const
{
	return _connection;
}

CDbCommand CDbCommandBuilder::createFindCommand(const string & tableName, const CDbCriteria & criteria, const string & alias)
{
	//ensureTable($table);
	string select = criteria.select.empty() ? "*" : CStringUtils::implode(",", criteria.select);
	string finalAlias = alias;
	if (!criteria.alias.empty()) {
		finalAlias = criteria.alias;
	}
	finalAlias = _schema.quoteTableName(finalAlias);

	// issue 1432: need to expand * when SQL has JOIN
	/*if($select==='*' && !empty($criteria->join))
	{
		$prefix=$alias.'.';
		$select=array();
		foreach($table->getColumnNames() as $name)
			$select[]=$prefix.$this->_schema->quoteColumnName($name);
		$select=implode(', ',$select);
	}*/

	string sql = criteria.distinct ? "SELECT DISTINCT" : "SELECT";
	sql += " " + select + " FROM " + tableName + " " + finalAlias;
	sql = applyJoin(sql, criteria.join);
	sql = applyCondition(sql, criteria.condition);
	sql = applyGroup(sql, criteria.group);
	sql = applyHaving(sql, criteria.having);
	sql = applyOrder(sql, criteria.order);
	sql = applyLimit(sql, criteria.limit, criteria.offset);
	CDbCommand command = _connection->createCommand(sql);
	bindValues(command, criteria.params);
	return command;
}

string CDbCommandBuilder::applyJoin(const string & sql, const string & join) const
{
	if (join.empty()) {
		return sql;
	}
	return sql + " " + join;
}

string CDbCommandBuilder::applyCondition(const string & sql, const string & condition) const
{
	if (condition.empty()) {
		return sql;
	}
	return sql + " WHERE " + condition;
}

string CDbCommandBuilder::applyOrder(const string & sql, const string & orderBy) const
{
	if (orderBy.empty()) {
		return sql;
	}
	return sql + " ORDER BY " + orderBy;
}

string CDbCommandBuilder::applyLimit(const string & sql, int limit, int offset)
{
	stringstream ret;
	ret << sql;
	if (limit >= 0) {
		ret << " LIMIT " << limit;
	}
	if (offset > 0) {
		ret << " OFFSET " << offset;
	}
	return ret.str();
}

string CDbCommandBuilder::applyGroup(const string & sql, const string & group) const
{
	if (group.empty()) {
		return sql;
	}
	return sql + " GROUP BY " + group;
}

string CDbCommandBuilder::applyHaving(const string & sql, const string & having) const
{
	if (having.empty()) {
		return sql;
	}
	return sql + " HAVING " + having;
}

void CDbCommandBuilder::bindValues(CDbCommand & command, const TCommandParameterMap & values) const
{
	int n = values.size();
	if (n == 0) {
		return;
	}
	for(TCommandParameterMap::const_iterator iter = values.begin(); iter != values.end(); ++iter) {
		string name = iter->first;
		if (name[0] != ':') {
			name = ":" + name;
		}
		command.bindParam(name, iter->second);
	}
}
