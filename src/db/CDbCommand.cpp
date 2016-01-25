/*
 * CDbCommand.cpp
 *
 *  Created on: Oct 25, 2013
 *      Author: djvibegga
 */

#include "db/CDbCommand.h"
#include "base/CStringUtils.h"
#include "db/CDbDataReader.h"
#include "base/Cws.h"
#include "base/CProfiler.h"

CDbCommandParameterValue::CDbCommandParameterValue()
 : type(SQL_STRING),
   value(0)
{
}

CDbCommandParameterValue::CDbCommandParameterValue(const string & name, ESQLType type, TDbValue value)
: name(name),
  type(type),
  value(value)
{
}

string CDbCommandParameterValue::dump() const
{
	stringstream ss;
	switch (type) {
	case SQL_STRING:
		ss << (const char*)value;
		break;
	case SQL_DOUBLE:
		ss << *((double*)value);
		break;
	case SQL_INT:
		ss << *((long*)value);
		break;
	case SQL_UNSIGNED_INT:
			ss << *((unsigned long*)value);
		break;
	default:
		throw CException("Invalid data type. Can\'t dump db command parameter.");
	}
	return ss.str();
}

void CDbCommandParameterValue::bind(SACommand * command) const
{
	string paramName = name[0] == ':' ? name.substr(1) : name;
	switch (type) {
	case SQL_STRING:
		command->Param(paramName.c_str()).setAsString() = (const char*)value;
		break;
	case SQL_INT:
		command->Param(paramName.c_str()).setAsLong() = *((long*)value);
		break;
	case SQL_UNSIGNED_INT:
		command->Param(paramName.c_str()).setAsULong() = *((unsigned long*)value);
		break;
	case SQL_DOUBLE:
		command->Param(paramName.c_str()).setAsDouble() = *((double*)value);
		break;
	}
}

void CCommandParameterMap::set(const string & name, const char * value)
{
	this->operator [](name) = TDbCommandParameterPtr(new CDbCommandParameterValue(name, SQL_STRING, (TDbValue)value));
}

void CCommandParameterMap::set(const string & name, const wchar_t * value)
{
	this->operator [](name) = TDbCommandParameterPtr(new CDbCommandParameterValue(name, SQL_STRING, (TDbValue)value));
}

void CCommandParameterMap::set(const string & name, const string & value)
{
	this->operator [](name) = TDbCommandParameterPtr(new CDbCommandParameterValue(name, SQL_STRING, (TDbValue)value.c_str()));
}

void CCommandParameterMap::set(const string & name, const wstring & value)
{
	this->operator [](name) = TDbCommandParameterPtr(new CDbCommandParameterValue(name, SQL_STRING, (TDbValue)value.c_str()));
}

void CCommandParameterMap::set(const string & name, const long & value)
{
	this->operator [](name) = TDbCommandParameterPtr(new CDbCommandParameterValue(name, SQL_INT, (TDbValue)&value));
}

void CCommandParameterMap::set(const string & name, const double & value)
{
	this->operator [](name) = TDbCommandParameterPtr(new CDbCommandParameterValue(name, SQL_DOUBLE, (TDbValue)&value));
}

void CCommandParameterMap::set(const string & name, const unsigned long & value)
{
	this->operator [](name) = TDbCommandParameterPtr(new CDbCommandParameterValue(name, SQL_UNSIGNED_INT, (TDbValue)&value));
}

void CCommandParameterMap::set(const string & name, const TDbCommandParameterPtr & value)
{
	this->operator [](name) = value;
}

CCommandParameterMap & CCommandParameterMap::mergeWith(const CCommandParameterMap & other)
{
	for (CCommandParameterMap::const_iterator iter = other.begin(); iter != other.end(); ++iter) {
		this->operator [](iter->first) = iter->second;
	}
	return *this;
}

CDbCommandParameterExpression::CDbCommandParameterExpression(const string & expression)
: expression(expression)
{
}

CDbCommandParameterExpression::CDbCommandParameterExpression(const string & expression, const CCommandParameterMap & params)
: expression(expression),
  params(params)
{
}

string CDbCommandParameterExpression::dump() const
{
	return expression;
}

void CDbCommandParameterExpression::bind(SACommand * command) const
{
}


CDbCommand::CDbCommand(const CDbConnection * const connection, const string & query)
: _connection(connection),
  _text(query),
  _saCommand(0)
{
}

CDbCommand::CDbCommand(const CDbConnection * const connection)
: _connection(connection),
  _text(""),
  _saCommand(0)
{
}

string CDbCommand::getClassName() const
{
	return "CDbCommand";
}

CDbCommand::~CDbCommand()
{
	if (_saCommand != 0) {
		delete _saCommand;
	}
}

void CDbCommand::setText(const string & query)
{
	_text = query;
}

string CDbCommand::getText() const
{
	return _text;
}

const CDbConnection * CDbCommand::getConnection() const
{
	return _connection;
}

CDbCommand & CDbCommand::bindParam(const string & name, const char * value)
{
	_params[name] = TDbCommandParameterPtr(new CDbCommandParameterValue(name, SQL_STRING, (TDbValue)value));
	return *this;
}

CDbCommand & CDbCommand::bindParam(const string & name, const string & value)
{
	_params[name] = TDbCommandParameterPtr(new CDbCommandParameterValue(name, SQL_STRING, (TDbValue)value.c_str()));
	return *this;
}

CDbCommand & CDbCommand::bindParam(const string & name, const long & value)
{
	_params[name] = TDbCommandParameterPtr(new CDbCommandParameterValue(name, SQL_INT, (TDbValue)&value));
	return *this;
}

CDbCommand & CDbCommand::bindParam(const string & name, const unsigned long &value)
{
	_params[name] = TDbCommandParameterPtr(new CDbCommandParameterValue(name, SQL_UNSIGNED_INT, (TDbValue)&value));
	return *this;
}

CDbCommand & CDbCommand::bindParam(const string & name, const TDbCommandParameterPtr & value)
{
	_params[name] = value;
	return *this;
}

CDbCommand & CDbCommand::bindParam(const string & name, const double & value)
{
	_params[name] = TDbCommandParameterPtr(new CDbCommandParameterValue(name, SQL_DOUBLE, (TDbValue)&value));
	return *this;
}

CDbCommand & CDbCommand::mergeParametersWith(const CCommandParameterMap & other)
{
	for (CCommandParameterMap::const_iterator iter = other.begin(); iter != other.end(); ++iter) {
		_params[iter->first] = iter->second;
	}
	return *this;
}

string CDbCommand::_makeParametersDump(const CCommandParameterMap & params)
{
#ifdef CWS_DB_PARAM_LOGGING
	string ret;
	if (!params.empty()) {
		vector<string> parametersList;
		for (CCommandParameterMap::const_iterator iter = params.begin(); iter != params.end(); ++iter) {
			parametersList.push_back(iter->first + "=" + iter->second.get()->dump());
		}
		ret = ". Bound with " + CStringUtils::implode(", ", parametersList);
	}
	return ret;
#else
	return "";
#endif
}

long unsigned int CDbCommand::execute() throw (CDbException)
{
	return execute(CCommandParameterMap());
}

long unsigned int CDbCommand::execute(const CCommandParameterMap & params) throw (CDbException)
{
	mergeParametersWith(params);
	string parametersDump = _makeParametersDump(_params);
#ifdef CWS_DEBUG
	Cws::trace("Executing SQL: " + _text + parametersDump, "system.db.CDbCommand");
#endif
	try {
		SAConnection * connection = _connection->getConnection();
		_saCommand = new SACommand(connection);
		_saCommand->setCommandText(_text.c_str());
		for (CCommandParameterMap::const_iterator iter = _params.begin(); iter != _params.end(); ++iter) {
			iter->second.get()->bind(_saCommand);
		}
		PROFILE_BEGIN("Executing SQL: " + _text + parametersDump);
		_saCommand->Execute();
		connection->Commit();
		PROFILE_END();
		return _saCommand->RowsAffected();

	} catch (const SAException & e) {

		string message = e.ErrText().GetMultiByteChars();
		Cws::log(
			"CDbCommand::execute() failed: " + message
			+ ". The SQL statement executed was: " + _text + "."
		);
#ifdef CWS_DEBUG
		message += ". The SQL statement executed was: " + _text + parametersDump;
#endif
		throw CDbException(
			"CDbCommand failed to execute the SQL statement: " + message,
			(int)e.ErrNativeCode()
		);
	}
	return 0;
}

CDbDataReader CDbCommand::_queryInternal(const CCommandParameterMap & params) throw (CDbException)
{
	mergeParametersWith(params);
	string parametersDump = _makeParametersDump(_params);
#ifdef CWS_DEBUG
	Cws::trace("Executing SQL: " + _text + parametersDump, "system.db.CDbCommand");
#endif

	try {
		SAConnection * connection = _connection->getConnection();
		_saCommand = new SACommand(connection, _text.c_str(), SA_CmdSQLStmt);
		if (connection->Client() == SA_MySQL_Client) {
			_saCommand->setOption("UseStatement") = "TRUE";
		}
		for (CCommandParameterMap::const_iterator iter = _params.begin(); iter != _params.end(); ++iter) {
			iter->second.get()->bind(_saCommand);
		}
		PROFILE_BEGIN("Executing SQL: " + _text + parametersDump);
		_saCommand->Execute();
		PROFILE_END();
		return CDbDataReader(_saCommand);

	} catch (const SAException & e) {

		string message = e.ErrText().GetMultiByteChars();
		stringstream ss;
		ss << "CDbCommand::_queryInternal() failed: " << message
		   << ". Code: " << (int)e.ErrNativeCode();
		Cws::log(
			"CDbCommand::_queryInternal() failed: " + ss.str()
			+ ". The SQL statement executed was: " + _text + "."
		);
#ifdef CWS_DEBUG
		message += ". The SQL statement executed was: " + _text + parametersDump;
#endif
		throw CDbException(
			"CDbCommand failed to execute the SQL statement: " + message,
			(int)e.ErrNativeCode()
		);
	}
}

CDbDataReader CDbCommand::queryAll() throw (CDbException)
{
	return queryAll(CCommandParameterMap());
}

CDbDataReader CDbCommand::queryAll(const CCommandParameterMap & params) throw (CDbException)
{
	return _queryInternal(params);
}

TDbRow CDbCommand::queryRow() throw (CDbException)
{
	return queryRow(CCommandParameterMap());
}

TDbRow CDbCommand::queryRow(const CCommandParameterMap & params) throw (CDbException)
{
	CDbDataReader reader = _queryInternal(params);
	if (!reader.nextResult()) {
		return TDbRow();
	}
	return reader.readRow();
}

SAField & CDbCommand::queryScalar() throw (CDbException)
{
	return queryScalar(CCommandParameterMap());
}

SAField & CDbCommand::queryScalar(const CCommandParameterMap & params) throw (CDbException)
{
	CDbDataReader reader = _queryInternal(params);
	reader.nextResult();
	return reader.readColumn(1);
}
