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

#include <boost/shared_ptr.hpp>

class CDbDataReader;

typedef const void* TDbValue;
typedef map<string, const SAField*> TDbRow;

enum ESQLType
{
	SQL_STRING, SQL_INT, SQL_UNSIGNED_INT, SQL_DOUBLE
};

class CDbCommandParameter
{
public:
	virtual ~CDbCommandParameter() {}
	virtual string dump() const = 0;
	virtual void bind(SACommand * command) const = 0;
};

typedef boost::shared_ptr<CDbCommandParameter> TDbCommandParameterPtr;

class CDbCommandParameterValue: public CDbCommandParameter
{
private:
	friend class CDbDataReader;

public:
	string name;
	ESQLType type;
	TDbValue value;

	CDbCommandParameterValue();
	CDbCommandParameterValue(const string & name, ESQLType type, TDbValue value);
	virtual string dump() const;
	virtual void bind(SACommand * command) const;
};

class CCommandParameterMap: public map<string, TDbCommandParameterPtr>
{
public:
	void set(const string & name, const char * value);
	void set(const string & name, const wchar_t * value);
	void set(const string & name, const string & value);
	void set(const string & name, const wstring & value);
	void set(const string & name, const long & value);
	void set(const string & name, const double & value);
	void set(const string & name, const unsigned long & value);
	void set(const string & name, const TDbCommandParameterPtr & value);
	CCommandParameterMap & mergeWith(const CCommandParameterMap & other);
};

class CDbCommandParameterExpression: public CDbCommandParameter
{
public:
	string expression;
	CCommandParameterMap params;

	CDbCommandParameterExpression(const string & expression);
	CDbCommandParameterExpression(const string & expression, const CCommandParameterMap & params);
	virtual string dump() const;
	virtual void bind(SACommand * command) const;
};

class CDbCommand: public CComponent
{
private:
	const CDbConnection * _connection;
	string _text;
	CCommandParameterMap _params;
	SACommand * _saCommand;

	string _makeParametersDump(const CCommandParameterMap & params);
	CDbDataReader _queryInternal(const CCommandParameterMap & params) throw (CDbException);

public:
	CDbCommand(const CDbConnection * const connection);
	CDbCommand(const CDbConnection * const connection, const string & query);
	virtual string getClassName() const;
	virtual ~CDbCommand();
	void setText(const string & query);
	string getText() const;
    const CDbConnection * getConnection() const;
    CDbCommand & bindParam(const string & name, const char * value);
    CDbCommand & bindParam(const string & name, const string & value);
    CDbCommand & bindParam(const string & name, const long & value);
    CDbCommand & bindParam(const string & name, const double & value);
    CDbCommand & bindParam(const string & name, const unsigned long & value);
    CDbCommand & bindParam(const string & name, const TDbCommandParameterPtr & value);
    CDbCommand & mergeParametersWith(const CCommandParameterMap & other);
    long unsigned int execute() throw (CDbException);
    long unsigned int execute(const CCommandParameterMap & params) throw (CDbException);
    CDbDataReader queryAll() throw (CDbException);
    CDbDataReader queryAll(const CCommandParameterMap & params) throw (CDbException);
    TDbRow queryRow() throw (CDbException);
    TDbRow queryRow(const CCommandParameterMap & params) throw (CDbException);
    SAField & queryScalar() throw (CDbException);
    SAField & queryScalar(const CCommandParameterMap & params) throw (CDbException);
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
