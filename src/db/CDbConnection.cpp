#include "db/CDbConnection.h"

#include <boost/regex.hpp>

CDbConnection::CDbConnection()
:
  _connection(0),
  connectionString(""),
  username(""),
  password("")
{
}

CDbConnection::CDbConnection(string dsn, string username, string password)
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
    return mysql_real_connect(
    	_connection,
    	host.c_str(),
    	username.c_str(), password.c_str(), dbname.c_str(),
    	0, NULL, 0
    );
}

void CDbConnection::close()
{
	mysql_close(_connection);
}

MYSQL * CDbConnection::getConnection()
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
