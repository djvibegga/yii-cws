/*
 * CActiveRecord.cpp
 *
 *  Created on: Feb 5, 2014
 *      Author: djvibegga
 */

#include "db/ar/CActiveRecord.h"
#include "db/schema/CDbCommandBuilder.h"
#include "base/Cws.h"
#include "base/CEvent.h"

void CAttributeCollection::setValue(const string & key, const string & value)
{
	_strValues[key] = value;
}

void CAttributeCollection::setValue(const string & key, const wstring & value)
{
	_wstrValues[key] = value;
}

void CAttributeCollection::setValue(const string & key, unsigned long value)
{
	_ulongValues[key] = value;
}

void CAttributeCollection::setValue(const string & key, long value)
{
	_longValues[key] = value;
}

void CAttributeCollection::setValue(const string & key, double value)
{
	_doubleValues[key] = value;
}

bool CAttributeCollection::has(const string & key) const
{
	if (_strValues.find(key) != _strValues.end()) {
		return true;
	}
	if (_wstrValues.find(key) != _wstrValues.end()) {
		return true;
	}
	if (_longValues.find(key) != _longValues.end()) {
		return true;
	}
	if (_ulongValues.find(key) != _ulongValues.end()) {
		return true;
	}
	if (_doubleValues.find(key) != _doubleValues.end()) {
		return true;
	}
	return false;
}

string CAttributeCollection::getStringValue(const string & key) const
{
	map<string, string>::const_iterator found = _strValues.find(key);
	if (found != _strValues.end()) {
		return found->second;
	}
	return "";
}

wstring CAttributeCollection::getWStringValue(const string & key) const
{
	map<string, wstring>::const_iterator found = _wstrValues.find(key);
	if (found != _wstrValues.end()) {
		return found->second;
	}
	return L"";
}

unsigned long CAttributeCollection::getULongValue(const string & key) const
{
	map<string, unsigned long>::const_iterator found = _ulongValues.find(key);
	if (found != _ulongValues.end()) {
		return found->second;
	}
	return 0;
}

long CAttributeCollection::getLongValue(const string & key) const
{
	map<string, long>::const_iterator found = _longValues.find(key);
	if (found != _longValues.end()) {
		return found->second;
	}
	return 0;
}

double CAttributeCollection::getDoubleValue(const string & key) const
{
	map<string, double>::const_iterator found = _doubleValues.find(key);
	if (found != _doubleValues.end()) {
		return found->second;
	}
	return 0;
}

CCommandParameterMap CAttributeCollection::convertToCommandParameterMap(const vector<string> & ignoredNames) const
{
	CCommandParameterMap ret;
	for (map<string, string>::const_iterator iter = _strValues.begin(); iter != _strValues.end(); ++iter) {
		if (!ignoredNames.empty() && std::find(ignoredNames.begin(), ignoredNames.end(), iter->first) != ignoredNames.end()) {
			continue;
		}
		ret.set(iter->first, iter->second);
	}
	for (map<string, wstring>::const_iterator iter = _wstrValues.begin(); iter != _wstrValues.end(); ++iter) {
		if (!ignoredNames.empty() && std::find(ignoredNames.begin(), ignoredNames.end(), iter->first) != ignoredNames.end()) {
			continue;
		}
		ret.set(iter->first, iter->second);
	}
	for (map<string, long>::const_iterator iter = _longValues.begin(); iter != _longValues.end(); ++iter) {
		if (!ignoredNames.empty() && std::find(ignoredNames.begin(), ignoredNames.end(), iter->first) != ignoredNames.end()) {
			continue;
		}
		ret.set(iter->first, iter->second);
	}
	for (map<string, unsigned long>::const_iterator iter = _ulongValues.begin(); iter != _ulongValues.end(); ++iter) {
		if (!ignoredNames.empty() && std::find(ignoredNames.begin(), ignoredNames.end(), iter->first) != ignoredNames.end()) {
			continue;
		}
		ret.set(iter->first, iter->second);
	}
	for (map<string, double>::const_iterator iter = _doubleValues.begin(); iter != _doubleValues.end(); ++iter) {
		if (!ignoredNames.empty() && std::find(ignoredNames.begin(), ignoredNames.end(), iter->first) != ignoredNames.end()) {
			continue;
		}
		ret.set(iter->first, iter->second);
	}
	return ret;
}

const string CActiveRecord::LOG_CATEGORY = "system.db.ar.CActiveRecord";
const string CActiveRecord::SCENARIO_CREATE = "create";
const string CActiveRecord::SCENARIO_UPDATE = "update";

CActiveRecord::CActiveRecord(const string & scenario, CDbConnection * connection)
: CComponent(),
  _connection(connection),
  _scenario(scenario),
  _isNew(true),
  _alias("t")
{
}

CActiveRecord::~CActiveRecord()
{
}

void CActiveRecord::init()
{
	if (_connection == 0) {
		setDbConnection(dynamic_cast<CDbConnection*>(Cws::app()->getComponent("db")));
	}
}

void CActiveRecord::setDbConnection(CDbConnection * connection)
{
	_connection = connection;
}

CDbConnection * CActiveRecord::getDbConnection() const
{
	return _connection;
}

CDbCommandBuilder * CActiveRecord::getCommandBuilder() const
{
	return _connection->getCommandBuilder();
}

CDbTableSchema CActiveRecord::getTableSchema() const
{
	return _connection->getSchema().getTableSchema(tableName());
}

bool CActiveRecord::validate(const vector<string> & attributeNames)
{
	return true;
}

TActiveRecordList CActiveRecord::findAll() throw (CDbException)
{
	CDbCriteria criteria;
	return findAll(criteria);
}

TActiveRecordList CActiveRecord::findAll(const CDbCriteria & criteria) throw (CDbException)
{
#ifdef CWS_DEBUG
	Cws::trace(typeid(this).name() + string(".findAll()"), LOG_CATEGORY);
#endif
	return query(criteria, true);
}

TActiveRecordPtr CActiveRecord::find(const CDbCriteria & criteria) throw (CDbException)
{
	TActiveRecordList records = query(criteria, false);
	return records.empty() ? TActiveRecordPtr((CActiveRecord*)0) : records[0];
}

TActiveRecordPtr CActiveRecord::findByPk(CTablePrimaryKey & pk) throw (CDbException)
{
	Cws::trace(getClassName() + ".findByPk()", "system.db.ar.CActiveRecord");
	CDbCriteria criteria = getCommandBuilder()->createPkCriteria(getTableAlias(false, false), pk);
	return find(criteria);
}

TActiveRecordList CActiveRecord::query(const CDbCriteria & criteria, bool all) throw (CDbException)
{
	CDbCommand command = _connection->getCommandBuilder()->createFindCommand(tableName(), criteria);
	if (all) {
		CDbDataReader reader = command.queryAll();
		return populateRecords(reader, true);
	}
	TActiveRecordList ret;
	TDbRow row = command.queryRow();
	if (!row.empty()) {
		ret.push_back(populateRecord(row));
	}
	return ret;
}

TActiveRecordList CActiveRecord::populateRecords(CDbDataReader & data, bool callAfterFind)
{
	TActiveRecordList records;
	while (data.nextResult()) {
		TActiveRecordPtr record = populateRecord(data.readRow(), callAfterFind);
		if (record.get() != 0) {
			records.push_back(record);
		}
	}
	return records;
}

TActiveRecordPtr CActiveRecord::populateRecord(const TDbRow & attributes, bool callAfterFind)
{
	CActiveRecord * record = instantiate(attributes);
	record->setScenario(SCENARIO_UPDATE);
	record->setIsNewRecord(false);
	record->init();
	for (TDbRow::const_iterator iter = attributes.begin(); iter != attributes.end(); ++iter) {
		record->populateProperty(iter->first, iter->second);
	}
	record->setPrimaryKey(record->fetchPrimaryKey());
	//$record->attachBehaviors($record->behaviors());
	if (callAfterFind) {
		record->afterFind();
	}
	return TActiveRecordPtr(record);
}

void CActiveRecord::setScenario(const string & scenario)
{
	_scenario = scenario;
}

string CActiveRecord::getScenario() const
{
	return _scenario;
}

bool CActiveRecord::save(bool runValidation) throw (CDbException)
{
	return save(runValidation, vector<string>());
}

bool CActiveRecord::save(bool runValidation, const vector<string> & attributes) throw (CDbException)
{
	if (runValidation || validate(attributes)) {
		return getIsNewRecord() ? insert(attributes) : update(attributes);
	} else {
		return false;
	}
}

bool CActiveRecord::getIsNewRecord() const
{
	return _isNew;
}

void CActiveRecord::setIsNewRecord(bool value)
{
	_isNew = value;
}

void CActiveRecord::onBeforeSave(CEvent & event)
{
	raiseEvent("onBeforeSave", event);
}

void CActiveRecord::onAfterSave(CEvent & event)
{
	raiseEvent("onAfterSave", event);
}

void CActiveRecord::onBeforeDelete(CEvent & event)
{
	raiseEvent("onBeforeDelete", event);
}

void CActiveRecord::onAfterDelete(CEvent & event)
{
	raiseEvent("onAfterDelete", event);
}

void CActiveRecord::onBeforeFind(CEvent & event)
{
	raiseEvent("onBeforeFind", event);
}

void CActiveRecord::onAfterFind(CEvent & event)
{
	raiseEvent("onAfterFind", event);
}

void CActiveRecord::onBeforeCount(CEvent & event)
{
	raiseEvent("onBeforeCount", event);
}

bool CActiveRecord::beforeSave()
{
	if (hasEventHandler("onBeforeSave")) {
		CEvent event(this);
		onBeforeSave(event);
		return event.handled;
	}
	return true;
}

void CActiveRecord::afterSave()
{
	if (hasEventHandler("onAfterSave")) {
		CEvent event(this);
		onAfterSave(event);
	}
}

bool CActiveRecord::beforeDelete()
{
	if (hasEventHandler("onBeforeDelete")) {
		CEvent event(this);
		onBeforeDelete(event);
		return event.handled;
	}
	return true;
}

void CActiveRecord::afterDelete()
{
	if (hasEventHandler("onAfterDelete")) {
		CEvent event(this);
		onAfterDelete(event);
	}
}

void CActiveRecord::beforeFind()
{
	if (hasEventHandler("onBeforeFind")) {
		CEvent event(this);
		onBeforeFind(event);
	}
}

void CActiveRecord::afterFind()
{
	if (hasEventHandler("onAfterFind")) {
		CEvent event(this);
		onAfterFind(event);
	}
}

void CActiveRecord::beforeCount()
{
	if (hasEventHandler("onBeforeCount")) {
		CEvent event(this);
		onBeforeCount(event);
	}
}

bool CActiveRecord::insert(const vector<string> & attributes) throw (CDbException)
{
	if (!getIsNewRecord()) {
		throw new CDbException("The active record cannot be inserted to database because it is not new.");
	}
	if (beforeSave()) {
		Cws::trace(getClassName() + ".insert()" , "system.db.ar.CActiveRecord");
		CDbCommandBuilder * builder = getCommandBuilder();
		string table = tableName();
		vector<string> pkAttributes = getTableSchema().getPrimaryKey();
		CAttributeCollection attrValues = getAttributes(attributes);
		CDbCommand command = builder->createInsertCommand(table, attrValues.convertToCommandParameterMap(pkAttributes));
		if (command.execute()) {
			if (pkAttributes.size() == 1) {
				CAttributeCollection collection;
				collection.setValue(pkAttributes[0], builder->getLastInsertId(table));
				setAttributes(collection);
			}
			setPrimaryKey(fetchPrimaryKey());
			afterSave();
			setIsNewRecord(false);
			setScenario(SCENARIO_UPDATE);
			return true;
		}
	}
	return false;
}

bool CActiveRecord::update(const vector<string> & attributes) throw (CDbException)
{
	if (getIsNewRecord()) {
		throw CDbException("The active record cannot be updated because it is new.");
	}
	if (beforeSave()) {
		Cws::trace(getClassName() + ".update()", "system.db.ar.CActiveRecord");
		CTablePrimaryKey pk = getPrimaryKey();
		CAttributeCollection attrValues = getAttributes(attributes);
		updateByPk(pk, attrValues.convertToCommandParameterMap());
		setPrimaryKey(fetchPrimaryKey());
		afterSave();
		return true;
	}
	return false;
}

bool CActiveRecord::saveAttributes(const vector<string> & attributes) throw (CDbException)
{
	if (!getIsNewRecord()) {
		Cws::trace(getClassName() + ".saveAttributes()", "system.db.ar.CActiveRecord");
		CTablePrimaryKey pk = getPrimaryKey();
		CAttributeCollection attrValues = getAttributes(attributes);
		updateByPk(pk, attrValues.convertToCommandParameterMap());
		setPrimaryKey(fetchPrimaryKey());
		return true;
	} else {
		throw CDbException("The active record cannot be updated because it is new.");
	}
}

bool CActiveRecord::saveCounters(const TUpdateCountersData & counters) throw (CDbException)
{
	Cws::trace(getClassName() + ".saveCounters()", "system.db.ar.CActiveRecord");
	CDbCommandBuilder * builder = getCommandBuilder();
	string table = tableName();
	CTablePrimaryKey pk = getPrimaryKey();
	CDbCriteria criteria = builder->createPkCriteria(table, pk);
	CDbCommand command = builder->createUpdateCounterCommand(table, counters, criteria);
	return command.execute() > 0 && refresh();
}

bool CActiveRecord::remove() throw (CDbException)
{
	if (!getIsNewRecord()) {
		Cws::trace(getClassName() + ".remove()", "system.db.ar.CActiveRecord");
		if (beforeDelete()) {
			CTablePrimaryKey pk = getPrimaryKey();
			bool result = removeByPk(pk) > 0;
			afterDelete();
			return result;
		}
		return false;
	} else {
		throw CDbException("The active record cannot be deleted because it is new.");
	}
}

bool CActiveRecord::refresh() throw (CDbException)
{
	Cws::trace(getClassName() + ".refresh()", "system.db.ar.CActiveRecord");
	CTablePrimaryKey pk = getPrimaryKey();
	TActiveRecordPtr recordPtr = findByPk(pk);
	CActiveRecord * record = recordPtr.get();
	if (record) {
		setAttributes(record->getAttributes());
		return true;
	}
	return false;
}

unsigned long CActiveRecord::count() throw (CDbException)
{
	return count(CDbCriteria());
}

unsigned long CActiveRecord::count(const CDbCriteria & criteria) throw (CDbException)
{
	Cws::trace(getClassName() + ".count()", "system.db.ar.CActiveRecord");
	CDbCommandBuilder * builder = getCommandBuilder();
	beforeCount();
	//applyScopes(criteria);
	if (criteria.with.empty()) {
		return builder->createCountCommand(tableName(), criteria).queryScalar().asULong();
	} else {
//		$finder=$this->getActiveFinder($criteria->with);
//		return $finder->count($criteria);
		return 0;
	}
}

bool CActiveRecord::exists() throw (CDbException)
{
	CDbCriteria criteria;
	return exists(criteria);
}

bool CActiveRecord::exists(CDbCriteria & criteria) throw (CDbException)
{
	Cws::trace(getClassName() + ".exists()", "system.db.ar.CActiveRecord");
	CDbCommandBuilder * builder = getCommandBuilder();
	criteria.select.push_back("1");
	criteria.limit = 1;
	//applyScopes(criteria);

	if (criteria.with.empty()) {
		TDbRow row = builder->createFindCommand(tableName(), criteria, getTableAlias(false, false)).queryRow();
		return !row.empty();
	} else {
		criteria.select.push_back("*");
//		$finder=$this->getActiveFinder($criteria->with);
//		return $finder->count($criteria)>0;
		return 0;
	}
}

bool CActiveRecord::updateByPk(CTablePrimaryKey & pk, const CCommandParameterMap & attributes) throw (CDbException)
{
	Cws::trace(getClassName() + ".updateByPk()", "system.db.ar.CActiveRecord");
	CDbCommandBuilder * builder = getCommandBuilder();
	string table = tableName();
	CDbCriteria criteria = builder->createPkCriteria(table, pk);
	CDbCommand command = builder->createUpdateCommand(table, attributes, criteria);
	return command.execute() > 0;
}

unsigned long CActiveRecord::updateAll(const CCommandParameterMap & attributes, const CDbCriteria & criteria) throw (CDbException)
{
	Cws::trace(getClassName() + ".updateAll()", "system.db.ar.CActiveRecord");
	CDbCommandBuilder * builder = getCommandBuilder();
	CDbCommand command = builder->createUpdateCommand(tableName(), attributes, criteria);
	return command.execute();
}

unsigned long CActiveRecord::updateCounters(const TUpdateCountersData & counters, const CDbCriteria & criteria) throw (CDbException)
{
	Cws::trace(getClassName() + ".updateCounters()", "system.db.ar.CActiveRecord");
	CDbCommandBuilder * builder = getCommandBuilder();
	CDbCommand command = builder->createUpdateCounterCommand(tableName(), counters, criteria);
	return command.execute();
}

bool CActiveRecord::removeByPk(CTablePrimaryKey & pk) throw (CDbException)
{
	Cws::trace(getClassName() + ".deleteByPk()", "system.db.ar.CActiveRecord");
	CDbCommandBuilder * builder = getCommandBuilder();
	string table = tableName();
	CDbCriteria criteria = builder->createPkCriteria(table, pk);
	CDbCommand command = builder->createDeleteCommand(table, criteria);
	return command.execute() > 0;
}

unsigned long CActiveRecord::removeAll(const CDbCriteria & criteria) throw (CDbException)
{
	Cws::trace(getClassName() + ".deleteAll()", "system.db.ar.CActiveRecord");
	CDbCommandBuilder * builder = getCommandBuilder();
	CDbCommand command = builder->createDeleteCommand(tableName(), criteria);
	return command.execute();
}

CTablePrimaryKey CActiveRecord::getPrimaryKey() const
{
	return _pk;
}

void CActiveRecord::setPrimaryKey(const CTablePrimaryKey & value)
{
	_pk = value;
}

string CActiveRecord::getTableAlias(bool quote, bool checkScopes)
{
	string alias;
	if (checkScopes) {
		CDbCriteria criteria = getDbCriteria();
		if (!criteria.alias.empty()) {
			alias = criteria.alias;
		}
	} else {
		alias = _alias;
	}
	return quote ? getDbConnection()->getSchema().quoteTableName(alias) : alias;
}

void CActiveRecord::setTableAlias(const string & alias)
{
	_alias = alias;
}

CDbCriteria & CActiveRecord::getDbCriteria()
{
	return _criteria;
}

bool CActiveRecord::isAttributeNeeded(const string & name, const vector<string> & names) const
{
	return names.empty() || std::find(names.begin(), names.end(), name) != names.end();
}
