#include "db/CDbCommand.h"
#include "db/schema/CDbCommandBuilder.h"
#include "db/CDbConnection.h"
#include <boost/regex.hpp>
#include <base/Jvibetto.h>
#include <sqlapi/myAPI.h>

SAConnection CDbConnection::glConnection;

CDbConnection::CDbConnection(CModule * module)
: CApplicationComponent(module),
  _saConnection(0),
  _isActive(false),
  _commandBuilder(0),
  connectionString(""),
  username(""),
  password("")
{
}

CDbConnection::CDbConnection(CModule * module, string dsn, string username, string password)
: CApplicationComponent(module),
  _saConnection(0),
  _isActive(false),
  _commandBuilder(0)
{
	connectionString = dsn;
	this->username = username;
	this->password = password;
}

string CDbConnection::getClassName() const
{
	return "CDbConnection";
}

CDbConnection::~CDbConnection()
{
	if (_commandBuilder == 0) {
		delete _commandBuilder;
	}
}

bool CDbConnection::open() throw(CException)
{
	boost::regex pattern("(\\w+):host=([\\d\\.]+);port=(\\d+);dbname=(\\w+)");
	boost::smatch match;

	_saConnection = new SAConnection();
	_saConnection->setClient(SA_MySQL_Client);
	string driver, host, port, dbname = "";
	if (!boost::regex_match(connectionString, match, pattern)) {
		string message = "Invalid connection string passed. ";
		message.append(connectionString);
		throw CException(message, 0, __FILE__, __LINE__);
	} else {
		driver = match[1];
		host = match[2];
		port = match[3];
		dbname = match[4];
	}
	try {
		_saConnection->Connect((host + ":" + port + "@" + dbname).c_str(), username.c_str(), password.c_str(), SA_MySQL_Client);
	} catch (SAException & e) {
	    Jvibetto::log(e.ErrText().GetMultiByteChars(), CLogger::LEVEL_ERROR);
		return false;
	}
	_isActive = true;

	if (_commandBuilder == 0) {
		_commandBuilder = new CDbCommandBuilder(this, _schema);
	}

	return true;
}

void CDbConnection::close()
{
	_saConnection->Disconnect();
	delete _saConnection;
	_saConnection = 0;
	_isActive = false;
	if (_commandBuilder == 0) {
		delete _commandBuilder;
	}
}

void CDbConnection::init()
{
	CApplicationComponent::init();
	initSchema(_schema);
}

SAConnection * CDbConnection::getConnection() const
{
	return _saConnection;
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

bool CDbConnection::getActive() const
{
	return _isActive;
}

CDbCommand CDbConnection::createCommand(const string & query)
{
	setActive(true);
	return CDbCommand(this, query);
}

CDbCommandBuilder * CDbConnection::getCommandBuilder() const
{
	return _commandBuilder;
}

const CDbSchema & CDbConnection::getSchema()
{
	return _schema;
}

void CDbConnection::setCommandBuilder(CDbCommandBuilder * builder)
{
	_commandBuilder = builder;
}

void CDbConnection::threadInit() throw (CException)
{
	if (glConnection.Client() == SA_MySQL_Client) {
		myAPI * nativeAPI = (myAPI*)glConnection.NativeAPI();
		if (nativeAPI) {
			nativeAPI->mysql_thread_init();
			return;
		}
	}
	throw CException("Can\'t notify about thread is created via native API.");
}

void CDbConnection::threadEnd() throw (CException)
{
	if (glConnection.Client() == SA_MySQL_Client) {
		myAPI * nativeAPI = (myAPI*)glConnection.NativeAPI();
		if (nativeAPI) {
			nativeAPI->mysql_thread_end();
			return;
		}
	}
	throw CException("Can\'t notify about thread is terminated via native API.");
}

unsigned long CDbConnection::getLastInsertId(const string & tableName)
{
	CDbCommand command = createCommand("SELECT LAST_INSERT_ID()");
	return command.queryScalar().asULong();
}
