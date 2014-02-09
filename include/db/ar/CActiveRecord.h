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

/**
 * Registering declaration of model(), instantiate methods
 */
#define DECLARE_AR_CLASS(className) \
private:\
	static boost::shared_ptr<className> _instance;\
public:\
	static className * model();\
	virtual CActiveRecord * instantiate(const TDbRow & attributes) const;

/**
 * Registering implementation of model(), instantiate methods
 */
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
}\
CActiveRecord * className::instantiate(const TDbRow & attributes) const\
{\
	return new className("", getDbConnection());\
}

/**
 * Common attributes conversions
 */
#define AR_CONVERISION_LONG ->asLong()
#define AR_CONVERISION_ULONG ->asULong()
#ifdef _UNICODE
#define AR_CONVERISION_STRING ->asString().GetWideChars()
#else
#define AR_CONVERISION_STRING ->asString().GetMultiByteChars()
#endif
#define AR_CONVERISION_DOUBLE ->asDouble()

/**
 * Registering declaration of populateProperty method
 */
#define DECLARE_ATTRIBUTE_RESOLVER() \
public:\
	virtual void populateProperty(const string & name, const SAField * value);\

/**
 * Registering implementation begin of populateProperty method
 */
#define BEGIN_ATTRIBUTE_RESOLVER(className) \
	void className::populateProperty(const string & name, const SAField * value)\
	{\

/**
 * Registering part of populateProperty method implementation which containts
 * convertion logic of database attribute value to AR property
 */
#define ATTRIBUTE_RESOLVE(attributeName, property, conversion) \
	if (name == attributeName) {\
		this->property = value conversion;\
		return;\
	}

/**
 * Registering implementation end of populateProperty method
 */
#define END_ATTRIBUTE_RESOLVER() \
	}

/**
 * Registering active record class property
 */
#define DECLATE_AR_ATTRIBUTE(type, name) \
	public:\
		type name;

/**
 * Common attributes types
 */
#define AR_ATTRIBUTE_LONG long
#define AR_ATTRIBUTE_ULONG unsigned long
#define AR_ATTRIBUTE_STRING _string
#define AR_ATTRIBUTE_DOUBLE double

#endif /* CACTIVERECORD_H_ */