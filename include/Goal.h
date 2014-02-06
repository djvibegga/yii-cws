/*
 * Goal.h
 *
 *  Created on: Feb 5, 2014
 *      Author: djvibegga
 */

#ifndef GOAL_H_
#define GOAL_H_

#include <db/ar/CActiveRecord.h>

class Goal: public CActiveRecord
{
	DECLARE_AR_CLASS(Goal)
	DECLARE_ATTRIBUTE_RESOLVER()

public:
	unsigned long id;
	string name;

	Goal(const string & scenario = "", CDbConnection * connection = 0);
	Goal(const Goal & other);
	virtual ~Goal();
	virtual string tableName() const;
};

#endif /* GOAL_H_ */
