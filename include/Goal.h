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

public:
	unsigned long id;
	string name;

	Goal(const string & scenario = "", CDbConnection * connection = 0);
	Goal(const Goal & other);
	virtual ~Goal();
	virtual CActiveRecord * instantiate(const TDbRow & attributes) const;
	virtual string tableName() const;
	virtual void populateProperty(const string & name, const SAField * value);
};

#endif /* GOAL_H_ */
