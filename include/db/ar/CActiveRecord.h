/*
 * CActiveRecord.h
 *
 *  Created on: Feb 5, 2014
 *      Author: djvibegga
 */

#ifndef CACTIVERECORD_H_
#define CACTIVERECORD_H_

#include "base/CComponent.h"
#include "db/CDbConnection.h"
#include "db/CDbCriteria.h"
#include "db/CDbDataReader.h"

class CActiveRecord;

typedef boost::shared_ptr<CActiveRecord> TActiveRecordPtr;
typedef vector<TActiveRecordPtr> TActiveRecordList;

class CActiveRecord: public CComponent
{
private:
	CDbConnection * _connection;
	string _scenario;

public:
	static const string LOG_CATEGORY;
	static const string SCENARIO_CREATE;
	static const string SCENARIO_UPDATE;

	CActiveRecord(const string & scenario = "", CDbConnection * connection = 0);
	virtual ~CActiveRecord();
	void init();
	virtual CActiveRecord * instantiate(const TDbRow & attributes) const = 0;
	virtual string tableName() const = 0;
	virtual void populateProperty(const string & name, const SAField * value) = 0;
	void setDbConnection(CDbConnection * connection);
	CDbConnection * getDbConnection() const;
	TActiveRecordList findAll(const CDbCriteria & criteria);
	TActiveRecordPtr find(const CDbCriteria & criteria);
	TActiveRecordList populateRecords(CDbDataReader & data, bool callAfterFind = true);
	TActiveRecordPtr populateRecord(const TDbRow & attributes, bool callAfterFind = true);
	void setScenario(const string & scenario);
	string getScenario() const;

protected:
	TActiveRecordList query(const CDbCriteria & criteria, bool all = false);
};

#define DECLARE_AR_CLASS(className) \
private:\
	static boost::shared_ptr<className> _instance;\
public:\
	static className * model();


#define IMPLEMENT_AR_CLASS(className) \
boost::shared_ptr<className> className::_instance;\
className * className::model()\
{\
	if (_instance.get() == 0) {\
		className * instance = new className();\
		instance->init();\
		_instance = boost::shared_ptr<className>(instance);\
	}\
	return _instance.get();\
}

#endif /* CACTIVERECORD_H_ */
