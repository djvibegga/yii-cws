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

class CDbConnection;
class CDbDataReader;

typedef void* TDbParameter;

enum ECommandParameter
{
	PARAM_STRING, PARAM_INT, PARAM_DOUBLE, PARAM_FLOAT
};

class CDbCommandParameter
{
public:
	ECommandParameter type;
	TDbParameter value;
	CDbCommandParameter();
	CDbCommandParameter(ECommandParameter type, TDbParameter value);
	string dump() const;
	MYSQL_BIND * constructBind() const;
};

typedef map<string, CDbCommandParameter> TCommandParameterMap;

class CDbCommand: public CComponent
{
private:
	const CDbConnection * _connection;
	string _text;
	TCommandParameterMap _params;
	MYSQL_STMT * _statement;
	vector<MYSQL_BIND*> _binds;

	CDbDataReader _queryInternal(const string & method, const TCommandParameterMap & params);

public:
	CDbCommand(const CDbConnection * const connection);
	CDbCommand(const CDbConnection * const connection, const string & query);
	virtual ~CDbCommand();
	void setText(const string & query);
	string getText() const;
	MYSQL_STMT * getStatement() const;
    const CDbConnection * getConnection() const;
    CDbCommand & bindParam(const string & name, const string & value);
    CDbCommand & bindParam(const string & name, const int value);
    CDbCommand & bindParam(const string & name, const float & value);
    CDbCommand & bindParam(const string & name, const double & value);
    CDbCommand & mergeParametersWith(const TCommandParameterMap & other);
    int execute() throw (CDbException);
    int execute(const TCommandParameterMap & params) throw (CDbException);
};

#endif /* CDBCOMMAND_H_ */
