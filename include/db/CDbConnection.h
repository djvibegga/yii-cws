#ifndef __C_DB_CONNECTION_H__
#define __C_DB_CONNECTION_H__

using namespace std;

#include <string>
#include <sqlapi/SQLAPI.h>
#include "base/CApplicationComponent.h"
#include "base/CException.h"
#include "db/schema/CDbSchema.h"

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
	CDbSchema _schema;

public:

	string connectionString;
	string username;
	string password;
	static SAConnection glConnection;

	CDbConnection(CModule * module);
	CDbConnection(CModule * module, string dsn, string username, string password);
	virtual string getClassName() const;
	virtual ~CDbConnection();
	bool open() throw(CException);
	void close();
	virtual void init();
	SAConnection * getConnection() const;
	void setActive(bool value);
	bool getActive() const;
	virtual void applyConfig(const xml_node & config);
	CDbCommand createCommand(const string & query);
	void setCommandBuilder(CDbCommandBuilder * builder);
	CDbCommandBuilder * getCommandBuilder() const;
	const CDbSchema & getSchema();
	unsigned long getLastInsertId(const string & tableName = "");

	static void threadInit() throw (CException);
	static void threadEnd() throw (CException);

protected:
	virtual void initSchema(CDbSchema & schema) = 0;
};

#endif //__C_DB_CONNECTION_H__
