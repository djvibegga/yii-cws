/*
 * CDbCommand.cpp
 *
 *  Created on: Oct 25, 2013
 *      Author: djvibegga
 */

#include "db/CDbCommand.h"
#include "base/CStringUtils.h"
#include "db/CDbDataReader.h"
#include "base/Jvibetto.h"
#include "base/CProfiler.h"

CDbCommandParameter::CDbCommandParameter()
 : type(SQL_STRING),
   value(0)
{
}

CDbCommandParameter::CDbCommandParameter(const string & name, ESQLType type, TDbValue value)
: name(name),
  type(type),
  value(value)
{
}

string CDbCommandParameter::dump() const
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

void CDbCommandParameter::bind(SACommand * command) const
{
	switch (type) {
	case SQL_STRING:
		command->Param(name.c_str()).setAsString() = (const char*)value;
		break;
	case SQL_INT:
		command->Param(name.c_str()).setAsLong() = *((long*)value);
		break;
	case SQL_UNSIGNED_INT:
		command->Param(name.c_str()).setAsULong() = *((unsigned long*)value);
		break;
	case SQL_DOUBLE:
		command->Param(name.c_str()).setAsDouble() = *((double*)value);
		break;
	}
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
	_params[name] = CDbCommandParameter(name, SQL_STRING, (TDbValue)value);
	return *this;
}

CDbCommand & CDbCommand::bindParam(const string & name, const string & value)
{
	_params[name] = CDbCommandParameter(name, SQL_STRING, (TDbValue)value.c_str());
	return *this;
}

CDbCommand & CDbCommand::bindParam(const string & name, const long & value)
{
	_params[name] = CDbCommandParameter(name, SQL_INT, (TDbValue)&value);
	return *this;
}

CDbCommand & CDbCommand::bindParam(const string & name, const unsigned long &value)
{
	_params[name] = CDbCommandParameter(name, SQL_UNSIGNED_INT, (TDbValue)&value);
	return *this;
}

CDbCommand & CDbCommand::bindParam(const string & name, const CDbCommandParameter & value)
{
	_params[name] = value;
	return *this;
}

CDbCommand & CDbCommand::bindParam(const string & name, const double & value)
{
	_params[name] = CDbCommandParameter(name, SQL_DOUBLE, (TDbValue)&value);
	return *this;
}

CDbCommand & CDbCommand::mergeParametersWith(const TCommandParameterMap & other)
{
	for (TCommandParameterMap::const_iterator iter = other.begin(); iter != other.end(); ++iter) {
		_params[iter->first] = iter->second;
	}
	return *this;
}

string CDbCommand::_makeParametersDump(const TCommandParameterMap & params)
{
#ifdef JV_DB_PARAM_LOGGING
	string ret;
	if (!params.empty()) {
		vector<string> parametersList;
		for (TCommandParameterMap::const_iterator iter = params.begin(); iter != params.end(); ++iter) {
			parametersList.push_back(iter->first + "=" + iter->second.dump());
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
	return execute(TCommandParameterMap());
}

long unsigned int CDbCommand::execute(const TCommandParameterMap & params) throw (CDbException)
{
	mergeParametersWith(params);
	string parametersDump = _makeParametersDump(_params);
#ifdef JV_DEBUG
	Jvibetto::trace("Executing SQL: " + _text + parametersDump, "system.db.CDbCommand");
#endif
	try {
		SAConnection * connection = _connection->getConnection();
		_saCommand = new SACommand(connection);
		_saCommand->setCommandText(_text.c_str());
		for (TCommandParameterMap::const_iterator iter = _params.begin(); iter != _params.end(); ++iter) {
			iter->second.bind(_saCommand);
		}
		PROFILE_BEGIN("Executing SQL: " + _text + parametersDump);
		_saCommand->Execute();
		connection->Commit();
		PROFILE_END();
		return _saCommand->RowsAffected();

	} catch (const SAException & e) {

		string message = e.ErrText().GetMultiByteChars();
		Jvibetto::log(
			"CDbCommand::execute() failed: " + message
			+ ". The SQL statement executed was: " + _text + "."
		);
#ifdef JV_DEBUG
		message += ". The SQL statement executed was: " + _text + parametersDump;
#endif
		throw CDbException(
			"CDbCommand failed to execute the SQL statement: " + message,
			(int)e.ErrNativeCode()
		);
	}
	return 0;
}

CDbDataReader CDbCommand::_queryInternal(const TCommandParameterMap & params) throw (CDbException)
{
	mergeParametersWith(params);
	string parametersDump = _makeParametersDump(_params);
#ifdef JV_DEBUG
	Jvibetto::trace("Executing SQL: " + _text + parametersDump, "system.db.CDbCommand");
#endif

	try {
		SAConnection * connection = _connection->getConnection();
		_saCommand = new SACommand(connection, _text.c_str(), SA_CmdSQLStmt);
		if (connection->Client() == SA_MySQL_Client) {
			_saCommand->setOption("UseStatement") = "TRUE";
		}
		for (TCommandParameterMap::const_iterator iter = _params.begin(); iter != _params.end(); ++iter) {
			iter->second.bind(_saCommand);
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
		Jvibetto::log(
			"CDbCommand::_queryInternal() failed: " + ss.str()
			+ ". The SQL statement executed was: " + _text + "."
		);
#ifdef JV_DEBUG
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
	return queryAll(TCommandParameterMap());
}

CDbDataReader CDbCommand::queryAll(const TCommandParameterMap & params) throw (CDbException)
{
	return _queryInternal(params);
}

TDbRow CDbCommand::queryRow() throw (CDbException)
{
	return queryRow(TCommandParameterMap());
}

TDbRow CDbCommand::queryRow(const TCommandParameterMap & params) throw (CDbException)
{
	CDbDataReader reader = _queryInternal(params);
	reader.nextResult();
	return reader.readRow();
}

SAField & CDbCommand::queryScalar() throw (CDbException)
{
	return queryScalar(TCommandParameterMap());
}

SAField & CDbCommand::queryScalar(const TCommandParameterMap & params) throw (CDbException)
{
	CDbDataReader reader = _queryInternal(params);
	reader.nextResult();
	return reader.readColumn(0);
}
