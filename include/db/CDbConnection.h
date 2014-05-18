#ifndef __C_DB_CONNECTION_H__
#define __C_DB_CONNECTION_H__

using namespace std;

#include <string>
#include <sqlapi/SQLAPI.h>
#include <base/CApplicationComponent.h>
#include "base/CException.h"

#ifdef _UNICODE
#define STRING_FROM_SA_FIELD(sa) sa->asString().getWideChars()
#else
#define STRING_FROM_SA_FIELD(sa) sa->asString().GetMultiByteChars()
#endif

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
	//static SAConnection glConnection;

	CDbConnection(CModule * module);
	CDbConnection(CModule * module, string dsn, string username, string password);
	bool open() throw(CException);
	void close();
	virtual void init();
	SAConnection * getConnection() const;
	void setActive(bool value);
	bool setActive() const;
	virtual void applyConfig(const xml_node & config);
	CDbCommand createCommand(const string & query);
	void setCommandBuilder(CDbCommandBuilder * builder);
	CDbCommandBuilder * getCommandBuilder() const;

	static void threadInit() throw (CException);
	static void threadEnd() throw (CException);
};

#endif //__C_DB_CONNECTION_H__
