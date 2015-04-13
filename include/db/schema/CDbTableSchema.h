/*
 * CDbTableSchema.h
 *
 *  Created on: Apr 12, 2015
 *      Author: djvibegga
 */

#ifndef CDBTABLESCHEMA_H_
#define CDBTABLESCHEMA_H_

#include "interfaces.h"

class CDbTableSchema
{
private:
	string _tableName;
	vector<string> _pk;

public:
	virtual ~CDbTableSchema();
	void setTableName(const string & tableName);
	string getTableName() const;
	void setPrimaryKey(const vector<string> & key);
	vector<string> getPrimaryKey() const;
};

#endif /* CDBTABLESCHEMA_H_ */
