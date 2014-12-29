/*
 * CDbCommand.h
 *
 *  Created on: Oct 25, 2013
 *      Author: djvibegga
 */

#ifndef CDBCOMMAND_H_
#define CDBCOMMAND_H_

#include "db/CDbConnection.h"
#include "db/CDbException.h"

class CDbDataReader;

typedef const void* TDbValue;
typedef map<string, const SAField*> TDbRow;

enum ESQLType
{
	SQL_STRING, SQL_INT, SQL_UNSIGNED_INT, SQL_DOUBLE
};

class CDbCommandParameter
{
	friend class CDbDataReader;

public:
	string name;
	ESQLType type;
	TDbValue value;
	CDbCommandParameter();
	CDbCommandParameter(const string & name, ESQLType type, TDbValue value);
	string dump() const;
	void bind(SACommand * command) const;
};

typedef map<string, CDbCommandParameter> TCommandParameterMap;

class CDbCommand: public CComponent
{
private:
	const CDbConnection * _connection;
	string _text;
	TCommandParameterMap _params;
	SACommand * _saCommand;

	string _makeParametersDump(const TCommandParameterMap & params);
	CDbDataReader _queryInternal(const TCommandParameterMap & params) throw (CDbException);

public:
	CDbCommand(const CDbConnection * const connection);
	CDbCommand(const CDbConnection * const connection, const string & query);
	virtual ~CDbCommand();
	void setText(const string & query);
	string getText() const;
    const CDbConnection * getConnection() const;
    CDbCommand & bindParam(const string & name, const char * value);
    CDbCommand & bindParam(const string & name, const string & value);
    CDbCommand & bindParam(const string & name, const long & value);
    CDbCommand & bindParam(const string & name, const double & value);
    CDbCommand & bindParam(const string & name, const unsigned long & value);
    CDbCommand & bindParam(const string & name, const CDbCommandParameter & value);
    CDbCommand & mergeParametersWith(const TCommandParameterMap & other);
    long unsigned int execute() throw (CDbException);
    long unsigned int execute(const TCommandParameterMap & params) throw (CDbException);
    CDbDataReader queryAll() throw (CDbException);
    CDbDataReader queryAll(const TCommandParameterMap & params) throw (CDbException);
    TDbRow queryRow() throw (CDbException);
    TDbRow queryRow(const TCommandParameterMap & params) throw (CDbException);
    SAField & queryScalar() throw (CDbException);
    SAField & queryScalar(const TCommandParameterMap & params) throw (CDbException);
};

/**
 * Data column attributes conversions
 */
#define COLUMN_TO_LONG(value) value->asLong()
#define COLUMN_TO_ULONG(value) value->asULong()
#define COLUMN_TO_STRING(value) value->asString().GetMultiByteChars()
#ifdef _UNICODE
#define COLUMN_TO_VSTRING(value) value->asString().GetWideChars()
#else
#define COLUMN_TO_VSTRING(value) value->asString().GetMultiByteChars()
#endif
#define COLUMN_TO_DOUBLE(value) value->asDouble()

#endif /* CDBCOMMAND_H_ */
