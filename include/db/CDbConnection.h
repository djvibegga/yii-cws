#ifndef __C_DB_CONNECTION_H__
#define __C_DB_CONNECTION_H__

using namespace std;

#include <string>
#include <sqlapi/SQLAPI.h>
#include <base/CApplicationComponent.h>
#include "base/CException.h"

class CDbCommand;
class CDbCommandBuilder;

class CDbConnection: public CApplicationComponent
{
private:
	SAConnection * _saConnection;
	bool _isActive;
	CDbCommandBuilder * _commandBuilder;

public:
	string connectionString;
	string username;
	string password;

	CDbConnection(const CModule * module);
	CDbConnection(const CModule * module, string dsn, string username, string password);
	bool open() throw(CException);
	void close();
	SAConnection * getConnection() const;
	void setActive(bool value);
	bool setActive() const;
	virtual void applyConfig(const xml_node & config);
	CDbCommand createCommand(const string & query);
	CDbCommandBuilder * getCommandBuilder();
};

#endif //__C_DB_CONNECTION_H__
