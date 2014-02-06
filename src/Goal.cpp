/*
 * Goal.cpp
 *
 *  Created on: Feb 5, 2014
 *      Author: djvibegga
 */

#include "Goal.h"


IMPLEMENT_AR_CLASS(Goal)

BEGIN_ATTRIBUTE_RESOLVER(Goal)
	ATTRIBUTE_RESOLVE("id", id, AR_ATTRIBUTE_ULONG)
	ATTRIBUTE_RESOLVE("name", name, AR_ATTRIBUTE_STRING)
END_ATTRIBUTE_RESOLVER()

Goal::Goal(const string & scenario, CDbConnection * connection)
: CActiveRecord(scenario, connection),
  id(0)
{
}

Goal::Goal(const Goal & other)
: CActiveRecord(other.getScenario(), other.getDbConnection()),
  id(other.id)
{

}

Goal::~Goal()
{
}

string Goal::tableName() const
{
	return "goals";
}
