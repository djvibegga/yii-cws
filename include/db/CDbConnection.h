#ifndef __C_DB_CONNECTION_H__
#define __C_DB_CONNECTION_H__

using namespace std;

#include <string>
#include <mysql/mysql.h>
#include <base/CComponent.h>
#include "base/CException.h"
#include "interfaces.h"

class CDbConnection: public CComponent, public IConfigureable
{
private:

	MYSQL * _connection;

public:

	string connectionString;

	string username;

	string password;

	CDbConnection();
	CDbConnection(string dsn, string username, string password);
	bool open() throw(CException);
	void close();
	MYSQL * getConnection();
	virtual void applyConfig(const xml_node & config);
};

#endif //__C_DB_CONNECTION_H__
