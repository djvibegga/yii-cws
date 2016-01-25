/*
 * CActiveRecord.h
 *
 *  Created on: Feb 5, 2014
 *      Author: djvibegga
 */

#ifndef CACTIVERECORD_H_
#define CACTIVERECORD_H_

#include "base/CComponent.h"
#include "base/CApplication.h"
#include "db/CDbConnection.h"
#include "db/CDbCriteria.h"
#include "db/CDbDataReader.h"
#include "db/schema/CDbCommandBuilder.h"

class CActiveRecord;

typedef boost::shared_ptr<CActiveRecord> TActiveRecordPtr;
typedef vector<TActiveRecordPtr> TActiveRecordList;

class CAttributeCollection
{
private:
	map<string, string> _strValues;
	map<string, wstring> _wstrValues;
	map<string, unsigned long> _ulongValues;
	map<string, long> _longValues;
	map<string, double> _doubleValues;

public:
	void setValue(const string & key, const string & value);
	void setValue(const string & key, const wstring & value);
	void setValue(const string & key, unsigned long value);
	void setValue(const string & key, long value);
	void setValue(const string & key, double value);

	bool has(const string & key) const;
	string getStringValue(const string & key) const;
	wstring getWStringValue(const string & key) const;
	unsigned long getULongValue(const string & key) const;
	long getLongValue(const string & key) const;
	double getDoubleValue(const string & key) const;
	CCommandParameterMap convertToCommandParameterMap(const vector<string> & ignoredNames = vector<string>()) const;
};

class CActiveRecord: public CComponent
{
private:
	CDbConnection * _connection;
	string _scenario;
	bool _isNew;
	CTablePrimaryKey _pk;
	string _alias;
	CDbCriteria _criteria;

public:
	static const string LOG_CATEGORY;
	static const string SCENARIO_CREATE;
	static const string SCENARIO_UPDATE;

	CActiveRecord(const string & scenario = "", CDbConnection * connection = 0);
	virtual ~CActiveRecord();
	void init();
	virtual CActiveRecord * instantiate(const TDbRow & attributes) const = 0;
	virtual string tableName() const = 0;
	virtual bool validate(const vector<string> & attributeNames = vector<string>());
	virtual void populateProperty(const string & name, const SAField * value) = 0;
	void setDbConnection(CDbConnection * connection);
	CDbConnection * getDbConnection() const;
	CDbCommandBuilder * getCommandBuilder() const;
	CDbTableSchema getTableSchema() const;
	TActiveRecordList findAll() throw (CDbException);
	TActiveRecordList findAll(const CDbCriteria & criteria) throw (CDbException);
	TActiveRecordPtr find(const CDbCriteria & criteria) throw (CDbException);
	TActiveRecordPtr findByPk(CTablePrimaryKey & pk) throw (CDbException);
	TActiveRecordList populateRecords(CDbDataReader & data, bool callAfterFind = true);
	TActiveRecordPtr populateRecord(const TDbRow & attributes, bool callAfterFind = true);
	void setScenario(const string & scenario);
	string getScenario() const;
	bool getIsNewRecord() const;
	void setIsNewRecord(bool value);
	bool save(bool runValidation = true) throw (CDbException);
	bool save(bool runValidation, const vector<string> & attributes) throw (CDbException);
	bool saveAttributes(const vector<string> & attributes) throw (CDbException);
	bool insert(const vector<string> & attributes) throw (CDbException);
	bool update(const vector<string> & attributes) throw (CDbException);
	bool saveCounters(const TUpdateCountersData & counters) throw (CDbException);
	bool remove() throw (CDbException);
	bool refresh() throw (CDbException);
	unsigned long count() throw (CDbException);
	unsigned long count(const CDbCriteria & criteria) throw (CDbException);
	bool exists() throw (CDbException);
	bool exists(CDbCriteria & criteria) throw (CDbException);
	bool updateByPk(CTablePrimaryKey & pk, const CCommandParameterMap & attributes) throw (CDbException);
	unsigned long updateAll(const CCommandParameterMap & attributes, const CDbCriteria & criteria) throw (CDbException);
	unsigned long updateCounters(const TUpdateCountersData & counters, const CDbCriteria & criteria) throw (CDbException);
	bool removeByPk(CTablePrimaryKey & pk) throw (CDbException);
	unsigned long removeAll(const CDbCriteria & criteria) throw (CDbException);
	CTablePrimaryKey getPrimaryKey() const;
	void setPrimaryKey(const CTablePrimaryKey & value);
	string getTableAlias(bool quote = false, bool checkScopes = true);
	void setTableAlias(const string & alias);
	CDbCriteria & getDbCriteria();

	virtual void beforeFind();
	virtual void afterFind();

	virtual bool isAttributeNeeded(const string & name, const vector<string> & names) const;
	virtual CAttributeCollection getAttributes(const vector<string> & names = vector<string>()) const = 0;
	virtual void setAttributes(const CAttributeCollection & attributes) = 0;

protected:
	TActiveRecordList query(const CDbCriteria & criteria, bool all = false) throw (CDbException);
	void onBeforeSave(CEvent & event);
	void onAfterSave(CEvent & event);
	void onBeforeDelete(CEvent & event);
	void onAfterDelete(CEvent & event);
	void onBeforeFind(CEvent & event);
	void onAfterFind(CEvent & event);
	void onBeforeCount(CEvent & event);

	virtual bool beforeSave();
	virtual void afterSave();
	virtual bool beforeDelete();
	virtual void afterDelete();
	virtual void beforeCount();

	virtual CTablePrimaryKey fetchPrimaryKey() const = 0;
};

typedef boost::shared_ptr<CActiveRecord> TArSharedPtr;

/**
 * Registering declaration of model(), instantiate methods
 */
#define DECLARE_AR_CLASS(className) \
private:\
	static map<long, TArSharedPtr> _instances;\
	static boost::mutex _instancesMutex;\
public:\
	static className * model();\
	virtual string getClassName() const;\
	virtual string tableName() const;\
	virtual CActiveRecord * instantiate(const TDbRow & attributes) const;

/**
 * Registering implementation of model(), instantiate methods
 */
#define IMPLEMENT_AR_CLASS(className, tblName) \
map<long, TArSharedPtr> className::_instances;\
boost::mutex className::_instancesMutex;\
string className::getClassName() const\
{\
	return #className;\
}\
string className::tableName() const\
{\
	return tblName;\
}\
className * className::model()\
{\
	long threadId = CApplication::getThreadId();\
	boost::lock_guard<boost::mutex> guard(_instancesMutex);\
	map<long, TArSharedPtr>::const_iterator found = _instances.find(threadId);\
	if (found == _instances.end()) {\
		className * instance = new className();\
		instance->setIsNewRecord(false);\
		instance->init();\
		_instances[threadId] = TArSharedPtr(instance);\
		return instance;\
	} else {\
		return dynamic_cast<className*>(found->second.get());\
	}\
}\
CActiveRecord * className::instantiate(const TDbRow & attributes) const\
{\
	return new className("", getDbConnection());\
}

/**
 * Common attributes conversions
 */
#define AR_POPULATE_LONG ->asLong()
#define AR_POPULATE_ULONG ->asULong()
#define AR_POPULATE_STRING ->asString().GetMultiByteChars()
#ifdef _UNICODE
#define AR_POPULATE_VSTRING ->asString().GetWideChars()
#else
#define AR_POPULATE_VSTRING ->asString().GetMultiByteChars()
#endif
#define AR_POPULATE_DOUBLE ->asDouble()

/**
 * Registering declaration of populateProperty method
 */
#define DECLARE_ATTRIBUTE_POPULATE() \
public:\
	virtual void populateProperty(const string & name, const SAField * value);

/**
 * Registering implementation begin of populateProperty method
 */
#define BEGIN_ATTRIBUTE_POPULATE(className) \
	void className::populateProperty(const string & name, const SAField * value)\
	{

/**
 * Registering part of populateProperty method implementation which containts
 * convertion logic of database attribute value to AR property
 */
#define ATTRIBUTE_POPULATE(property, attributeName, conversion) \
	if (name == attributeName) {\
		this->property = value conversion;\
		return;\
	}

/**
 * Registering implementation end of populateProperty method
 */
#define END_ATTRIBUTE_POPULATE() \
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
#define AR_ATTRIBUTE_STRING string
#define AR_ATTRIBUTE_VSTRING _string
#define AR_ATTRIBUTE_DOUBLE double

#define DECLARE_ATTRIBUTES_GETSET()\
public:\
	virtual CAttributeCollection getAttributes(const vector<string> & names) const;\
	virtual void setAttributes(const CAttributeCollection & attributes);

#define BEGIN_ATTRIBUTES_SETTER(className)\
void className::setAttributes(const CAttributeCollection & attributes)\
{\

#define ATTRIBUTES_SETTER(property, attribute, type)\
	if (attributes.has(attribute)) {\
		this->property = attributes. type (attribute);\
	}

#define END_ATTRIBUTES_SETTER()\
}

#define BEGIN_ATTRIBUTES_GETTER(className)\
CAttributeCollection className::getAttributes(const vector<string> & names) const\
{\
	CAttributeCollection ret;

#define ATTRIBUTES_GETTER(attribute, property)\
	if (this->isAttributeNeeded(attribute, names)) {\
		ret.setValue(attribute, this->property);\
	}

#define END_ATTRIBUTES_GETTER()\
	return ret;\
}
/**
 * Common attributes set-get types
 */
#define AR_ATTRIBUTE_GET_LONG getLongValue
#define AR_ATTRIBUTE_GET_ULONG getULongValue
#define AR_ATTRIBUTE_GET_STRING getStringValue
#ifdef _UNICODE
#define AR_ATTRIBUTE_GET_VSTRING getWStringValue
#else
#define AR_ATTRIBUTE_GET_VSTRING getStringValue
#endif
#define AR_ATTRIBUTE_GET_DOUBLE getDoubleValue

/**
 * Registering declaration of "fetchPrimaryKey" method
 */
#define DECLARE_PRIMARY_KEY() \
protected:\
	CTablePrimaryKey fetchPrimaryKey() const;\

/**
 * Registering implementation begin of "fetchPrimaryKey" method
 */
#define BEGIN_PRIMARY_KEY(className) \
	CTablePrimaryKey className::fetchPrimaryKey() const\
	{\
		CTablePrimaryKey key;\

/**
 * Fetch a property value into primary key structure.
 */
#define PRIMARY_KEY_ADD_PROPERTY(attribute, property) \
	key.setValue(attribute, this->property);\

/**
 * Registering implementation end of "fetchPrimaryKey" method
 */
#define END_PRIMARY_KEY() \
		return key;\
	}

#endif /* CACTIVERECORD_H_ */
