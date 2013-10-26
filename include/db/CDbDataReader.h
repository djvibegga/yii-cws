/*
 * CDbDataReader.h
 *
 *  Created on: Oct 26, 2013
 *      Author: djvibegga
 */

#ifndef CDBDATAREADER_H_
#define CDBDATAREADER_H_

#include "base/CComponent.h"
#include "db/CDbCommand.h"
#include <map>
#include <vector>
#include <mysql/mysql.h>

using namespace std;

typedef void* TDbColumnValue;
typedef map<string, TDbColumnValue> TDbRow;

class CDbDataReader: public CComponent
{
private:
	MYSQL_STMT * _statement;
	bool _closed;
	TDbRow _row;
	int _index;

public:
	CDbDataReader(const CDbCommand & command);
	~CDbDataReader();
};

#endif /* CDBDATAREADER_H_ */
