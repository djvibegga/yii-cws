/*
 * CDbCommandBuilder.cpp
 *
 *  Created on: Oct 25, 2013
 *      Author: djvibegga
 */

#include "db/schema/CDbCommandBuilder.h"
#include "base/CStringUtils.h"

CTablePrimaryKey::CTablePrimaryKey()
{
}

void CTablePrimaryKey::setValue(const string & key, const string & value)
{
	map<string, string>::const_iterator found = _strValues.find(key);
	if (found == _strValues.end()) {
		_keyNames.push_back(key);
	}
	_strValues[key] = value;
}

void CTablePrimaryKey::setValue(const string & key, unsigned long value)
{
	map<string, unsigned long>::const_iterator found = _ulongValues.find(key);
	if (found == _ulongValues.end()) {
		_keyNames.push_back(key);
	}
	_ulongValues[key] = value;
}

void CTablePrimaryKey::setValue(const string & key, long value)
{
	map<string, long>::const_iterator found = _longValues.find(key);
	if (found == _longValues.end()) {
		_keyNames.push_back(key);
	}
	_longValues[key] = value;
}

string CTablePrimaryKey::getStringValue(const string & key)
{
	return _strValues[key];
}

unsigned long CTablePrimaryKey::getULongValue(const string & key)
{
	return _ulongValues[key];
}

long CTablePrimaryKey::getLongValue(const string & key)
{
	return _longValues[key];
}

vector<string> CTablePrimaryKey::getKeyNames() const
{
	return _keyNames;
}

bool CTablePrimaryKey::getIsComposite() const
{
	return _keyNames.size() > 1;
}

void CTablePrimaryKey::bind(const CDbSchema & schema, const string & tableName, CDbCriteria & criteria)
{
	for (map<string, string>::iterator iter = _strValues.begin(); iter != _strValues.end(); ++iter) {
		criteria.compare(schema.quoteTableName(tableName) + "." + schema.quoteColumnName(iter->first), iter->second);
	}
	for (map<string, long>::iterator iter = _longValues.begin(); iter != _longValues.end(); ++iter) {
		criteria.compare(schema.quoteTableName(tableName) + "." + schema.quoteColumnName(iter->first), iter->second);
	}
	for (map<string, unsigned long>::iterator iter = _ulongValues.begin(); iter != _ulongValues.end(); ++iter) {
		criteria.compare(schema.quoteTableName(tableName) + "." + schema.quoteColumnName(iter->first), iter->second);
	}
}

const string CDbCommandBuilder::PARAM_PREFIX = ":jnp";

CDbCommandBuilder::CDbCommandBuilder(CDbConnection * connection, const CDbSchema & schema)
: CComponent(),
  _schema(schema),
  _connection(connection)
{
}

string CDbCommandBuilder::getClassName() const
{
	return "CDbCommandBuilder";
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

CDbCommand CDbCommandBuilder::createFindCommand(const string & table, const CDbCriteria & criteria, const string & alias)
{
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
	sql += " " + select + " FROM " + table + " " + finalAlias;
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

void CDbCommandBuilder::bindValues(CDbCommand & command, const CCommandParameterMap & values) const
{
	int n = values.size();
	if (n == 0) {
		return;
	}
	for(CCommandParameterMap::const_iterator iter = values.begin(); iter != values.end(); ++iter) {
		string name = iter->first;
		if (name[0] != ':') {
			name = ":" + name;
		}
		command.bindParam(name, iter->second);
	}
}

unsigned long CDbCommandBuilder::getLastInsertId(const string & tableName)
{
	return _connection->getLastInsertId(tableName);
}

CDbCommand CDbCommandBuilder::createCountCommand(const string & table, const string & alias)
{
	return createCountCommand(table, CDbCriteria(), alias);
}

CDbCommand CDbCommandBuilder::createCountCommand(const string & table, const CDbCriteria & criteria, const string & alias)
{
	string tableAlias = alias;
	if (!criteria.alias.empty()) {
		tableAlias = criteria.alias;
	}
	tableAlias = _schema.quoteTableName(tableAlias);

	string select = CStringUtils::implode(", ", criteria.select);
	string sql;
	if (!criteria.group.empty() || !criteria.having.empty()) {
		if (!criteria.alias.empty()) {
			tableAlias = criteria.alias;
		}
		sql = string(criteria.distinct ? "SELECT DISTINCT" : "SELECT")
			+ " " + select + " FROM " + table + " " + tableAlias;
		sql = applyJoin(sql, criteria.join);
		sql = applyCondition(sql, criteria.condition);
		sql = applyGroup(sql, criteria.group);
		sql = applyHaving(sql, criteria.having);
		sql = "SELECT COUNT(*) FROM (" + sql + ") sq";
	} else {
		if (select.find("count") != ::string::npos) {
			sql = "SELECT " + select;
		} else if (criteria.distinct) {
			vector<string> pk = _schema.getTableSchema(table).getPrimaryKey();
			string pkString;
			if (pk.size() > 1) {
				for (vector<string>::iterator iter = pk.begin(); iter != pk.end(); ++iter) {
					*iter = tableAlias + "." + _schema.quoteColumnName(*iter);
				}
				pkString = CStringUtils::implode(",", pk);
			} else {
				pkString = pk[0];
			}
			sql = "SELECT COUNT(DISTINCT " + pkString + ")";
		} else {
			sql = "SELECT COUNT(*)";
		}
		sql += " FROM " + _schema.quoteTableName(table) + " " + tableAlias;
		sql = applyJoin(sql, criteria.join);
		sql = applyCondition(sql, criteria.condition);
	}

	// Suppress binding of parameters belonging to the ORDER clause. Issue #1407.
//	if($criteria->order && $criteria->params)
//	{
//		$params1=array();
//		preg_match_all('/(:\w+)/',$sql,$params1);
//		$params2=array();
//		preg_match_all('/(:\w+)/',$this->applyOrder($sql,$criteria->order),$params2);
//		foreach(array_diff($params2[0],$params1[0]) as $param)
//			unset($criteria->params[$param]);
//	}

	// Do the same for SELECT part.
//	if($criteria->select && $criteria->params)
//	{
//		$params1=array();
//		preg_match_all('/(:\w+)/',$sql,$params1);
//		$params2=array();
//		preg_match_all('/(:\w+)/',$sql.' '.(is_array($criteria->select) ? implode(', ',$criteria->select) : $criteria->select),$params2);
//		foreach(array_diff($params2[0],$params1[0]) as $param)
//			unset($criteria->params[$param]);
//	}

	CDbCommand command = _connection->createCommand(sql);
	bindValues(command, criteria.params);
	return command;
}

CDbCommand CDbCommandBuilder::createDeleteCommand(const string & table)
{
	return createDeleteCommand(table, CDbCriteria());
}

CDbCommand CDbCommandBuilder::createDeleteCommand(const string & table, const CDbCriteria & criteria)
{
	string sql = "DELETE FROM " + _schema.quoteTableName(table);
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

CDbCommand CDbCommandBuilder::createInsertCommand(const string & table, const CCommandParameterMap & data) throw (CException)
{
	if (data.empty()) {
		throw CException("Can\'t create insert command, because values list is empty.");
	}
	vector<string> fields;
	CCommandParameterMap values;
	vector<string> placeholders;
	int i = 0;
	for (CCommandParameterMap::const_iterator iter = data.begin(); iter != data.end(); ++iter) {
		CDbCommandParameter * parameter = iter->second.get();
		CDbCommandParameterExpression * pExpression = dynamic_cast<CDbCommandParameterExpression*>(parameter);
		fields.push_back(_schema.quoteColumnName(iter->first));
		if (pExpression) {
			placeholders.push_back(pExpression->expression);
			for (CCommandParameterMap::const_iterator param = pExpression->params.begin(); param != pExpression->params.end(); ++param) {
				values[param->first] = param->second;
			}
		} else {
			CDbCommandParameterValue * pValue = dynamic_cast<CDbCommandParameterValue*>(parameter);
			string paramName = PARAM_PREFIX + CStringUtils::fromInt(i);
			placeholders.push_back(paramName);
			pValue->name = paramName;
			values[paramName] = iter->second;
			++i;
		}
	}
	if (fields.empty()) {
		throw CException("Can\'t create insert command, because values list is empty.");
	}
	string sql = "INSERT INTO " + _schema.quoteTableName(table)
		+ " (" + CStringUtils::implode(", ", fields) + ")"
		+ " VALUES (" + CStringUtils::implode(", ", placeholders) + ")";

	CDbCommand command = _connection->createCommand(sql);
	bindValues(command, values);
	return command;
}

CDbCommand CDbCommandBuilder::createMultipleInsertCommand(const string & table, const TMultipleInsertData & data) throw (CException)
{
	if (data.empty()) {
		throw CException("Data can\'t be empty when you are creating multiple insert command.");
	}

	vector<string> columnInsertNames;
	const CCommandParameterMap & firstDataElement = data[0];
	for (CCommandParameterMap::const_iterator iter = firstDataElement.begin(); iter != firstDataElement.end(); ++iter) {
		columnInsertNames.push_back(_schema.quoteColumnName(iter->first));
	}

	string sql = "INSERT INTO " + _schema.quoteTableName(table)
		+ " (" + CStringUtils::implode(",", columnInsertNames) + ") VALUES ";

	int i = 0;
	int step = 0;
	int stepsCount = data.size();
	CCommandParameterMap values;

	for (vector<CCommandParameterMap>::const_iterator iter = data.begin(); iter != data.end(); ++iter) {
		vector<string> placeholders;
		for (CCommandParameterMap::const_iterator element = iter->begin(); element != iter->end(); ++element) {
			CDbCommandParameter * parameter = element->second.get();
			CDbCommandParameterExpression * pExpression = dynamic_cast<CDbCommandParameterExpression*>(parameter);
			if (pExpression) {
				placeholders.push_back(pExpression->expression);
				for (CCommandParameterMap::const_iterator param = pExpression->params.begin(); param != pExpression->params.end(); ++param) {
					values[param->first] = param->second;
				}
			} else {
				CDbCommandParameterValue * pValue = dynamic_cast<CDbCommandParameterValue*>(parameter);
				string paramName = PARAM_PREFIX + CStringUtils::fromInt(i);
				placeholders.push_back(paramName);
				pValue->name = paramName;
				values[paramName] = element->second;
				++i;
			}
		}
		sql += "(" + CStringUtils::implode(",", placeholders) + ")";
		++step;
		if (step < stepsCount) {
			sql += ",";
		}
	}

	CDbCommand command = _connection->createCommand(sql);
	bindValues(command, values);
	return command;
}

CDbCommand CDbCommandBuilder::createUpdateCommand(const string & table, const CCommandParameterMap & data) throw (CException)
{
	return createUpdateCommand(table, data, CDbCriteria());
}

CDbCommand CDbCommandBuilder::createUpdateCommand(const string & table, const CCommandParameterMap & data, const CDbCriteria & criteria) throw (CException)
{
	vector<string> fields;
	CCommandParameterMap values;
	int i = 0;
	for (CCommandParameterMap::const_iterator iter = data.begin(); iter != data.end(); ++iter) {
		CDbCommandParameter * parameter = iter->second.get();
		CDbCommandParameterExpression * pExpression = dynamic_cast<CDbCommandParameterExpression*>(parameter);
		if (pExpression) {
			fields.push_back(_schema.quoteColumnName(iter->first) + "=" + pExpression->expression);
		} else {
			CDbCommandParameterValue * pValue = dynamic_cast<CDbCommandParameterValue*>(parameter);
			if (pValue) {
				string paramName = PARAM_PREFIX + CStringUtils::fromInt(i);
				fields.push_back(_schema.quoteColumnName(iter->first) + "=" + paramName);
				pValue->name = paramName;
				values[paramName] = iter->second;
				++i;
			}
		}
	}

	if (fields.empty()) {
		throw CDbException("No columns are being updated for table '" + table + "'.");
	}
	string sql = "UPDATE " + _schema.quoteTableName(table) + " SET " + CStringUtils::implode(",", fields);
	sql = applyJoin(sql, criteria.join);
	sql = applyCondition(sql, criteria.condition);
	sql = applyOrder(sql, criteria.order);
	sql = applyLimit(sql, criteria.limit, criteria.offset);

	CDbCommand command = _connection->createCommand(sql);
	bindValues(command, values.mergeWith(criteria.params));
	return command;
}

CDbCommand CDbCommandBuilder::createUpdateCounterCommand(const string & table, const TUpdateCountersData & counters) throw (CException)
{
	return createUpdateCounterCommand(table, counters, CDbCriteria());
}

CDbCommand CDbCommandBuilder::createUpdateCounterCommand(const string & table, const TUpdateCountersData & counters, const CDbCriteria & criteria) throw (CException)
{
	vector<string> fields;
	for (TUpdateCountersData::const_iterator iter = counters.begin(); iter != counters.end(); ++iter) {
		string columnName = _schema.quoteColumnName(iter->first);
		if (iter->second < 0) {
			fields.push_back(columnName + "=" + columnName + "-" + CStringUtils::fromLong(abs(iter->second)));
		} else {
			fields.push_back(columnName + "=" + columnName + "+" + CStringUtils::fromLong(iter->second));
		}
	}
	if (fields.empty()) {
		throw CDbException("No counter columns are being updated for table '" + table + "'.");
	}

	string sql = "UPDATE " + _schema.quoteTableName(table) + " SET " + CStringUtils::implode(",", fields);
	sql = applyJoin(sql, criteria.join);
	sql = applyCondition(sql, criteria.condition);
	sql = applyOrder(sql, criteria.order);
	sql = applyLimit(sql, criteria.limit, criteria.offset);

	CDbCommand command = _connection->createCommand(sql);
	bindValues(command, criteria.params);
	return command;
}

CDbCriteria CDbCommandBuilder::createPkCriteria(const string & table, CTablePrimaryKey & pk)
{
	CDbCriteria criteria;
	pk.bind(_schema, table, criteria);
	return criteria;
}
