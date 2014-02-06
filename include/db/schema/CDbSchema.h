/*
 * CDbSchema.h
 *
 *  Created on: Feb 5, 2014
 *      Author: djvibegga
 */

#ifndef CDBSCHEMA_H_
#define CDBSCHEMA_H_

#include "base/CComponent.h"

class CDbSchema: public CComponent
{
public:
	CDbSchema();
	virtual ~CDbSchema();
	virtual string quoteTableName(const string & tableName);
};

#endif /* CDBSCHEMA_H_ */
