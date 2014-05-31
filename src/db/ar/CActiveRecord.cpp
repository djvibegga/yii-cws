/*
 * CActiveRecord.cpp
 *
 *  Created on: Feb 5, 2014
 *      Author: djvibegga
 */

#include "db/ar/CActiveRecord.h"
#include "db/schema/CDbCommandBuilder.h"
#include "base/Jvibetto.h"

const string CActiveRecord::LOG_CATEGORY = "system.db.ar.CActiveRecord";
const string CActiveRecord::SCENARIO_CREATE = "create";
const string CActiveRecord::SCENARIO_UPDATE = "update";

CActiveRecord::CActiveRecord(const string & scenario, CDbConnection * connection)
: CComponent(),
  _connection(connection),
  _scenario(scenario)
{
}

CActiveRecord::~CActiveRecord()
{
}

void CActiveRecord::init()
{
	if (_connection == 0) {
		setDbConnection(dynamic_cast<CDbConnection*>(Jvibetto::app()->getComponent("db")));
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

TActiveRecordList CActiveRecord::findAll(const CDbCriteria & criteria) throw (CDbException)
{
#ifdef JV_DEBUG
	Jvibetto::trace(typeid(this).name() + string(".findAll()"), LOG_CATEGORY);
#endif
	return query(criteria, true);
}

TActiveRecordPtr CActiveRecord::find(const CDbCriteria & criteria) throw (CDbException)
{
	TActiveRecordList records = query(criteria, false);
	return records[0];
}

TActiveRecordList CActiveRecord::query(const CDbCriteria & criteria, bool all) throw (CDbException)
{
	CDbCommand command = _connection->getCommandBuilder()->createFindCommand(tableName(), criteria);
	if (all) {
		CDbDataReader reader = command.queryAll();
		return populateRecords(reader, true);
	}
	TActiveRecordList ret;
	ret.push_back(populateRecord(command.queryRow()));
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
	record->init();
	for (TDbRow::const_iterator iter = attributes.begin(); iter != attributes.end(); ++iter) {
		record->populateProperty(iter->first, iter->second);
	}
	//$record->_pk=$record->getPrimaryKey();
	//$record->attachBehaviors($record->behaviors());
	if (callAfterFind) {
		//record->afterFind();
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
