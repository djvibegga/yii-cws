#include "db/CDbCommand.h"
#include "db/schema/CDbCommandBuilder.h"
#include "db/CDbConnection.h"
#include <boost/regex.hpp>

CDbConnection::CDbConnection(const CModule * module)
: CApplicationComponent(module),
  _connection(0),
  _isActive(false),
  _commandBuilder(0),
  connectionString(""),
  username(""),
  password(""),
  enableParamLogging(false)
{
}

CDbConnection::CDbConnection(const CModule * module, string dsn, string username, string password)
: CApplicationComponent(module),
  _isActive(false),
  _commandBuilder(0),
  enableParamLogging(false)
{
	this->_connection = 0;
	connectionString = dsn;
	this->username = username;
	this->password = password;
}

bool CDbConnection::open() throw(CException)
{
	_connection = mysql_init(NULL);
	boost::regex pattern("(\\w+):host=([\\d\\.]+);dbname=(\\w+)");
	boost::smatch match;

	string driver, host, dbname = "";
	if (!boost::regex_match(connectionString, match, pattern)) {
		string message = "Invalid connection string passed. ";
		message.append(connectionString);
		throw CException(message, 0, __FILE__, __LINE__);
	} else {
		driver = match[1];
		host = match[2];
		dbname = match[3];
	}
	if (mysql_real_connect(
		_connection,
		host.c_str(),
		username.c_str(), password.c_str(), dbname.c_str(),
		0, NULL, 0
	)) {
		_isActive = true;
		return true;
	}
    return false;
}

void CDbConnection::close()
{
	mysql_close(_connection);
	_isActive = false;
}

MYSQL * CDbConnection::getConnection() const
{
	return _connection;
}

void CDbConnection::applyConfig(const xml_node & config)
{
	if (!config.child("cdn").empty()) {
		connectionString = config.child("cdn").attribute("value").value();
	}
	if (!config.child("username").empty()) {
		username = config.child("username").attribute("value").value();
	}
	if (!config.child("password").empty()) {
		password = config.child("password").attribute("value").value();
	}
}

void CDbConnection::setActive(bool value)
{
	_isActive = value;
}

bool CDbConnection::setActive() const
{
	return _isActive;
}

CDbCommand CDbConnection::createCommand(const string & query)
{
	setActive(true);
	return CDbCommand(this, query);
}

CDbCommandBuilder * CDbConnection::getCommandBuilder()
{
	return _commandBuilder;
}
